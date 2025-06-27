#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "godot_distrho_plugin.h"
#include "DistrhoPluginInfo.h"
#include "distrho_schema.capnp.h"
#include <thread>
#include <capnp/serialize.h>
#include <kj/string.h>


START_NAMESPACE_DISTRHO

GodotDistrhoPlugin::GodotDistrhoPlugin() : Plugin(0, 0, 0) // parameters, programs, states
{
    distrho_shared_memory_audio.initialize(DISTRHO_PLUGIN_NUM_INPUTS, DISTRHO_PLUGIN_NUM_OUTPUTS);
    distrho_shared_memory_rpc.initialize();

#if DISTRHO_PLUGIN_ENABLE_SUBPROCESS
    if (plugin == NULL) {
        boost::process::environment env = boost::this_process::environment();
        env["DISTRHO_SHARED_MEMORY_AUDIO"] = distrho_shared_memory_audio.shared_memory_name.c_str();
        env["DISTRHO_SHARED_MEMORY_RPC"] = distrho_shared_memory_rpc.shared_memory_name.c_str();
#if defined(_WIN32)
        plugin = new boost::process::child("godot-plugin.exe", env);
#else
        plugin = new boost::process::child("godot-plugin", env);
#endif
    }
#endif

    while (!distrho_shared_memory_audio.buffer->godot_ready && !distrho_shared_memory_rpc.buffer->godot_ready) {
        sleep(1);
    }

    std::string label = getLabel();
    label = getLabel();
    printf("%s\n", label.c_str());
}

GodotDistrhoPlugin::~GodotDistrhoPlugin()
{
}

const char* GodotDistrhoPlugin::getLabel() const
{
    if (distrho_shared_memory_rpc.buffer->godot_ready) {
        // Acquire the mutex
        boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(distrho_shared_memory_rpc.buffer->mutex);

        capnp::MallocMessageBuilder builder;
        PluginInterface::GetLabelParams::Builder request = builder.initRoot<PluginInterface::GetLabelParams>();

        distrho_shared_memory_rpc.write_request(&builder, PluginInterface::GetLabelParams::_capnpPrivate::typeId);

        // Signal input ready
        distrho_shared_memory_rpc.buffer->input_condition.notify_one();

        boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time() 
            + boost::posix_time::milliseconds(100);

        // Set output flag to wait
        bool result = distrho_shared_memory_rpc.buffer->output_condition.timed_wait(lock, timeout);
        //distrho_shared_memory_rpc.buffer->output_condition.wait(lock);

        if (result) {
            capnp::FlatArrayMessageReader reader = distrho_shared_memory_rpc.read_reponse();
            PluginInterface::GetLabelResults::Reader response = reader.getRoot<PluginInterface::GetLabelResults>();
            return response.getLabel().cStr();
        }
        // The mutex is automatically released when the scoped_lock goes out of scope
    }

    return "godot-distrho";
}

const char* GodotDistrhoPlugin::getDescription() const
{
    return "Godot DISTRHO plugin";
}

const char* GodotDistrhoPlugin::getMaker() const
{
    return "godot-distrho";
}

const char* GodotDistrhoPlugin::getHomePage() const
{
    return "https://github.com/nonameentername/godot-distrho";
}

const char* GodotDistrhoPlugin::getLicense() const
{
    return "MIT";
}

uint32_t GodotDistrhoPlugin::getVersion() const
{
    return d_version(1, 0, 0);
}

int64_t GodotDistrhoPlugin::getUniqueId() const
{
    return d_cconst('M', 'u', 't', 'e');
}


void GodotDistrhoPlugin::initAudioPort(const bool input, const uint32_t index, AudioPort& port)
{
    // treat meter audio ports as mono
    port.groupId = kPortGroupMono;

    // everything else is as default
    Plugin::initAudioPort(input, index, port);
}

void GodotDistrhoPlugin::initParameter(const uint32_t index, Parameter& parameter)
{
}


float GodotDistrhoPlugin::getParameterValue(const uint32_t index) const
{
    return 0;
}

void GodotDistrhoPlugin::setParameterValue(const uint32_t index, const float value)
{
}

void GodotDistrhoPlugin::activate()
{
}

void GodotDistrhoPlugin::run(const float** inputs, float** outputs, uint32_t numSamples)
{
    bool reinitialize = false;

    // Wait for Godot to be ready
    if (distrho_shared_memory_audio.buffer->godot_ready) {
        // Acquire the mutex
        boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(distrho_shared_memory_audio.buffer->mutex);

        // Write input from the correct segment
        distrho_shared_memory_audio.write_input_channel(inputs, godot::BUFFER_FRAME_SIZE);
        distrho_shared_memory_audio.advance_input_write_index(godot::BUFFER_FRAME_SIZE);

        // Signal input ready
        distrho_shared_memory_audio.buffer->input_condition.notify_one();

        //printf("Plugin: Input written, waiting for output...\n");

        boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time() 
            + boost::posix_time::milliseconds(100);

        // Set output flag to wait
        bool result = distrho_shared_memory_audio.buffer->output_condition.timed_wait(lock, timeout);

        if (result) {
            // Read processed output into the correct segment
            distrho_shared_memory_audio.read_output_channel(outputs, godot::BUFFER_FRAME_SIZE);
            distrho_shared_memory_audio.advance_output_read_index(godot::BUFFER_FRAME_SIZE);
        } else {
            reinitialize = true;
        }

        // The mutex is automatically released when the scoped_lock goes out of scope
    } else {
        for (int channel = 0; channel < DISTRHO_PLUGIN_NUM_OUTPUTS; channel++) {
            for (int frame = 0; frame < godot::BUFFER_FRAME_SIZE; frame++) {
                outputs[channel][frame] = 0;
            }
        }
    }

    if (reinitialize) {
#if DISTRHO_PLUGIN_ENABLE_SUBPROCESS
        if(plugin->running()) {
            plugin->terminate();
        }
        delete plugin;
        plugin = NULL;
#endif
        distrho_shared_memory_audio.initialize(DISTRHO_PLUGIN_NUM_INPUTS, DISTRHO_PLUGIN_NUM_OUTPUTS);
        distrho_shared_memory_rpc.initialize();
    }
}

Plugin* createPlugin()
{
    GodotDistrhoPlugin* const plugin = new GodotDistrhoPlugin();
    return plugin;
}

END_NAMESPACE_DISTRHO
