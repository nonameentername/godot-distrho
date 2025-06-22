#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "godot_distrho_plugin.h"
#include "DistrhoPluginInfo.h"
#include <thread>


START_NAMESPACE_DISTRHO

//std::atomic<bool> keep_running(true);

void run_godot() {
    /*
    godot::GodotInstance *instance = NULL;

	//open default display
	Display *display = GodotDistrhoUtils::get_x11_display();

	printf("display = %ld\n", (long)display);

	//get default root window of display
	::Window window_id = GodotDistrhoUtils::get_x11_window(display);

	printf("window_id = %ld\n", (long)window_id);


    if (instance == NULL) {
        std::vector<std::string> args = {
            "godot-distrho",
            "--path", "/home/wmendiza/source/godot-distrho",
            //"--path", "/home/wmendiza/.lv2/godot-distrho.lv2",
            //"--main-pack", "/home/wmendiza/.lv2/godot-distrho.lv2/distrhogodot.pck",
            "--display-driver", "x11",
            "--rendering-method", "gl_compatibility",
            "--rendering-driver", "opengl3",
            //"--audio-driver", "Dummy",
			"--wid", std::to_string(window_id),
            "--", "--distrho-plugin"
        };

        std::vector<char*> argvs;
        for (const auto& arg : args) {
            argvs.push_back((char*)arg.data());
        }
        argvs.push_back(nullptr);

        instance = libgodot.create_godot_instance(argvs.size(), argvs.data());

        if (instance == nullptr) {
            fprintf(stderr, "Error creating Godot instance\n");
        } else {
            instance->start();
        }
    }

    while (keep_running && !instance->iteration()) {}

    libgodot.destroy_godot_instance(instance);
    */
}

GodotDistrhoPlugin::GodotDistrhoPlugin() : Plugin(0, 0, 0) // parameters, programs, states
{
    godot_thread = std::thread(run_godot);

    distrho_audio_shared_memory.initialize(DISTRHO_PLUGIN_NUM_INPUTS, DISTRHO_PLUGIN_NUM_OUTPUTS);
}

GodotDistrhoPlugin::~GodotDistrhoPlugin()
{
	//keep_running = false;
    godot_thread.join();
}

const char* GodotDistrhoPlugin::getLabel() const
{
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

#if DISTRHO_PLUGIN_ENABLE_SUBPROCESS
    if (plugin == NULL) {
        boost::process::environment env = boost::this_process::environment();
        env["DISTRHO_AUDIO_SHARED_MEMORY"] = distrho_audio_shared_memory.shared_memory_name.c_str();
#if defined(_WIN32)
        plugin = new boost::process::child("godot-plugin.exe", env);
#else
        plugin = new boost::process::child("godot-plugin", env);
#endif
    }
#endif

    // Wait for Godot to be ready
    if (distrho_audio_shared_memory.buffer->godot_ready) {
        // Acquire the mutex
        boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(distrho_audio_shared_memory.buffer->mutex);

        // Write input from the correct segment
        distrho_audio_shared_memory.write_input_channel(inputs, godot::BUFFER_FRAME_SIZE);
        distrho_audio_shared_memory.advance_input_write_index(godot::BUFFER_FRAME_SIZE);

        // Signal input ready
        distrho_audio_shared_memory.buffer->input_condition.notify_one();

        //printf("Plugin: Input written, waiting for output...\n");

        boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time() 
            + boost::posix_time::milliseconds(100);

        // Set output flag to wait
        bool result = distrho_audio_shared_memory.buffer->output_condition.timed_wait(lock, timeout);

        if (result) {
            // Read processed output into the correct segment
            distrho_audio_shared_memory.read_output_channel(outputs, godot::BUFFER_FRAME_SIZE);
            distrho_audio_shared_memory.advance_output_read_index(godot::BUFFER_FRAME_SIZE);
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
        distrho_audio_shared_memory.initialize(DISTRHO_PLUGIN_NUM_INPUTS, DISTRHO_PLUGIN_NUM_OUTPUTS);
    }
}

Plugin* createPlugin()
{
    GodotDistrhoPlugin* const plugin = new GodotDistrhoPlugin();
    return plugin;
}

END_NAMESPACE_DISTRHO
