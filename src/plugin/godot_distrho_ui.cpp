#include "godot_distrho_ui.h"
#include "godot_distrho_plugin.h"
#include "godot_distrho_utils.h"

#include "Window.hpp"
#include <string>

START_NAMESPACE_DISTRHO

GodotDistrhoUI::GodotDistrhoUI() : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT) {
    client = NULL;
    server = NULL;

    // if (isVisible() || isEmbed()) {
    if (isVisible()) {
        visibilityChanged(true);
    }
}

GodotDistrhoUI::~GodotDistrhoUI() {
    if (client != NULL) {
        client->shutdown();
        delete client;
        client = NULL;
    }

    if (server != NULL) {
        delete server;
        server = NULL;
    }
}

void GodotDistrhoUI::parameterChanged(const uint32_t index, const float value) {
}

void GodotDistrhoUI::uiIdle() {
}

void GodotDistrhoUI::create_godot_instance() {
}

void GodotDistrhoUI::visibilityChanged(const bool p_visible) {
    printf("visibility changed\n");

    if (p_visible) {
        if (client == NULL) {
            client = new GodotDistrhoUIClient(DistrhoCommon::UI_TYPE);
            server = new GodotDistrhoUIServer(this, client->get_godot_rpc_memory());
        }
    } else {
        if (client != NULL) {
            client->shutdown();
            delete client;
            client = NULL;
        }

        if (server != NULL) {
            delete server;
            server = NULL;
        }
    }
}

uintptr_t GodotDistrhoUI::get_window_id() {
    return window_id;
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
