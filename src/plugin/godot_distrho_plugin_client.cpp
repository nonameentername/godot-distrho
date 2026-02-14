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
    boost::process::environment env = boost::this_process::environment();

    env["DISTRHO_MODULE_TYPE"] = std::to_string(p_type);
    env["DISTRHO_SHARED_MEMORY_UUID"] = shared_memory.shared_memory_name.c_str();
#if defined(_WIN32)
    plugin = GodotDistrhoUtils::launch_process("godot-plugin.exe", env);
#else
    plugin = GodotDistrhoUtils::launch_process("godot-plugin", env);
#endif
#endif

    while (!audio_memory.buffer->ready) {
        sleep(1);
    }

    while (!rpc_memory.buffer->ready) {
        sleep(1);
    }

    std::string label = getLabel();
    printf("%s\n", label.c_str());

    std::string description = getDescription();
    printf("%s\n", description.c_str());

    std::string maker = getMaker();
    printf("%s\n", maker.c_str());

    std::string homepage = getHomePage();
    printf("%s\n", homepage.c_str());

    std::string license = getLicense();
    printf("%s\n", license.c_str());

    int version = getVersion();
    printf("%d\n", version);

    int unique_id = getUniqueId();
    printf("%d\n", unique_id);
}

GodotDistrhoPluginClient::~GodotDistrhoPluginClient() {
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
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetLabelRequest, GetLabelResponse>(result, [](auto &req) { req; });

    GetLabelResponse::Reader response = reader.getRoot<GetLabelResponse>();
    return response.getLabel().cStr();
}

const char *GodotDistrhoPluginClient::getDescription() const {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetDescriptionRequest, GetDescriptionResponse>(result);
    GetDescriptionResponse::Reader response = reader.getRoot<GetDescriptionResponse>();
    return response.getDescription().cStr();
}

const char *GodotDistrhoPluginClient::getMaker() const {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetMakerRequest, GetMakerResponse>(result);
    GetMakerResponse::Reader response = reader.getRoot<GetMakerResponse>();
    return response.getMaker().cStr();
}

const char *GodotDistrhoPluginClient::getHomePage() const {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetHomePageRequest, GetHomePageResponse>(result);
    GetHomePageResponse::Reader response = reader.getRoot<GetHomePageResponse>();
    return response.getHomePage().cStr();
}

const char *GodotDistrhoPluginClient::getLicense() const {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetLicenseRequest, GetLicenseResponse>(result);
    GetLicenseResponse::Reader response = reader.getRoot<GetLicenseResponse>();
    return response.getLicense().cStr();
}

uint32_t GodotDistrhoPluginClient::getVersion() const {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetVersionRequest, GetVersionResponse>(result);
    GetVersionResponse::Reader response = reader.getRoot<GetVersionResponse>();

    return d_version(response.getMajor(), response.getMinor(), response.getPatch());
}

int64_t GodotDistrhoPluginClient::getUniqueId() const {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetUniqueIdRequest, GetUniqueIdResponse>(result);
    GetUniqueIdResponse::Reader response = reader.getRoot<GetUniqueIdResponse>();
    std::string unique_id = response.getUniqueId();

    if (unique_id.length() < 4) {
        return d_cconst('n', 'o', 'n', 'e');
    } else {
        return d_cconst(unique_id[0], unique_id[1], unique_id[2], unique_id[4]);
    }
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

void GodotDistrhoPluginClient::run(const float **inputs, float **outputs, uint32_t numSamples,
                                   const MidiEvent *input_midi, int input_midi_size, MidiEvent *output_midi,
                                   int &output_midi_size) {
    //bool reinitialize = false;

    if (audio_memory.buffer->ready) {
        scoped_lock<interprocess_mutex> lock(audio_memory.buffer->mutex);

        audio_memory.write_input_channel(inputs, godot::BUFFER_FRAME_SIZE);
        audio_memory.advance_input_write_index(godot::BUFFER_FRAME_SIZE);
        audio_memory.write_input_midi(input_midi, input_midi_size);

        audio_memory.buffer->input_condition.notify_one();

        ptime timeout = microsec_clock::universal_time() + milliseconds(1000);

        bool result = audio_memory.buffer->output_condition.timed_wait(lock, timeout);

        if (result) {
            audio_memory.read_output_channel(outputs, godot::BUFFER_FRAME_SIZE);
            audio_memory.advance_output_read_index(godot::BUFFER_FRAME_SIZE);
            output_midi_size = audio_memory.read_output_midi(output_midi);
        } else {
            //reinitialize = true;
        }
    } else {
        for (int channel = 0; channel < DISTRHO_PLUGIN_NUM_OUTPUTS; channel++) {
            for (int frame = 0; frame < godot::BUFFER_FRAME_SIZE; frame++) {
                outputs[channel][frame] = 0;
            }
        }
    }

    /*
    if (reinitialize) {
#if DISTRHO_PLUGIN_ENABLE_SUBPROCESS
        if (plugin->running()) {
            plugin->terminate();
        }
        delete plugin;
        plugin = NULL;
#endif
        audio_memory.initialize(DISTRHO_PLUGIN_NUM_INPUTS, DISTRHO_PLUGIN_NUM_OUTPUTS);
        rpc_memory.initialize("DISTRHO_SHARED_MEMORY_RPC");
        godot_rpc_memory.initialize("GODOT_SHARED_MEMORY_RPC");
    }
    */
}

bool GodotDistrhoPluginClient::get_parameter(int p_index, Parameter &parameter) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<GetParameterRequest, GetParameterResponse>(result, [p_index](auto &req) { req.setIndex(p_index); });

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
}

void GodotDistrhoPluginClient::get_parameter_enum(int p_parameter_index, int p_index, ParameterEnumerationValue *parameter_enum) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<GetParameterEnumRequest, GetParameterEnumResponse>(result, [p_parameter_index, p_index](auto &req) {
                req.setParameterIndex(p_parameter_index);
                req.setIndex(p_index);
                });

    GetParameterEnumResponse::Reader response = reader.getRoot<GetParameterEnumResponse>();
    parameter_enum->label = response.getLabel().cStr();
    parameter_enum->value = response.getValue();
}

int GodotDistrhoPluginClient::get_parameter_count() {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetParameterCountRequest, GetParameterCountResponse>(result);
    GetParameterCountResponse::Reader response = reader.getRoot<GetParameterCountResponse>();
    return response.getCount();
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

    GetInitialStateValueResponse::Reader response = reader.getRoot<GetInitialStateValueResponse>();
    p_state.key = response.getKey().cStr();
    p_state.defaultValue = response.getValue().cStr();

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
    GetProgramCountResponse::Reader response = reader.getRoot<GetProgramCountResponse>();
    return response.getCount();
}

int GodotDistrhoPluginClient::get_state_count() {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetStateCountRequest, GetStateCountResponse>(result);
    GetStateCountResponse::Reader response = reader.getRoot<GetStateCountResponse>();
    return response.getCount();
}

int GodotDistrhoPluginClient::get_number_of_input_ports() {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetNumberOfInputPortsRequest, GetNumberOfInputPortsResponse>(result);
    GetNumberOfInputPortsResponse::Reader response = reader.getRoot<GetNumberOfInputPortsResponse>();
    return response.getNumberOfInputPorts();
}

int GodotDistrhoPluginClient::get_number_of_output_ports() {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetNumberOfOutputPortsRequest, GetNumberOfOutputPortsResponse>(result);
    GetNumberOfOutputPortsResponse::Reader response = reader.getRoot<GetNumberOfOutputPortsResponse>();
    return response.getNumberOfOutputPorts();
}

bool GodotDistrhoPluginClient::get_input_port(int p_index, AudioPort &port) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<GetInputPortRequest, GetInputPortResponse>(result, [p_index](auto &req) { req.setIndex(p_index); });

    GetInputPortResponse::Reader response = reader.getRoot<GetInputPortResponse>();
    port.hints = response.getHints();
    port.name = response.getName().cStr();
    port.symbol = response.getSymbol().cStr();
    port.groupId = response.getGroupId();

    return response.getResult();
}

bool GodotDistrhoPluginClient::get_output_port(int p_index, AudioPort &port) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<GetOutputPortRequest, GetOutputPortResponse>(result, [p_index](auto &req) { req.setIndex(p_index); });

    GetOutputPortResponse::Reader response = reader.getRoot<GetOutputPortResponse>();
    port.hints = response.getHints();
    port.name = response.getName().cStr();
    port.symbol = response.getSymbol().cStr();
    port.groupId = response.getGroupId();

    return response.getResult();
}

bool GodotDistrhoPluginClient::shutdown() {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<ShutdownRequest, ShutdownResponse>(result);
    ShutdownResponse::Reader response = reader.getRoot<ShutdownResponse>();
    return response.getResult();
}

godot::DistrhoSharedMemoryRPC *GodotDistrhoPluginClient::get_godot_rpc_memory() {
    return &godot_rpc_memory;
}

END_NAMESPACE_DISTRHO
