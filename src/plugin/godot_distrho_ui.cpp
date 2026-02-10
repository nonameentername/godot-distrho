#include "godot_distrho_gui_widget.h"
#include "godot_distrho_ui.h"
#include "godot_distrho_plugin.h"
#include "godot_distrho_utils.h"

//#include "Window.hpp"
#include <string>

START_NAMESPACE_DISTRHO

GodotDistrhoUI::GodotDistrhoUI() : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT) {
    client = NULL;
    server = NULL;

    window_id = 0;
    fprintf(stderr, "parentWindowHandle = %ld\n", window_id);

    if (isVisible()) {
        visibilityChanged(true);
    }
}

GodotDistrhoUI::~GodotDistrhoUI() {
    if (server != NULL) {
        delete server;
        server = NULL;
    }

    if (client != NULL) {
        client->shutdown();
        delete client;
        client = NULL;
    }
}

void GodotDistrhoUI::parameterChanged(const uint32_t index, const float value) {
    if (client != NULL) {
        client->parameter_changed(index, value);
    }
}

void GodotDistrhoUI::uiIdle() {
	if (client != NULL && godot_window_id == 0) {
		if (client->is_ready()) {
			godot_window_id = client->get_native_window_id();
    		fprintf(stderr, "GodotWindowHandle = %ld\n", godot_window_id);
		}
	}
}

void GodotDistrhoUI::stateChanged(const char* key, const char* value) {
    if (client != NULL) {
        client->state_changed(key, value);
    }
}

void GodotDistrhoUI::uiFocus(bool, DGL_NAMESPACE::CrossingMode) {
}

uintptr_t GodotDistrhoUI::getNativeWindowHandle() const noexcept {
    return window_id;
}

void GodotDistrhoUI::visibilityChanged(const bool p_visible) {
    printf("visibility changed\n");

    if (p_visible) {
        if (client == NULL) {
            client = new GodotDistrhoUIClient(DistrhoCommon::UI_TYPE, window_id);
            //window_id = client->get_native_window_id();
            server = new GodotDistrhoUIServer(this, client->get_godot_rpc_memory());
        }
    } else {
        if (server != NULL) {
            delete server;
            server = NULL;
        }

        if (client != NULL) {
            client->shutdown();
            delete client;
            client = NULL;
        }
    }
}

uintptr_t GodotDistrhoUI::get_window_id() {
    //return window_id;
    return 0;
}

/*
void GodotDistrhoUI::onDisplay() {
}
*/

UI *createUI() {
    GodotDistrhoUI *godot_distrho_ui = new GodotDistrhoUI();
    return godot_distrho_ui;
}

END_NAMESPACE_DISTRHO
