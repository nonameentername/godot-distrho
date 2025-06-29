#include <boost/interprocess/sync/scoped_lock.hpp>
#include "distrho_server.h"
#include "distrho_audio_port.h"
#include "distrho_config.h"
#include "distrho_plugin_instance.h"
#include "distrho_schema.capnp.h"
#include "distrho_shared_memory_audio.h"
#include "distrho_circular_buffer.h"
#include "distrho_shared_memory_rpc.h"
#include "godot_cpp/classes/audio_server.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/classes/os.hpp"
#include "distrho_shared_memory_audio.h"
#include "version_generated.gen.h"
#include <capnp/serialize.h>
#include <cstdlib>
#include <godot_cpp/core/mutex_lock.hpp>
#include <godot_cpp/classes/mutex.hpp>
#include "godot_cpp/classes/engine.hpp"
#include <kj/string.h>
#include <unistd.h>


using namespace godot;
using namespace boost::interprocess;


DistrhoServer *DistrhoServer::singleton = NULL;

DistrhoServer::DistrhoServer() {
    initialized = false;
	active = false;
    exit_thread = false;
    distrho_config = memnew(DistrhoConfig);
    distrho_plugin = memnew(DistrhoPluginInstance);
	audio_memory = new DistrhoSharedMemoryAudio();
    const char* audio_shared_memory = std::getenv("DISTRHO_SHARED_MEMORY_AUDIO");
    if (audio_shared_memory == NULL) {
        audio_shared_memory = "";
    }
	audio_memory->initialize(0, 0, audio_shared_memory);
    const char* rpc_shared_memory = std::getenv("DISTRHO_SHARED_MEMORY_RPC");
    if (rpc_shared_memory == NULL) {
        rpc_shared_memory = "";
    }
    rpc_memory = new DistrhoSharedMemoryRPC();
	rpc_memory->initialize(rpc_shared_memory);
    singleton = this;

    //TODO: use the correct number of channels instad of num_channels (16)
	for (int i = 0; i < num_channels; ++i) {
		input_buffer[i] = input_data[i];
		output_buffer[i] = output_data[i];
    }

    input_channels.resize(audio_memory->get_num_input_channels());
    output_channels.resize(audio_memory->get_num_output_channels());

    for (int channel = 0; channel < audio_memory->get_num_input_channels(); channel++) {
        input_channels.write[channel] = new DistrhoCircularBuffer();;
    }

    for (int channel = 0; channel < audio_memory->get_num_output_channels(); channel++) {
        output_channels.write[channel] = new DistrhoCircularBuffer();
    }

    call_deferred("initialize");
}

DistrhoServer::~DistrhoServer() {
	//delete distrho_shared_memory_audio;
    //delete distrho_shared_memory_rpc;

    for (int channel = 0; channel < input_channels.size(); channel++) {
        delete input_channels[channel];
    }

    for (int channel = 0; channel < output_channels.size(); channel++) {
        delete output_channels[channel];
    }

    singleton = NULL;
}

DistrhoServer *DistrhoServer::get_singleton() {
    return singleton;
}

void DistrhoServer::initialize() {
    start();
    initialized = true;
}

void DistrhoServer::audio_thread_func() {
    scoped_lock<interprocess_mutex> shared_memory_lock(audio_memory->buffer->mutex);
    audio_memory->buffer->godot_ready = true;
    shared_memory_lock.unlock();

	active = true;

    while (!exit_thread) {
        scoped_lock<interprocess_mutex> shared_memory_lock(audio_memory->buffer->mutex);

        if (initialized) {
            AudioServer::get_singleton()->process_external(BUFFER_FRAME_SIZE);
        }

        audio_memory->buffer->input_condition.wait(shared_memory_lock);

		audio_memory->read_input_channel(input_buffer, BUFFER_FRAME_SIZE);
		audio_memory->advance_input_read_index(BUFFER_FRAME_SIZE);

        for (int channel = 0; channel < audio_memory->get_num_input_channels(); channel++) {
            input_channels.write[channel]->write_channel(input_buffer[channel], BUFFER_FRAME_SIZE);
        }

        for (int channel = 0; channel < audio_memory->get_num_output_channels(); channel++) {
            output_channels[channel]->read_channel(output_buffer[channel], BUFFER_FRAME_SIZE);
        }

		audio_memory->write_output_channel(output_buffer, BUFFER_FRAME_SIZE);
		audio_memory->advance_output_write_index(BUFFER_FRAME_SIZE);

        audio_memory->buffer->output_condition.notify_one();
    }
	delete audio_memory;
}

template<typename T, typename R> void DistrhoServer::handle_rpc_call(std::function<void(typename T::Reader&, typename R::Builder&)> handle_request) {
    capnp::FlatArrayMessageReader reader = rpc_memory->read_request();
    typename T::Reader request = reader.getRoot<T>();

    capnp::MallocMessageBuilder builder;
    typename R::Builder response = builder.initRoot<R>();

    handle_request(request, response);

    rpc_memory->write_reponse(&builder);
    rpc_memory->buffer->output_condition.notify_one();
}

void DistrhoServer::rpc_thread_func() {
    scoped_lock<interprocess_mutex> shared_memory_lock(rpc_memory->buffer->mutex);
    rpc_memory->buffer->godot_ready = true;
    shared_memory_lock.unlock();

    while (!exit_thread) {
        scoped_lock<interprocess_mutex> shared_memory_lock(rpc_memory->buffer->mutex);
        rpc_memory->buffer->input_condition.wait(shared_memory_lock);

        switch (rpc_memory->buffer->request_id) {
            case GetLabelRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetLabelRequest, GetLabelResponse>(
                [](auto& request, auto&response) {
                    String label = DistrhoServer::get_singleton()->get_distrho_plugin()->_get_label();
                    response.setLabel(std::string(label.ascii()));
                });
                break;
            }

            case GetDescriptionRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetDescriptionRequest, GetDescriptionResponse>(
                [](auto& request, auto&response) {
                    String description = DistrhoServer::get_singleton()->get_distrho_plugin()->_get_description();
                    response.setDescription(std::string(description.ascii()));
                });
                break;
            }

            case GetMakerRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetMakerRequest, GetMakerResponse>(
                [](auto& request, auto&response) {
                    String maker = DistrhoServer::get_singleton()->get_distrho_plugin()->_get_maker();
                    response.setMaker(std::string(maker.ascii()));
                });
                break;
            }

            case GetHomePageRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetHomePageRequest, GetHomePageResponse>(
                [](auto& request, auto&response) {
                    String homepage = DistrhoServer::get_singleton()->get_distrho_plugin()->_get_homepage();
                    response.setHomePage(std::string(homepage.ascii()));
                });
                break;
            }

            case GetLicenseRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetLicenseRequest, GetLicenseResponse>(
                [](auto& request, auto&response) {
                    String license = DistrhoServer::get_singleton()->get_distrho_plugin()->_get_license();
                    response.setLicense(std::string(license.ascii()));
                });
                break;
            }

            case GetVersionRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetVersionRequest, GetVersionResponse>(
                [](auto& request, auto&response) {
                    String value = DistrhoServer::get_singleton()->get_distrho_plugin()->_get_version();
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
                handle_rpc_call<GetUniqueIdRequest, GetUniqueIdResponse>(
                [](auto& request, auto&response) {
                    String unique_id = DistrhoServer::get_singleton()->get_distrho_plugin()->_get_unique_id();
                    if (unique_id.length() >= 4) {
                        response.setUniqueId(std::string(unique_id.ascii()));
                    }
                });
                break;
            }

            case GetNumberOfInputPortsRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetNumberOfInputPortsRequest, GetNumberOfInputPortsResponse>(
                [](auto& request, auto&response) {
                    int number_of_input_ports = DistrhoServer::get_singleton()->get_distrho_plugin()->_get_input_ports().size();
					response.setNumberOfInputPorts(number_of_input_ports);
                });
                break;
            }

            case GetNumberOfOutputPortsRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetNumberOfOutputPortsRequest, GetNumberOfOutputPortsResponse>(
                [](auto& request, auto&response) {
                    int number_of_input_ports = DistrhoServer::get_singleton()->get_distrho_plugin()->_get_output_ports().size();
					response.setNumberOfOutputPorts(number_of_input_ports);
                });
                break;
            }

            case GetInputPortRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetInputPortRequest, GetInputPortResponse>(
                [](auto& request, auto&response) {
                    Vector<Ref<DistrhoAudioPort>> input_ports = DistrhoServer::get_singleton()->get_distrho_plugin()->_get_input_ports();
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
                handle_rpc_call<GetOutputPortRequest, GetOutputPortResponse>(
                [](auto& request, auto&response) {
                    Vector<Ref<DistrhoAudioPort>> output_ports = DistrhoServer::get_singleton()->get_distrho_plugin()->_get_output_ports();
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
        }
    }
	delete rpc_memory;
}

int DistrhoServer::process_sample(AudioFrame *p_buffer, float p_rate, int p_frames) {
    lock_audio();

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

    //semaphore->post();

    return p_frames;
}

void DistrhoServer::set_channel_sample(AudioFrame *p_buffer, float p_rate, int p_frames, int left, int right) {
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

int DistrhoServer::get_channel_sample(AudioFrame *p_buffer, float p_rate, int p_frames, int left, int right) {
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

Error DistrhoServer::start() {
    if(!godot::Engine::get_singleton()->is_editor_hint()) {
        audio_thread.instantiate();
        audio_mutex.instantiate();
        audio_thread->start(callable_mp(this, &DistrhoServer::audio_thread_func), Thread::PRIORITY_HIGH);

        rpc_thread.instantiate();
        rpc_thread->start(callable_mp(this, &DistrhoServer::rpc_thread_func), Thread::PRIORITY_NORMAL);
    }
    return OK;
}

void DistrhoServer::lock_audio() {
    if (audio_thread.is_null() || audio_mutex.is_null()) {
        return;
    }
    audio_mutex->lock();
}

void DistrhoServer::unlock_audio() {
    if (audio_thread.is_null() || audio_mutex.is_null()) {
        return;
    }
    audio_mutex->unlock();
}

void DistrhoServer::finish() {
    if(!godot::Engine::get_singleton()->is_editor_hint()) {
        exit_thread = true;
        audio_thread->wait_to_finish();
        rpc_thread->wait_to_finish();
    }
}

DistrhoConfig *DistrhoServer::get_config() {
    return distrho_config;
}

String DistrhoServer::get_version() {
    return GODOT_DISTRHO_VERSION;
}

String DistrhoServer::get_build() {
    return GODOT_DISTRHO_BUILD;
}

void DistrhoServer::set_distrho_launcher(DistrhoLauncher *p_distrho_launcher) {
	distrho_launcher = p_distrho_launcher;
}

DistrhoLauncher *DistrhoServer::get_distrho_launcher() {
	return distrho_launcher;
}

void DistrhoServer::set_distrho_plugin(DistrhoPluginInstance *p_distrho_plugin) {
    distrho_plugin = p_distrho_plugin;
}

DistrhoPluginInstance *DistrhoServer::get_distrho_plugin() {
    return distrho_plugin;
}

void DistrhoServer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize"), &DistrhoServer::initialize);

    ClassDB::bind_method(D_METHOD("get_config"), &DistrhoServer::get_config);
    ClassDB::bind_method(D_METHOD("set_distrho_plugin", "distrho_plugin"), &DistrhoServer::set_distrho_plugin);

    ClassDB::bind_method(D_METHOD("get_version"), &DistrhoServer::get_version);
    ClassDB::bind_method(D_METHOD("get_build"), &DistrhoServer::get_build);
}
