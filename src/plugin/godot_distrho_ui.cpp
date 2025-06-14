#include "godot_distrho_ui.h"
#include "godot_distrho_plugin.h"
#include "godot_distrho_utils.h"
//#include "godot_cpp/classes/display_server.hpp"
//#include "libgodot_distrho.h"

//#include "godot_cpp/classes/display_server_embedded.hpp"
//#include "godot_cpp/classes/rendering_native_surface_x11.hpp"

#include <string>
//#include <X11/Xlib.h>


START_NAMESPACE_DISTRHO


void run_godot(uintptr_t window_id) {
    //godot::GodotInstance *instance = NULL;



    //while (!instance->iteration()) {}

    //libgodot.destroy_godot_instance(instance);
}

GodotDistrhoUI::GodotDistrhoUI() : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT)
{
}


GodotDistrhoUI::~GodotDistrhoUI()
{
    godot_thread.join();
}

void GodotDistrhoUI::parameterChanged(const uint32_t index, const float value)
{
}

/*
void GodotDistrhoUI::stateChanged(const char* const key, const char* const value)
{
}

void GodotDistrhoUI::onResize(const ResizeEvent& event)
{
}
*/

void GodotDistrhoUI::onDisplay()
{
    const uint width = getWidth();
    const uint height = getHeight();


    /*
    if (instance != NULL) {
        if(!instance->is_started()) {
            //printf("string instance\n");
            //instance->start();
        } else {
            instance->iteration();
        }
    }
    */

}

void GodotDistrhoUI::requestStateFile(const char* const stateKey, const String& lastDir, const char* const title)
{
}

void GodotDistrhoUI::uiIdle()
{
    //repaint();

    /*
    if (instance != NULL) {
        if(!instance->is_started()) {
            //printf("string instance\n");
            //instance->start();
        } else {
            instance->iteration();
        }
    }
    */

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
