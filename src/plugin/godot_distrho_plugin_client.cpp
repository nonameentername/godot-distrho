#include "godot_distrho_plugin_client.h"
#include "distrho_common.h"
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
    audio_memory.initialize(DISTRHO_PLUGIN_NUM_INPUTS, DISTRHO_PLUGIN_NUM_OUTPUTS);
    rpc_memory.initialize("DISTRHO_SHARED_MEMORY_RPC");
    godot_rpc_memory.initialize("GODOT_SHARED_MEMORY_RPC");

#if DISTRHO_PLUGIN_ENABLE_SUBPROCESS
    boost::process::environment env = boost::this_process::environment();

    env["DISTRHO_MODULE_TYPE"] = std::to_string(p_type);
    env["DISTRHO_SHARED_MEMORY_AUDIO"] = audio_memory.shared_memory_name.c_str();
    env["DISTRHO_SHARED_MEMORY_RPC"] = rpc_memory.shared_memory_name.c_str();
    env["GODOT_SHARED_MEMORY_RPC"] = godot_rpc_memory.shared_memory_name.c_str();

#if defined(_WIN32)
    plugin = GodotDistrhoUtils::launch_process("godot-plugin.exe", env);
    // new boost::process::child("godot-plugin.exe", env);
#else
    plugin = GodotDistrhoUtils::launch_process("godot-plugin", env);
    // plugin = new boost::process::child("godot-plugin", env);
#endif
#endif

    if (p_type == DistrhoCommon::PLUGIN_TYPE) {
        while (!audio_memory.buffer->ready) {
            sleep(1);
        }
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
    if (plugin != NULL) {
        if (plugin->running()) {
            plugin->terminate();
        }
        delete plugin;
        plugin = NULL;
    }
}

template <typename T, typename R>
capnp::FlatArrayMessageReader GodotDistrhoPluginClient::rpc_call(
    std::function<void(typename T::Builder &)> build_request) const {
    return DistrhoCommon::rpc_call<T, R>(rpc_memory, build_request);
}

const char *GodotDistrhoPluginClient::getLabel() const {
    capnp::FlatArrayMessageReader reader = rpc_call<GetLabelRequest, GetLabelResponse>([](auto &req) { req; });

    GetLabelResponse::Reader response = reader.getRoot<GetLabelResponse>();
    return response.getLabel().cStr();
}

const char *GodotDistrhoPluginClient::getDescription() const {
    capnp::FlatArrayMessageReader reader = rpc_call<GetDescriptionRequest, GetDescriptionResponse>();
    GetDescriptionResponse::Reader response = reader.getRoot<GetDescriptionResponse>();
    return response.getDescription().cStr();
}

const char *GodotDistrhoPluginClient::getMaker() const {
    capnp::FlatArrayMessageReader reader = rpc_call<GetMakerRequest, GetMakerResponse>();
    GetMakerResponse::Reader response = reader.getRoot<GetMakerResponse>();
    return response.getMaker().cStr();
}

const char *GodotDistrhoPluginClient::getHomePage() const {
    capnp::FlatArrayMessageReader reader = rpc_call<GetHomePageRequest, GetHomePageResponse>();
    GetHomePageResponse::Reader response = reader.getRoot<GetHomePageResponse>();
    return response.getHomePage().cStr();
}

const char *GodotDistrhoPluginClient::getLicense() const {
    capnp::FlatArrayMessageReader reader = rpc_call<GetLicenseRequest, GetLicenseResponse>();
    GetLicenseResponse::Reader response = reader.getRoot<GetLicenseResponse>();
    return response.getLicense().cStr();
}

uint32_t GodotDistrhoPluginClient::getVersion() const {
    capnp::FlatArrayMessageReader reader = rpc_call<GetVersionRequest, GetVersionResponse>();
    GetVersionResponse::Reader response = reader.getRoot<GetVersionResponse>();

    return d_version(response.getMajor(), response.getMinor(), response.getPatch());
}

int64_t GodotDistrhoPluginClient::getUniqueId() const {
    capnp::FlatArrayMessageReader reader = rpc_call<GetUniqueIdRequest, GetUniqueIdResponse>();
    GetUniqueIdResponse::Reader response = reader.getRoot<GetUniqueIdResponse>();
    std::string unique_id = response.getUniqueId();

    if (unique_id.length() < 4) {
        return d_cconst('n', 'o', 'n', 'e');
    } else {
        return d_cconst(unique_id[0], unique_id[1], unique_id[2], unique_id[4]);
    }
}

void GodotDistrhoPluginClient::initAudioPort(const bool input, const uint32_t index, AudioPort &port) {
}

void GodotDistrhoPluginClient::initParameter(const uint32_t index, Parameter &parameter) {
}

float GodotDistrhoPluginClient::getParameterValue(const uint32_t index) const {
    return 0;
}

void GodotDistrhoPluginClient::setParameterValue(const uint32_t index, const float value) {
}

void GodotDistrhoPluginClient::activate() {
}

void GodotDistrhoPluginClient::run(const float **inputs, float **outputs, uint32_t numSamples,
                                   const MidiEvent *input_midi, int input_midi_size, MidiEvent *output_midi,
                                   int &output_midi_size) {
    bool reinitialize = false;

    if (audio_memory.buffer->ready) {
        scoped_lock<interprocess_mutex> lock(audio_memory.buffer->mutex);

        audio_memory.write_input_channel(inputs, godot::BUFFER_FRAME_SIZE);
        audio_memory.advance_input_write_index(godot::BUFFER_FRAME_SIZE);
        audio_memory.write_input_midi(input_midi, input_midi_size);

        audio_memory.buffer->input_condition.notify_one();

        ptime timeout = microsec_clock::universal_time() + milliseconds(100);

        bool result = audio_memory.buffer->output_condition.timed_wait(lock, timeout);

        if (result) {
            audio_memory.read_output_channel(outputs, godot::BUFFER_FRAME_SIZE);
            audio_memory.advance_output_read_index(godot::BUFFER_FRAME_SIZE);
            output_midi_size = audio_memory.read_output_midi(output_midi);
        } else {
            reinitialize = true;
        }
    } else {
        for (int channel = 0; channel < DISTRHO_PLUGIN_NUM_OUTPUTS; channel++) {
            for (int frame = 0; frame < godot::BUFFER_FRAME_SIZE; frame++) {
                outputs[channel][frame] = 0;
            }
        }
    }

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
}

int GodotDistrhoPluginClient::get_parameter_count() {
    return 0;
}

int GodotDistrhoPluginClient::get_program_count() {
    return 0;
}

int GodotDistrhoPluginClient::get_state_count() {
    return 0;
}

int GodotDistrhoPluginClient::get_number_of_input_ports() {
    capnp::FlatArrayMessageReader reader = rpc_call<GetNumberOfInputPortsRequest, GetNumberOfInputPortsResponse>();
    GetNumberOfInputPortsResponse::Reader response = reader.getRoot<GetNumberOfInputPortsResponse>();
    return response.getNumberOfInputPorts();
}

int GodotDistrhoPluginClient::get_number_of_output_ports() {
    capnp::FlatArrayMessageReader reader = rpc_call<GetNumberOfOutputPortsRequest, GetNumberOfOutputPortsResponse>();
    GetNumberOfOutputPortsResponse::Reader response = reader.getRoot<GetNumberOfOutputPortsResponse>();
    return response.getNumberOfOutputPorts();
}

bool GodotDistrhoPluginClient::get_input_port(int p_index, AudioPort &port) {
    capnp::FlatArrayMessageReader reader =
        rpc_call<GetInputPortRequest, GetInputPortResponse>([p_index](auto &req) { req.setIndex(p_index); });

    GetInputPortResponse::Reader response = reader.getRoot<GetInputPortResponse>();
    port.hints = response.getHints();
    port.name = response.getName().cStr();
    port.symbol = response.getSymbol().cStr();
    port.groupId = response.getGroupId();

    return response.getResult();
}

bool GodotDistrhoPluginClient::get_output_port(int p_index, AudioPort &port) {
    capnp::FlatArrayMessageReader reader =
        rpc_call<GetOutputPortRequest, GetOutputPortResponse>([p_index](auto &req) { req.setIndex(p_index); });

    GetOutputPortResponse::Reader response = reader.getRoot<GetOutputPortResponse>();
    port.hints = response.getHints();
    port.name = response.getName().cStr();
    port.symbol = response.getSymbol().cStr();
    port.groupId = response.getGroupId();

    return response.getResult();
}

bool GodotDistrhoPluginClient::shutdown() {
    capnp::FlatArrayMessageReader reader = rpc_call<ShutdownRequest, ShutdownResponse>();
    ShutdownResponse::Reader response = reader.getRoot<ShutdownResponse>();
    return response.getResult();
}

godot::DistrhoSharedMemoryRPC *GodotDistrhoPluginClient::get_godot_rpc_memory() {
    return &godot_rpc_memory;
}

END_NAMESPACE_DISTRHO
