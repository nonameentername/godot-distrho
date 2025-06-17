#include "godot_distrho_plugin.h"
#include "distrho_shared_memory.h"
#include "godot_distrho_utils.h"
#include "DistrhoPluginInfo.h"
#include <chrono>
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

    godot_distrho_shared_memory.initialize(DISTRHO_PLUGIN_NUM_INPUTS, DISTRHO_PLUGIN_NUM_OUTPUTS);
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
    return "LGPL";
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
    /*
    while(godot_distrho_shared_memory.get_output_flag() != godot::OUTPUT_SYNC::OUTPUT_READY) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    */

    godot_distrho_shared_memory.read_output_channel(outputs, godot::BUFFER_FRAME_SIZE);
    godot_distrho_shared_memory.advance_output_read_index(godot::BUFFER_FRAME_SIZE);

    godot_distrho_shared_memory.write_input_channel(inputs, godot::BUFFER_FRAME_SIZE);
    godot_distrho_shared_memory.advance_input_write_index(godot::BUFFER_FRAME_SIZE);

    godot_distrho_shared_memory.set_input_flag(godot::INPUT_SYNC::INPUT_READY);

    godot_distrho_shared_memory.read_output_channel(outputs, godot::BUFFER_FRAME_SIZE);
    godot_distrho_shared_memory.advance_output_read_index(godot::BUFFER_FRAME_SIZE);

    godot_distrho_shared_memory.write_input_channel(inputs, godot::BUFFER_FRAME_SIZE);
    godot_distrho_shared_memory.advance_input_write_index(godot::BUFFER_FRAME_SIZE);

    godot_distrho_shared_memory.set_input_flag(godot::INPUT_SYNC::INPUT_READY);

    godot_distrho_shared_memory.set_output_flag(godot::OUTPUT_SYNC::OUTPUT_WAIT);
}

Plugin* createPlugin()
{
    GodotDistrhoPlugin* const plugin = new GodotDistrhoPlugin();
    return plugin;
}

END_NAMESPACE_DISTRHO
