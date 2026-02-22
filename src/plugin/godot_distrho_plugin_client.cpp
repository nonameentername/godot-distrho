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
            plugin->wait();
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

float GodotDistrhoPluginClient::get_parameter_value(int p_index) const {
    return shared_memory_region.read_parameter_value(p_index);
}

void GodotDistrhoPluginClient::set_parameter_value(int p_index, float p_value) {
    shared_memory_region.write_parameter_value(p_index, p_value);
}

void GodotDistrhoPluginClient::set_state_value(const char* p_key, const char* p_value) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<SetStateValueRequest, SetStateValueResponse>(result, [p_key, p_value](auto &req) {
            req.setKey(p_key);
            req.setValue(p_value);
        });
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
