#include "godot-distrho-ui.h"
#include "godot-distrho-plugin.h"
#include "godot-distrho-utils.h"
#include "libgodot_distrho.h"

#include "godot_cpp/classes/display_server_embedded.hpp"
#include "godot_cpp/classes/rendering_native_surface_x11.hpp"

#include <string>
//#include <X11/Xlib.h>


START_NAMESPACE_DISTRHO

void run_godot(uintptr_t window_id) {
    godot::GodotInstance *instance = NULL;



    //while (!instance->iteration()) {}

    //libgodot.destroy_godot_instance(instance);
}

GodotDistrhoUI::GodotDistrhoUI() : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT)
{
    uintptr_t window_id = getWindow().getNativeWindowHandle();
    printf("window_id = %ld\n", window_id);

    if (instance == NULL) {
        //open default display
        Display *display = GodotDistrhoUtils::get_x11_display();
        //get default root window of display
        //Window window = DefaultRootWindow(display);

        printf("display = %ld\n", (long)display);

        //uintptr_t window = getWindow().getNativeWindowHandle();
        //printf("window = %ld\n", (long)window);

        /*
        if (window == NULL) {
            printf("null root\n");
        } else {
            printf("good root\n");
        }
        */

        std::string program;
        //std::vector<std::string> args = { "program", "--editor", "--rendering-method", "gl_compatibility", "--rendering-driver", "opengl3", "--display-driver", "embedded" };
        //std::vector<std::string> args = { "program", "--path", "/home/wmendiza/source/godot-csound", "--rendering-method", "gl_compatibility", "--rendering-driver", "opengl3", "--display-driver", "x11" };
        //std::vector<std::string> args = { "program", "--editor", "--rendering-method", "forward_plus", "--rendering-driver", "vulkan", "--display-driver", "x11" };
        std::vector<std::string> args = { "program", "--path", "/home/wmendiza/source/godot-csound", "--rendering-method", "forward_plus", "--rendering-driver", "vulkan", "--display-driver", "x11" };

        std::vector<char*> argvs;
        for (const auto& arg : args) {
            argvs.push_back((char*)arg.data());
        }
        argvs.push_back(nullptr);

        instance = libgodot.create_godot_instance(argvs.size(), argvs.data());

        if (instance == nullptr) {
            fprintf(stderr, "Error creating Godot instance\n");
        } else {
            if(instance != NULL && !instance->is_started()) {
                /*
                printf("creating x11 surface\n");

                godot::Ref<godot::RenderingNativeSurfaceX11> x11_surface = godot::RenderingNativeSurfaceX11::create((const void*)window_id, display);

                if (x11_surface == NULL) {
                    printf("null x11_surface\n");
                } else {
                    printf("good x11_surface\n");
                }

                godot::DisplayServerEmbedded::get_singleton()->set_native_surface(x11_surface);
                */
            }

            instance->start();
        }
    } else {
        //instance->iteration();
    }

    //godot_thread = std::thread(run_godot, window_id);
}


GodotDistrhoUI::~GodotDistrhoUI()
{
    godot_thread.join();
}

void GodotDistrhoUI::parameterChanged(const uint32_t index, const float value)
{
}

void GodotDistrhoUI::stateChanged(const char* const key, const char* const value)
{
}

void GodotDistrhoUI::onResize(const ResizeEvent& event)
{
}

void GodotDistrhoUI::onDisplay()
{
    const uint width = getWidth();
    const uint height = getHeight();




    if (instance != NULL) {
        if(!instance->is_started()) {
            //printf("string instance\n");
            //instance->start();
        } else {
            //instance->iteration();
        }
    }
}

}

void GodotDistrhoUI::requestStateFile(const char* const stateKey, const String& lastDir, const char* const title)
{
}

void GodotDistrhoUI::uiIdle()
{
    repaint();

    if (instance != NULL) {
        if(!instance->is_started()) {
            //printf("string instance\n");
            //instance->start();
        } else {
            instance->iteration();
        }
    }

    /*
    if (GodotDistrhoPlugin* const pluginPtr = (GodotDistrhoPlugin*)getPluginInstancePointer())
    {
        if (pluginPtr->libgodot == nullptr) {
            return;
        }

        //initialized = true;

        //const MutexLocker csm(dspPtr->fMutex);
        libGodot = pluginPtr->libgodot;
    }
    */

}

void GodotDistrhoUI::uiFileBrowserSelected(const char* const filename)
{
}

void GodotDistrhoUI::create_godot_instance() {
}

UI* createUI()
{
    GodotDistrhoUI *godot_distrho_ui = new GodotDistrhoUI();

    return godot_distrho_ui;
}

END_NAMESPACE_DISTRHO
