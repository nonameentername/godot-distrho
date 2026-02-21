#include "godot_distrho_plugin_client.h"
#include "distrho_common.h"
#include "distrho_shared_memory_rpc.h"
#include "godot_distrho_schema.capnp.h"
#include "godot_distrho_utils.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <capnp/serialize.h>
#include <kj/string.h>

using namespace boost::interprocess;
using namespace boost::posix_time;

START_NAMESPACE_DISTRHO

GodotDistrhoPluginClient::GodotDistrhoPluginClient(DistrhoCommon::DISTRHO_MODULE_TYPE p_type) {
    int memory_size = audio_memory.get_memory_size() + rpc_memory.get_memory_size() + godot_rpc_memory.get_memory_size();

    shared_memory.initialize("", memory_size);
    audio_memory.initialize(&shared_memory, DISTRHO_PLUGIN_NUM_INPUTS, DISTRHO_PLUGIN_NUM_OUTPUTS);
    rpc_memory.initialize(&shared_memory, godot::RPC_BUFFER_NAME);
    godot_rpc_memory.initialize(&shared_memory, godot::GODOT_RPC_BUFFER_NAME);
    shared_memory_region.initialize(&shared_memory);

#if DISTRHO_PLUGIN_ENABLE_SUBPROCESS
#if defined(_WIN32)
    boost::process::v1::wenvironment env = boost::this_process::wenvironment();

    env[L"DISTRHO_MODULE_TYPE"] = std::to_wstring(p_type);
    env[L"DISTRHO_SHARED_MEMORY_UUID"] = std::wstring(shared_memory.shared_memory_name.begin(), shared_memory.shared_memory_name.end());

    plugin = GodotDistrhoUtils::launch_process("godot-plugin.exe", env, windows_group);
#else
    boost::process::v1::environment env = boost::this_process::environment();

    env["DISTRHO_MODULE_TYPE"] = std::to_string(p_type);
    env["DISTRHO_SHARED_MEMORY_UUID"] = shared_memory.shared_memory_name.c_str();

    plugin = GodotDistrhoUtils::launch_process("godot-plugin", env);
#endif
#endif

    while (!audio_memory.buffer->ready) {
        usleep(10 * 1000);
    }

    while (!rpc_memory.buffer->ready) {
        usleep(10 * 1000);
    }
}

GodotDistrhoPluginClient::~GodotDistrhoPluginClient() {
    is_shutting_down.store(true);

    if (audio_memory.buffer) {
        audio_memory.buffer->output_condition.notify_all(); 
    }

#if DISTRHO_PLUGIN_ENABLE_SUBPROCESS
    if (plugin != NULL) {
        if (plugin->running()) {
            plugin->terminate();
        }
        delete plugin;
        plugin = NULL;
    }
#endif
}

template <typename T, typename R>
capnp::FlatArrayMessageReader GodotDistrhoPluginClient::rpc_call(
    bool &result, std::function<void(typename T::Builder &)> build_request) const {
    return DistrhoCommon::rpc_call<T, R>(rpc_memory, build_request, result);
}

const char *GodotDistrhoPluginClient::getLabel() const {
    if (label.size() > 0) {
        return label.c_str();
    }

    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetLabelRequest, GetLabelResponse>(result, [](auto &req) { req; });
    if (result) {
        GetLabelResponse::Reader response = reader.getRoot<GetLabelResponse>();
        label = response.getLabel();
    }
    return label.c_str();
}

const char *GodotDistrhoPluginClient::getDescription() const {
    if (description.size() > 0) {
        return description.c_str();
    }

    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetDescriptionRequest, GetDescriptionResponse>(result);
    if (result) {
        GetDescriptionResponse::Reader response = reader.getRoot<GetDescriptionResponse>();
        description = response.getDescription();
    }
    return description.c_str();
}

const char *GodotDistrhoPluginClient::getMaker() const {
    if (maker.size() > 0) {
        return maker.c_str();
    }
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetMakerRequest, GetMakerResponse>(result);
    if (result) {
        GetMakerResponse::Reader response = reader.getRoot<GetMakerResponse>();
        maker = response.getMaker();
    }
    return maker.c_str();
}

const char *GodotDistrhoPluginClient::getHomePage() const {
    if (homepage.size() > 0) {
        return homepage.c_str();
    }
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetHomePageRequest, GetHomePageResponse>(result);
    if (result) {
        GetHomePageResponse::Reader response = reader.getRoot<GetHomePageResponse>();
        homepage = response.getHomePage();
    }
    return homepage.c_str();
}

const char *GodotDistrhoPluginClient::getLicense() const {
    if (license.size() > 0) {
        return license.c_str();
    }
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetLicenseRequest, GetLicenseResponse>(result);
    if (result) {
        GetLicenseResponse::Reader response = reader.getRoot<GetLicenseResponse>();
        license = response.getLicense();
    }
    return license.c_str();
}

uint32_t GodotDistrhoPluginClient::getVersion() const {
    if (version > 0) {
        return version;
    }
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetVersionRequest, GetVersionResponse>(result);
    if (result) {
        GetVersionResponse::Reader response = reader.getRoot<GetVersionResponse>();
        version = d_version(response.getMajor(), response.getMinor(), response.getPatch());
    }
    return version;
}

int64_t GodotDistrhoPluginClient::getUniqueId() const {
    if (unique_id > 0) {
        return unique_id;
    }
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetUniqueIdRequest, GetUniqueIdResponse>(result);
    if (result) {
        GetUniqueIdResponse::Reader response = reader.getRoot<GetUniqueIdResponse>();
        std::string string_id = response.getUniqueId();
        if (string_id.length() < 4) {
            unique_id = d_cconst('n', 'o', 'n', 'e');
        } else {
            unique_id = d_cconst(string_id[0], string_id[1], string_id[2], string_id[4]);
        }
    }
    return unique_id;
}

// void GodotDistrhoPluginClient::initAudioPort(const bool input, const uint32_t index, AudioPort &port) {
// }

void GodotDistrhoPluginClient::initParameter(const uint32_t index, Parameter &parameter) {
    get_parameter(index, parameter);
}

float GodotDistrhoPluginClient::getParameterValue(const uint32_t index) const {
    return get_parameter_value(index);
}

void GodotDistrhoPluginClient::setParameterValue(const uint32_t index, const float value) {
    set_parameter_value(index, value);
}

void GodotDistrhoPluginClient::activate() {
}

void GodotDistrhoPluginClient::initState(uint32_t index, State& state) {
    get_initial_state_value(index, state);
}

void GodotDistrhoPluginClient::run(const float **inputs, float **outputs, uint32_t num_samples,
                                   const MidiEvent *input_midi, int input_midi_size, MidiEvent *output_midi,
                                   int &output_midi_size) {
    //bool reinitialize = false;

    if (audio_memory.buffer->ready) {
        scoped_lock<interprocess_mutex> lock(audio_memory.buffer->mutex);

        audio_memory.buffer->num_samples = num_samples;
        audio_memory.write_input_channel(inputs, num_samples);
        audio_memory.advance_input_write_index(num_samples);
        audio_memory.write_input_midi(input_midi, input_midi_size);

        audio_memory.buffer->input_condition.notify_one();

        ptime timeout = microsec_clock::universal_time() + milliseconds(100);

        bool result = audio_memory.buffer->output_condition.timed_wait(lock, timeout);

        if (is_shutting_down.load()) {
            return;
        }

        if (result) {
            audio_memory.read_output_channel(outputs, num_samples);
            audio_memory.advance_output_read_index(num_samples);
            output_midi_size = audio_memory.read_output_midi(output_midi);
        } else {
            //reinitialize = true;
        }
    } else {
        for (int channel = 0; channel < DISTRHO_PLUGIN_NUM_OUTPUTS; channel++) {
            for (int frame = 0; frame < num_samples; frame++) {
                outputs[channel][frame] = 0;
            }
        }
    }
}

bool GodotDistrhoPluginClient::get_parameter(int p_index, Parameter &parameter) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<GetParameterRequest, GetParameterResponse>(result, [p_index](auto &req) { req.setIndex(p_index); });

    if (result) {
        GetParameterResponse::Reader response = reader.getRoot<GetParameterResponse>();

        if (response.getResult()) {
            parameter.hints = response.getHints();
            parameter.name = response.getName().cStr();
            parameter.symbol = response.getSymbol().cStr();
            parameter.unit = response.getUnit().cStr();
            parameter.description = response.getDescription().cStr();

            parameter.ranges.def = response.getDefaultValue();
            parameter.ranges.min = response.getMinValue();
            parameter.ranges.max = response.getMaxValue();

            int enumeration_count = response.getEnumerationCount();
            parameter.enumValues.count = enumeration_count;
            parameter.enumValues.values = new ParameterEnumerationValue[enumeration_count];

            for (int i = 0; i < enumeration_count; i++) {
                get_parameter_enum(p_index, i, &parameter.enumValues.values[i]);
            }

            parameter.designation = (ParameterDesignation)response.getDesignation();

            parameter.midiCC = response.getMidiCC();
            parameter.groupId = response.getGroupId();
        }

        return response.getResult();
    } else {
        return result;
    }
}

void GodotDistrhoPluginClient::get_parameter_enum(int p_parameter_index, int p_index, ParameterEnumerationValue *parameter_enum) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<GetParameterEnumRequest, GetParameterEnumResponse>(result, [p_parameter_index, p_index](auto &req) {
                req.setParameterIndex(p_parameter_index);
                req.setIndex(p_index);
                });

    if (result) {
        GetParameterEnumResponse::Reader response = reader.getRoot<GetParameterEnumResponse>();
        parameter_enum->label = response.getLabel().cStr();
        parameter_enum->value = response.getValue();
    }
}

int GodotDistrhoPluginClient::get_parameter_count() {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetParameterCountRequest, GetParameterCountResponse>(result);
    if (result) {
        GetParameterCountResponse::Reader response = reader.getRoot<GetParameterCountResponse>();
        return response.getCount();
    } else {
        return 0;
    }
}

float GodotDistrhoPluginClient::get_parameter_value(int p_index) const {
    return shared_memory_region.read_parameter_value(p_index);
}

void GodotDistrhoPluginClient::set_parameter_value(int p_index, float p_value) {
    shared_memory_region.write_parameter_value(p_index, p_value);
}

bool GodotDistrhoPluginClient::get_initial_state_value(int p_index, State &p_state) {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetInitialStateValueRequest, GetInitialStateValueResponse>(result, [p_index](auto &req) {
            req.setIndex(p_index);
        });

    if (result) {
        GetInitialStateValueResponse::Reader response = reader.getRoot<GetInitialStateValueResponse>();
        p_state.key = response.getKey().cStr();
        p_state.defaultValue = response.getValue().cStr();
    }

    return result;
}

void GodotDistrhoPluginClient::set_state_value(const char* p_key, const char* p_value) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<SetStateValueRequest, SetStateValueResponse>(result, [p_key, p_value](auto &req) {
            req.setKey(p_key);
            req.setValue(p_value);
        });
}

int GodotDistrhoPluginClient::get_program_count() {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetProgramCountRequest, GetProgramCountResponse>(result);
    if (result) {
        GetProgramCountResponse::Reader response = reader.getRoot<GetProgramCountResponse>();
        return response.getCount();
    } else {
        return 0;
    }
}

int GodotDistrhoPluginClient::get_state_count() {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetStateCountRequest, GetStateCountResponse>(result);
    if (result) {
        GetStateCountResponse::Reader response = reader.getRoot<GetStateCountResponse>();
        return response.getCount();
    } else {
        return 0;
    }
}

int GodotDistrhoPluginClient::get_number_of_input_ports() {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetNumberOfInputPortsRequest, GetNumberOfInputPortsResponse>(result);
    if (result) {
        GetNumberOfInputPortsResponse::Reader response = reader.getRoot<GetNumberOfInputPortsResponse>();
        return response.getNumberOfInputPorts();
    } else {
        return 0;
    }
}

int GodotDistrhoPluginClient::get_number_of_output_ports() {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetNumberOfOutputPortsRequest, GetNumberOfOutputPortsResponse>(result);
    if (result) {
        GetNumberOfOutputPortsResponse::Reader response = reader.getRoot<GetNumberOfOutputPortsResponse>();
        return response.getNumberOfOutputPorts();
    } else {
        return 0;
    }
}

bool GodotDistrhoPluginClient::get_input_port(int p_index, AudioPort &port) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<GetInputPortRequest, GetInputPortResponse>(result, [p_index](auto &req) { req.setIndex(p_index); });

    if (result) {
        GetInputPortResponse::Reader response = reader.getRoot<GetInputPortResponse>();
        port.hints = response.getHints();
        port.name = response.getName().cStr();
        port.symbol = response.getSymbol().cStr();
        port.groupId = response.getGroupId();

        return response.getResult();
    } else {
        return result;
    }
}

bool GodotDistrhoPluginClient::get_output_port(int p_index, AudioPort &port) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<GetOutputPortRequest, GetOutputPortResponse>(result, [p_index](auto &req) { req.setIndex(p_index); });

    if (result) {
        GetOutputPortResponse::Reader response = reader.getRoot<GetOutputPortResponse>();
        port.hints = response.getHints();
        port.name = response.getName().cStr();
        port.symbol = response.getSymbol().cStr();
        port.groupId = response.getGroupId();

        return response.getResult();
    } else {
        return result;
    }
}

bool GodotDistrhoPluginClient::shutdown() {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<ShutdownRequest, ShutdownResponse>(result);
    if (result) {
        ShutdownResponse::Reader response = reader.getRoot<ShutdownResponse>();
        return response.getResult();
    } else {
        return result;
    }
}

godot::DistrhoSharedMemoryRPC *GodotDistrhoPluginClient::get_godot_rpc_memory() {
    return &godot_rpc_memory;
}

END_NAMESPACE_DISTRHO
