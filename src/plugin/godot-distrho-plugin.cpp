#include "godot-distrho-plugin.h"
#include "godot-distrho-utils.h"
#include <string>


START_NAMESPACE_DISTRHO

std::atomic<bool> keep_running(true);

void run_godot() {
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
}

GodotDistrhoPlugin::GodotDistrhoPlugin() : Plugin(0, 0, 0) // parameters, programs, states
{
    godot_thread = std::thread(run_godot);
}

GodotDistrhoPlugin::~GodotDistrhoPlugin()
{
	keep_running = false;
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
}

Plugin* createPlugin()
{
    GodotDistrhoPlugin* const plugin = new GodotDistrhoPlugin();
    return plugin;
}

END_NAMESPACE_DISTRHO
