#include "distrho_plugin_server.h"
#include "distrho_audio_port.h"
#include "distrho_circular_buffer.h"
#include "distrho_common.h"
#include "distrho_config.h"
#include "distrho_launcher.h"
#include "distrho_midi_event.h"
#include "distrho_parameter.h"
#include "distrho_plugin_instance.h"
#include "distrho_plugin_server_node.h"
#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory_rpc.h"
// #include "distrho_plugin_client.h"
#include "godot_cpp/classes/audio_server.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/os.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/classes/time.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_distrho_schema.capnp.h"
#include "version_generated.gen.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <capnp/serialize.h>
#include <cstdlib>
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/core/mutex_lock.hpp>
#include <kj/string.h>
#include <unistd.h>

using namespace godot;
using namespace boost::interprocess;
using namespace boost::posix_time;

DistrhoPluginServer *DistrhoPluginServer::singleton = NULL;

DistrhoPluginServer::DistrhoPluginServer() {
    process_sample_frame_size = 1024;
    buffer_start_time_usec = 0;

    const char *module_type = std::getenv("DISTRHO_MODULE_TYPE");
    if (module_type == NULL) {
        module_type = std::to_string(DistrhoCommon::PLUGIN_TYPE).c_str();
    }

    is_plugin = std::stoi(module_type) == DistrhoCommon::PLUGIN_TYPE;

    initialized = false;
    active = false;
    exit_thread = false;
    singleton = this;

    distrho_config = memnew(DistrhoConfig);
    distrho_plugin = memnew(DistrhoPluginInstance);

    if (is_plugin) {
        const char *audio_shared_memory = std::getenv("DISTRHO_SHARED_MEMORY_AUDIO");
        if (audio_shared_memory == NULL) {
            audio_shared_memory = "";
        }
        audio_memory = new DistrhoSharedMemoryAudio();
        audio_memory->initialize(0, 0, audio_shared_memory);

        const char *rpc_shared_memory = std::getenv("DISTRHO_SHARED_MEMORY_RPC");
        if (rpc_shared_memory == NULL) {
            rpc_shared_memory = "";
        }
        rpc_memory = new DistrhoSharedMemoryRPC();
        rpc_memory->initialize("DISTRHO_SHARED_MEMORY_RPC", rpc_shared_memory);

        const char *godot_rpc_shared_memory = std::getenv("GODOT_SHARED_MEMORY_RPC");
        if (godot_rpc_shared_memory == NULL) {
            godot_rpc_shared_memory = "";
        }
        godot_rpc_memory = new DistrhoSharedMemoryRPC();
        godot_rpc_memory->initialize("GODOT_SHARED_MEMORY_RPC", godot_rpc_shared_memory);

        // client = new DistrhoPluginClient(godot_rpc_memory);

        // TODO: use the correct number of channels instad of num_channels (16)
        for (int i = 0; i < num_channels; ++i) {
            input_buffer[i] = input_data[i];
            output_buffer[i] = output_data[i];
        }

        input_channels.resize(audio_memory->get_num_input_channels());
        output_channels.resize(audio_memory->get_num_output_channels());

        for (int channel = 0; channel < audio_memory->get_num_input_channels(); channel++) {
            input_channels.write[channel] = new DistrhoCircularBuffer();
        }

        for (int channel = 0; channel < audio_memory->get_num_output_channels(); channel++) {
            output_channels.write[channel] = new DistrhoCircularBuffer();
        }

        call_deferred("initialize");
    }
}

DistrhoPluginServer::~DistrhoPluginServer() {
    // delete distrho_shared_memory_audio;
    // delete distrho_shared_memory_rpc;

    for (int channel = 0; channel < input_channels.size(); channel++) {
        delete input_channels[channel];
    }

    for (int channel = 0; channel < output_channels.size(); channel++) {
        delete output_channels[channel];
    }

    // delete client;
    singleton = NULL;
}

DistrhoPluginServer *DistrhoPluginServer::get_singleton() {
    return singleton;
}

void DistrhoPluginServer::initialize() {
    if (!initialized) {
        Node *distrho_server_node = memnew(DistrhoPluginServerNode);
        SceneTree *tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
        tree->get_root()->add_child(distrho_server_node);
        distrho_server_node->set_process(true);

        parameters.resize(distrho_plugin->_get_parameters().size());

        for (int i = 0; i < distrho_plugin->_get_parameters().size(); i++) {
            parameters.set(i, distrho_plugin->_get_parameters().get(i)->get_default_value());
        }
    }

    start();
    initialized = true;
}

void DistrhoPluginServer::audio_thread_func() {
    scoped_lock<interprocess_mutex> shared_memory_lock(audio_memory->buffer->mutex);
    audio_memory->buffer->ready = true;
    shared_memory_lock.unlock();

    active = true;

    static MidiEvent midi_input[godot::MIDI_BUFFER_SIZE];
    static MidiEvent midi_output[godot::MIDI_BUFFER_SIZE];

    bool first_wait = true;

    while (!exit_thread) {
        scoped_lock<interprocess_mutex> shared_memory_lock(audio_memory->buffer->mutex);

        if (initialized) {
            start_buffer_processing();
            AudioServer::get_singleton()->process_external(BUFFER_FRAME_SIZE);
        }

        ptime timeout = microsec_clock::universal_time() + milliseconds(first_wait ? 1000 : 100);
        bool result = audio_memory->buffer->input_condition.timed_wait(shared_memory_lock, timeout);
        first_wait = false;

        if (result) {
            audio_memory->read_input_channel(input_buffer, BUFFER_FRAME_SIZE);
            audio_memory->advance_input_read_index(BUFFER_FRAME_SIZE);
            int midi_input_size = audio_memory->read_input_midi(midi_input);

            if (midi_input_size > 0) {
                std::lock_guard<std::mutex> lock(midi_input_mutex);
                for (int i = 0; i < midi_input_size; i++) {
                    midi_input_queue.push(midi_input[i]);
                }
            }

            int midi_output_size = 0;
            std::lock_guard<std::mutex> lock(midi_output_mutex);

            while (!midi_output_queue.empty()) {
                MidiEvent midi_event = midi_output_queue.front();
                midi_output_queue.pop();

                midi_output[midi_output_size].size = midi_event.size;
                midi_output[midi_output_size].data[0] = midi_event.data[0];
                midi_output[midi_output_size].data[1] = midi_event.data[1];
                midi_output[midi_output_size].data[2] = midi_event.data[2];
                midi_output[midi_output_size].frame = midi_event.frame;

                midi_output_size = midi_output_size + 1;
            }

            for (int channel = 0; channel < audio_memory->get_num_input_channels(); channel++) {
                input_channels.write[channel]->write_channel(input_buffer[channel], BUFFER_FRAME_SIZE);
            }

            for (int channel = 0; channel < audio_memory->get_num_output_channels(); channel++) {
                output_channels[channel]->read_channel(output_buffer[channel], BUFFER_FRAME_SIZE);
            }

            audio_memory->write_output_channel(output_buffer, BUFFER_FRAME_SIZE);
            audio_memory->advance_output_write_index(BUFFER_FRAME_SIZE);
            audio_memory->write_output_midi(midi_output, midi_output_size);

            audio_memory->buffer->output_condition.notify_one();
        } else {
            /*
            if (!audio_memory->get_is_host()) {
                // TODO: remove duplicate
                exit_thread = true;
                SceneTree *tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
                tree->quit();
            }
            break;
            */
        }
    }
    delete audio_memory;
}

template <typename T, typename R>
void DistrhoPluginServer::handle_rpc_call(
    std::function<void(typename T::Reader &, typename R::Builder &)> handle_request) {
    return DistrhoCommon::handle_rpc_call<T, R>(*rpc_memory, handle_request);
}

void DistrhoPluginServer::rpc_thread_func() {
    {
        scoped_lock<interprocess_mutex> shared_memory_lock(rpc_memory->buffer->mutex);
        rpc_memory->buffer->ready = true;
    }

    bool first_wait = true;

    while (!exit_thread) {
        scoped_lock<interprocess_mutex> shared_memory_lock(rpc_memory->buffer->mutex);

        ptime timeout = microsec_clock::universal_time() + milliseconds(first_wait ? 1000 : 100);
        bool result = rpc_memory->buffer->input_condition.timed_wait(
            shared_memory_lock, timeout, [this]() { return rpc_memory->buffer->request_id != 0; });
        first_wait = false;

        if (result) {
            switch (rpc_memory->buffer->request_id) {

            case GetLabelRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetLabelRequest, GetLabelResponse>([](auto &request, auto &response) {
                    String label = DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_label();
                    response.setLabel(std::string(label.ascii()));
                });
                break;
            }

            case GetDescriptionRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetDescriptionRequest, GetDescriptionResponse>([](auto &request, auto &response) {
                    String description = DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_description();
                    response.setDescription(std::string(description.ascii()));
                });
                break;
            }

            case GetMakerRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetMakerRequest, GetMakerResponse>([](auto &request, auto &response) {
                    String maker = DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_maker();
                    response.setMaker(std::string(maker.ascii()));
                });
                break;
            }

            case GetHomePageRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetHomePageRequest, GetHomePageResponse>([](auto &request, auto &response) {
                    String homepage = DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_homepage();
                    response.setHomePage(std::string(homepage.ascii()));
                });
                break;
            }

            case GetLicenseRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetLicenseRequest, GetLicenseResponse>([](auto &request, auto &response) {
                    String license = DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_license();
                    response.setLicense(std::string(license.ascii()));
                });
                break;
            }

            case GetVersionRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetVersionRequest, GetVersionResponse>([](auto &request, auto &response) {
                    String value = DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_version();
                    PackedStringArray version = value.split(".");
                    if (version.size() == 3) {
                        response.setMajor(version.get(0).to_int());
                        response.setMinor(version.get(1).to_int());
                        response.setPatch(version.get(2).to_int());
                    }
                });
                break;
            }

            case GetUniqueIdRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetUniqueIdRequest, GetUniqueIdResponse>([](auto &request, auto &response) {
                    String unique_id = DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_unique_id();
                    if (unique_id.length() >= 4) {
                        response.setUniqueId(std::string(unique_id.ascii()));
                    }
                });
                break;
            }

            case GetNumberOfInputPortsRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetNumberOfInputPortsRequest, GetNumberOfInputPortsResponse>(
                    [](auto &request, auto &response) {
                        int number_of_input_ports =
                            DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_input_ports().size();
                        response.setNumberOfInputPorts(number_of_input_ports);
                    });
                break;
            }

            case GetNumberOfOutputPortsRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetNumberOfOutputPortsRequest, GetNumberOfOutputPortsResponse>(
                    [](auto &request, auto &response) {
                        int number_of_input_ports =
                            DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_output_ports().size();
                        response.setNumberOfOutputPorts(number_of_input_ports);
                    });
                break;
            }

            case GetInputPortRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetInputPortRequest, GetInputPortResponse>([](auto &request, auto &response) {
                    Vector<Ref<DistrhoAudioPort>> input_ports =
                        DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_input_ports();
                    if (request.getIndex() < input_ports.size()) {
                        Ref<DistrhoAudioPort> port = input_ports[request.getIndex()];
                        response.setHints(port->get_hints());
                        response.setName(std::string(port->get_name().ascii()));
                        response.setSymbol(std::string(port->get_symbol().ascii()));
                        response.setGroupId(port->get_group_id());
                        response.setResult(true);
                    } else {
                        response.setResult(false);
                    }
                });
                break;
            }

            case GetOutputPortRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetOutputPortRequest, GetOutputPortResponse>([](auto &request, auto &response) {
                    Vector<Ref<DistrhoAudioPort>> output_ports =
                        DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_output_ports();
                    if (request.getIndex() < output_ports.size()) {
                        Ref<DistrhoAudioPort> port = output_ports[request.getIndex()];
                        response.setHints(port->get_hints());
                        response.setName(std::string(port->get_name().ascii()));
                        response.setSymbol(std::string(port->get_symbol().ascii()));
                        response.setGroupId(port->get_group_id());
                        response.setResult(true);
                    } else {
                        response.setResult(false);
                    }
                });
                break;
            }

            case GetParameterRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetParameterRequest, GetParameterResponse>([this](auto &request, auto &response) {
                    Ref<DistrhoParameter> parameter =
                        DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_parameters().get(
                            request.getIndex());
                    if (parameter.is_valid()) {
                        response.setHints(parameter->get_hints());
                        response.setName(std::string(parameter->get_name().ascii()));
                        response.setShortName(std::string(parameter->get_short_name().ascii()));
                        response.setSymbol(std::string(parameter->get_symbol().ascii()));
                        response.setUnit(std::string(parameter->get_unit().ascii()));
                        response.setDescription(std::string(parameter->get_description().ascii()));

                        response.setDefaultValue(parameter->get_default_value());
                        response.setMinValue(parameter->get_min_value());
                        response.setMaxValue(parameter->get_max_value());

                        // TODO: enumeration_values
                        response.setEnumerationValues("");
                        response.setDesignation(parameter->get_designation());

                        response.setMidiCC(parameter->get_midi_cc());
                        response.setGroupId(parameter->get_group_id());
                        response.setResult(true);
                    } else {
                        response.setResult(false);
                    }
                });
                break;
            }

            case GetParameterValueRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetParameterValueRequest, GetParameterValueResponse>(
                    [this](auto &request, auto &response) { response.setValue(parameters.get(request.getIndex())); });
                break;
            }

            case SetParameterValueRequest::_capnpPrivate::typeId: {
                handle_rpc_call<SetParameterValueRequest, SetParameterValueResponse>(
                    [this](auto &request, auto &response) {
                        parameters.set(request.getIndex(), request.getValue());
                        call_deferred("emit_signal", "parameter_changed", request.getIndex(), request.getValue());
                    });
                break;
            }

            case GetParameterCountRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetParameterCountRequest, GetParameterCountResponse>(
                    [this](auto &request, auto &response) {
                        response.setCount(
                            DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_parameters().size());
                    });
                break;
            }

            case GetProgramCountRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetProgramCountRequest, GetProgramCountResponse>(
                    [this](auto &request, auto &response) { response.setCount(0); });
                break;
            }

            case GetStateCountRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetStateCountRequest, GetStateCountResponse>(
                    [this](auto &request, auto &response) { response.setCount(0); });
                break;
            }

            case ShutdownRequest::_capnpPrivate::typeId: {
                handle_rpc_call<ShutdownRequest, ShutdownResponse>([this](auto &request, auto &response) {
                    // TODO: remove duplicate
                    exit_thread = true;
                    SceneTree *tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
                    tree->quit();
                    response.setResult(true);
                });
                break;
            }

            default: {
                break;
            }
            }
            rpc_memory->buffer->request_id = 0;
        }
    }
    delete rpc_memory;
}

void DistrhoPluginServer::process() {
    std::lock_guard<std::mutex> lock(midi_input_mutex);

    while (!midi_input_queue.empty()) {
        MidiEvent midi_event = midi_input_queue.front();
        midi_input_queue.pop();

        emit_midi_event(midi_event);
    }
}

void DistrhoPluginServer::emit_midi_event(MidiEvent &p_midi_event) {
    Ref<DistrhoMidiEvent> midi_event = Ref<DistrhoMidiEvent>();
    midi_event.instantiate();

    uint8_t channel = p_midi_event.data[0] & 0x0F;
    uint8_t status = p_midi_event.data[0] & 0xF0;

    midi_event->set_channel(channel);
    midi_event->set_status(status);
    midi_event->set_data1(p_midi_event.data[1]);
    midi_event->set_data2(p_midi_event.data[2]);
    midi_event->set_frame(p_midi_event.frame);

    switch (status) {
    // TODO: make variables for note on, off, etc.
    case 0x90: // Note On
        if (midi_event->get_data2() > 0) {
            emit_signal("midi_note_on", channel, midi_event->get_data1(), midi_event->get_data2(),
                        midi_event->get_frame());
        } else {
            emit_signal("midi_note_off", channel, midi_event->get_data1(), midi_event->get_data2(),
                        midi_event->get_frame());
        }
        break;

    case 0x80: // Note Off
        emit_signal("midi_note_off", channel, midi_event->get_data1(), midi_event->get_data2(),
                    midi_event->get_frame());
        break;

    case 0xB0: // Control Change
        emit_signal("midi_cc", channel, midi_event->get_data1(), midi_event->get_data2(), midi_event->get_frame());
        break;

    case 0xC0: // Program Change
        emit_signal("midi_program_change", channel, midi_event->get_data1(), midi_event->get_frame());
        break;

    case 0xD0: // Channel Pressure (Aftertouch)
        emit_signal("midi_channel_pressure", channel, midi_event->get_data1(), midi_event->get_frame());
        break;

    case 0xE0: { // Pitch Bend
        int lsb = midi_event->get_data1() & 0x7F;
        int msb = midi_event->get_data2() & 0x7F;
        int pitch_bend = (msb << 7) | lsb; // 14-bit value
        emit_signal("midi_pitch_bend", channel, pitch_bend, midi_event->get_frame());
        break;
    }
    case 0xA0: // Polyphonic Aftertouch
        emit_signal("midi_poly_aftertouch", channel, midi_event->get_data1(), midi_event->get_data2(),
                    midi_event->get_frame());
        break;
    default:
        break;
    }

    emit_signal("midi_event", midi_event);
}

void DistrhoPluginServer::send_midi_event(Ref<DistrhoMidiEvent> p_midi_event) {
    MidiEvent event;
    event.size = 3;
    event.frame = get_frame_offset_for_event(Time::get_singleton()->get_ticks_usec());
    event.data[0] = p_midi_event->get_status();
    event.data[1] = p_midi_event->get_data1();
    event.data[2] = p_midi_event->get_data2();

    std::lock_guard<std::mutex> lock(midi_output_mutex);
    midi_output_queue.push(event);
}

void DistrhoPluginServer::note_on(int p_channel, int p_note, int p_velocity) {
    MidiEvent event;
    event.size = 3;
    event.frame = get_frame_offset_for_event(Time::get_singleton()->get_ticks_usec());
    event.data[0] = 0x90 | (p_channel & 0x0F);
    event.data[1] = p_note & 0x7F;
    event.data[2] = p_velocity & 0x7F;

    std::lock_guard<std::mutex> lock(midi_output_mutex);
    midi_output_queue.push(event);
}

void DistrhoPluginServer::note_off(int p_channel, int p_note, int p_velocity) {
    MidiEvent event;
    event.size = 3;
    event.frame = get_frame_offset_for_event(Time::get_singleton()->get_ticks_usec());
    event.data[0] = 0x80 | (p_channel & 0x0F);
    event.data[1] = p_note & 0x7F;
    event.data[2] = p_velocity & 0x7F;

    std::lock_guard<std::mutex> lock(midi_output_mutex);
    midi_output_queue.push(event);
}

void DistrhoPluginServer::program_change(int p_channel, int p_program_number) {
    MidiEvent event;
    event.size = 2;
    event.frame = get_frame_offset_for_event(Time::get_singleton()->get_ticks_usec());
    event.data[0] = 0xC0 | (p_channel & 0x0F);
    event.data[1] = p_program_number & 0x7F;
    event.data[2] = 0;

    std::lock_guard<std::mutex> lock(midi_output_mutex);
    midi_output_queue.push(event);
}

void DistrhoPluginServer::control_change(int p_channel, int p_controller, int p_value) {
    MidiEvent event;
    event.size = 3;
    event.frame = get_frame_offset_for_event(Time::get_singleton()->get_ticks_usec());
    event.data[0] = 0xB0 | (p_channel & 0x0F);
    event.data[1] = p_controller & 0x7F;
    event.data[2] = p_value & 0x7F;

    std::lock_guard<std::mutex> lock(midi_output_mutex);
    midi_output_queue.push(event);
}

void DistrhoPluginServer::pitch_bend(int p_channel, int p_value) {
    p_value = std::clamp(p_value, 0, 16383);

    MidiEvent event;
    event.size = 3;
    event.frame = get_frame_offset_for_event(Time::get_singleton()->get_ticks_usec());
    event.data[0] = 0xE0 | (p_channel & 0x0F);
    event.data[1] = p_value & 0x7F;
    event.data[2] = (p_value >> 7) & 0x7F;

    std::lock_guard<std::mutex> lock(midi_output_mutex);
    midi_output_queue.push(event);
}

void DistrhoPluginServer::channel_pressure(int p_channel, int p_pressure) {
    p_pressure = std::clamp(p_pressure, 0, 127);

    MidiEvent event;
    event.size = 2;
    event.frame = get_frame_offset_for_event(Time::get_singleton()->get_ticks_usec());
    event.data[0] = 0xD0 | (p_channel & 0x0F);
    event.data[1] = p_pressure & 0x7F;
    event.data[2] = 0;

    std::lock_guard<std::mutex> lock(midi_output_mutex);
    midi_output_queue.push(event);
}

void DistrhoPluginServer::midi_poly_aftertouch(int p_channel, int p_note, int p_pressure) {
    MidiEvent event;
    event.size = 3;
    event.frame = get_frame_offset_for_event(Time::get_singleton()->get_ticks_usec());
    event.data[0] = 0xA0 | (p_channel & 0x0F);
    event.data[1] = p_note;
    event.data[2] = p_pressure;

    std::lock_guard<std::mutex> lock(midi_output_mutex);
    midi_output_queue.push(event);
}

float DistrhoPluginServer::get_parameter_value(int p_index) {
    return parameters[p_index];
}

void DistrhoPluginServer::set_parameter_value(int p_index, float p_value) {
    parameters.ptrw()[p_index] = p_value;
}

void DistrhoPluginServer::start_buffer_processing() {
    buffer_start_time_usec = Time::get_singleton()->get_ticks_usec();
}

uint32_t DistrhoPluginServer::get_frame_offset_for_event(uint64_t p_event_time_usec) {
    if (p_event_time_usec < buffer_start_time_usec) {
        return 0;
    }

    uint64_t delta_usec = p_event_time_usec - buffer_start_time_usec;
    float mix_rate = AudioServer::get_singleton()->get_mix_rate();
    uint32_t frame_offset = static_cast<uint32_t>((delta_usec * mix_rate) / 1'000'000);

    if (frame_offset >= static_cast<uint32_t>(process_sample_frame_size)) {
        frame_offset = process_sample_frame_size - 1;
    }

    return frame_offset;
}

int DistrhoPluginServer::process_sample(AudioFrame *p_buffer, float p_rate, int p_frames) {
    if (!is_plugin) {
        return p_frames;
    }

    lock_audio();

    if (process_sample_frame_size != p_frames) {
        process_sample_frame_size = p_frames;
    }

    if (audio_memory->get_num_input_channels() == 0) {
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].left = 0;
            p_buffer[frame].right = 0;
        }
    } else if (audio_memory->get_num_input_channels() == 1) {
        input_channels[0]->read_channel(temp_buffer, p_frames);
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].left = temp_buffer[frame];
            p_buffer[frame].right = temp_buffer[frame];
        }
    } else {
        input_channels[0]->read_channel(temp_buffer, p_frames);
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].left = temp_buffer[frame];
        }

        input_channels[1]->read_channel(temp_buffer, p_frames);
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].right = temp_buffer[frame];
        }
    }

    unlock_audio();

    // semaphore->post();

    return p_frames;
}

void DistrhoPluginServer::set_channel_sample(AudioFrame *p_buffer, float p_rate, int p_frames, int left, int right) {
    bool has_left_channel = left >= 0 && left < output_channels.size();
    bool has_right_channel = right >= 0 && right < output_channels.size();

    if (!has_left_channel && !has_right_channel && !active) {
        return;
    }

    lock_audio();

    if (has_left_channel) {
        for (int frame = 0; frame < p_frames; frame++) {
            temp_buffer[frame] = p_buffer[frame].left;
        }
        output_channels.write[left]->write_channel(temp_buffer, p_frames);
    }

    if (has_right_channel) {
        for (int frame = 0; frame < p_frames; frame++) {
            temp_buffer[frame] = p_buffer[frame].right;
        }
        output_channels.write[right]->write_channel(temp_buffer, p_frames);
    }

    unlock_audio();
}

int DistrhoPluginServer::get_channel_sample(AudioFrame *p_buffer, float p_rate, int p_frames, int left, int right) {
    bool has_left_channel = left >= 0 && left < input_channels.size();
    bool has_right_channel = right >= 0 && right < input_channels.size();

    lock_audio();
    if (has_left_channel && active) {
        input_channels[left]->read_channel(temp_buffer, p_frames);
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].left = temp_buffer[frame];
        }
    } else {
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].left = 0;
        }
    }
    if (has_right_channel && active) {
        input_channels[right]->read_channel(temp_buffer, p_frames);
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].right = temp_buffer[frame];
        }
    } else {
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].right = 0;
        }
    }
    unlock_audio();

    return p_frames;
}

Error DistrhoPluginServer::start() {
    if (!godot::Engine::get_singleton()->is_editor_hint()) {
        if (is_plugin) {
            audio_thread.instantiate();
            audio_mutex.instantiate();
            audio_thread->start(callable_mp(this, &DistrhoPluginServer::audio_thread_func), Thread::PRIORITY_HIGH);
        }

        rpc_thread.instantiate();
        rpc_thread->start(callable_mp(this, &DistrhoPluginServer::rpc_thread_func), Thread::PRIORITY_NORMAL);
    }
    return OK;
}

void DistrhoPluginServer::lock_audio() {
    if (audio_thread.is_null() || audio_mutex.is_null()) {
        return;
    }
    audio_mutex->lock();
}

void DistrhoPluginServer::unlock_audio() {
    if (audio_thread.is_null() || audio_mutex.is_null()) {
        return;
    }
    audio_mutex->unlock();
}

void DistrhoPluginServer::finish() {
    if (!godot::Engine::get_singleton()->is_editor_hint()) {
        exit_thread = true;
        if (is_plugin) {
            audio_thread->wait_to_finish();
        }
        rpc_thread->wait_to_finish();
    }
}

DistrhoConfig *DistrhoPluginServer::get_config() {
    return distrho_config;
}

godot::String DistrhoPluginServer::get_version() {
    return GODOT_DISTRHO_VERSION;
}

godot::String DistrhoPluginServer::get_build() {
    return GODOT_DISTRHO_BUILD;
}

void DistrhoPluginServer::set_distrho_launcher(DistrhoLauncher *p_distrho_launcher) {
    distrho_launcher = p_distrho_launcher;
}

DistrhoLauncher *DistrhoPluginServer::get_distrho_launcher() {
    return distrho_launcher;
}

void DistrhoPluginServer::set_distrho_plugin(DistrhoPluginInstance *p_distrho_plugin) {
    distrho_plugin = p_distrho_plugin;
}

DistrhoPluginInstance *DistrhoPluginServer::get_distrho_plugin() {
    return distrho_plugin;
}

Ref<DistrhoParameter> DistrhoPluginServer::create_parameter(const Dictionary &p_data) {
    Ref<DistrhoParameter> parameter;
    parameter.instantiate();

    parameter->set_hints(p_data.get("hints", DistrhoParameter::HINT_NONE));
    parameter->set_name(p_data.get("name", ""));
    parameter->set_short_name(p_data.get("short_name", ""));
    parameter->set_symbol(p_data.get("symbol", ""));
    parameter->set_unit(p_data.get("unit", ""));
    parameter->set_description(p_data.get("description", ""));

    parameter->set_default_value(p_data.get("default_value", 0));
    parameter->set_min_value(p_data.get("min_value", 0));
    parameter->set_max_value(p_data.get("max_value", 0));

    Dictionary enumeration_values;
    parameter->set_enumeration_values(p_data.get("enumeration_values", enumeration_values));
    parameter->set_designation(p_data.get("designation", 0));

    parameter->set_midi_cc(p_data.get("midi_cc", 0));
    parameter->set_group_id(p_data.get("midi_cc", DistrhoAudioPort::PORT_GROUP_NONE));

    return parameter;
}

Ref<DistrhoAudioPort> DistrhoPluginServer::create_audio_port(const Dictionary &p_data) {
    Ref<DistrhoAudioPort> audio_port;
    audio_port.instantiate();

    audio_port->set_hints(p_data.get("hints", DistrhoAudioPort::HINT_NONE));
    audio_port->set_name(p_data.get("name", ""));
    audio_port->set_symbol(p_data.get("symbol", ""));
    audio_port->set_group_id(p_data.get("group_id", DistrhoAudioPort::PORT_GROUP_NONE));

    return audio_port;
}

void DistrhoPluginServer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize"), &DistrhoPluginServer::initialize);

    ClassDB::bind_method(D_METHOD("send_midi_event"), &DistrhoPluginServer::send_midi_event);
    ClassDB::bind_method(D_METHOD("note_on", "channel", "note", "velocity"), &DistrhoPluginServer::note_on);
    ClassDB::bind_method(D_METHOD("note_off", "channel", "note", "velocity"), &DistrhoPluginServer::note_off);
    ClassDB::bind_method(D_METHOD("program_change", "channel", "program_number"), &DistrhoPluginServer::program_change);
    ClassDB::bind_method(D_METHOD("control_change", "channel", "controller", "value"),
                         &DistrhoPluginServer::control_change);
    ClassDB::bind_method(D_METHOD("midi_poly_aftertouch", "channel", "note", "pressure"),
                         &DistrhoPluginServer::midi_poly_aftertouch);

    ClassDB::bind_method(D_METHOD("get_parameter_value", "index"), &DistrhoPluginServer::get_parameter_value);
    ClassDB::bind_method(D_METHOD("set_parameter_value", "index", "value"), &DistrhoPluginServer::set_parameter_value);

    ClassDB::bind_method(D_METHOD("create_parameter", "data"), &DistrhoPluginServer::create_parameter);
    ClassDB::bind_method(D_METHOD("create_audio_port", "data"), &DistrhoPluginServer::create_audio_port);

    ClassDB::bind_method(D_METHOD("get_config"), &DistrhoPluginServer::get_config);
    ClassDB::bind_method(D_METHOD("set_distrho_plugin", "distrho_plugin"), &DistrhoPluginServer::set_distrho_plugin);

    ClassDB::bind_method(D_METHOD("get_version"), &DistrhoPluginServer::get_version);
    ClassDB::bind_method(D_METHOD("get_build"), &DistrhoPluginServer::get_build);

    ADD_SIGNAL(
        MethodInfo("parameter_changed", PropertyInfo(Variant::INT, "index"), PropertyInfo(Variant::FLOAT, "value")));

    ADD_SIGNAL(MethodInfo("midi_event",
                          PropertyInfo(Variant::OBJECT, "event", PROPERTY_HINT_RESOURCE_TYPE, "DistrhoMidiEvent")));

    ADD_SIGNAL(MethodInfo("midi_note_on", PropertyInfo(Variant::INT, "channel"), PropertyInfo(Variant::INT, "note"),
                          PropertyInfo(Variant::INT, "velocity"), PropertyInfo(Variant::INT, "frame")));

    ADD_SIGNAL(MethodInfo("midi_note_off", PropertyInfo(Variant::INT, "channel"), PropertyInfo(Variant::INT, "note"),
                          PropertyInfo(Variant::INT, "velocity"), PropertyInfo(Variant::INT, "frame")));

    ADD_SIGNAL(MethodInfo("midi_cc", PropertyInfo(Variant::INT, "channel"), PropertyInfo(Variant::INT, "controller"),
                          PropertyInfo(Variant::INT, "value"), PropertyInfo(Variant::INT, "frame")));

    ADD_SIGNAL(MethodInfo("midi_program_change", PropertyInfo(Variant::INT, "channel"),
                          PropertyInfo(Variant::INT, "program"), PropertyInfo(Variant::INT, "frame")));

    ADD_SIGNAL(MethodInfo("midi_channel_pressure", PropertyInfo(Variant::INT, "channel"),
                          PropertyInfo(Variant::INT, "pressure"), PropertyInfo(Variant::INT, "frame")));

    ADD_SIGNAL(MethodInfo("midi_pitch_bend", PropertyInfo(Variant::INT, "channel"),
                          PropertyInfo(Variant::INT, "bend_value"), PropertyInfo(Variant::INT, "frame")));

    ADD_SIGNAL(MethodInfo("midi_poly_aftertouch", PropertyInfo(Variant::INT, "channel"),
                          PropertyInfo(Variant::INT, "note"), PropertyInfo(Variant::INT, "pressure"),
                          PropertyInfo(Variant::INT, "frame")));
}
