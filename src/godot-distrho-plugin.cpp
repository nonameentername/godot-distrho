#include "godot-distrho-plugin.h"
#include "libgodot_distrho.h"

#include "godot_cpp/classes/display_server_embedded.hpp"
#include "godot_cpp/classes/rendering_native_surface_x11.hpp"
#include <X11/Xlib.h>

START_NAMESPACE_DISTRHO

GodotDistrhoPlugin::GodotDistrhoPlugin() : Plugin(0, 0, 0) // parameters, programs, states
{
    if (instance == NULL) {
        //open default display
        Display *display = XOpenDisplay(NULL);
        //get default root window of display
        Window window = DefaultRootWindow(display);

        if (window == NULL) {
            printf("null root\n");
        } else {
            printf("good root\n");
        }

        std::string program;
        std::vector<std::string> args = { "program", "--rendering-method", "gl_compatibility", "--rendering-driver", "opengl3", "--display-driver", "x11" };

        std::vector<char*> argvs;
        for (const auto& arg : args) {
            argvs.push_back((char*)arg.data());
        }
        argvs.push_back(nullptr);

        instance = libgodot.create_godot_instance(argvs.size(), argvs.data());

        if (instance == nullptr) {
            fprintf(stderr, "Error creating Godot instance\n");
        } else {
            if(!instance->is_started()) {
                printf("creating x11 surface\n");

                godot::Ref<godot::RenderingNativeSurfaceX11> x11_surface = godot::RenderingNativeSurfaceX11::create((const void*)window, display);

                if (x11_surface == NULL) {
                    printf("null x11_surface\n");
                } else {
                    printf("good x11_surface\n");
                }

                godot::DisplayServerEmbedded::get_singleton()->set_native_surface(x11_surface);
            }

            instance->start();
        }
    } else {
        instance->iteration();
    }
}

GodotDistrhoPlugin::~GodotDistrhoPlugin()
{
    if (instance) {
        libgodot.destroy_godot_instance(instance);
    }
}

const char* GodotDistrhoPlugin::getLabel() const
{
    return "godot-distrho";
}

const char* GodotDistrhoPlugin::getDescription() const
{
    return "Simple loader for neural models using RTNeural inference engine.";
}

const char* GodotDistrhoPlugin::getMaker() const
{
    return DISTRHO_PLUGIN_BRAND;
}

const char* GodotDistrhoPlugin::getHomePage() const
{
    return "https://github.com/nonameentername/godot-distrho";
}

const char* GodotDistrhoPlugin::getLicense() const
{
    return "GPL-3.0-or-later";
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
    if (instance) {
        //instance->iteration();
    }
}

Plugin* createPlugin()
{
    GodotDistrhoPlugin* const plugin = new GodotDistrhoPlugin();
    return plugin;
}

END_NAMESPACE_DISTRHO
