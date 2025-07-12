#include "godot_distrho_ui.h"
#include "godot_distrho_plugin.h"
#include "godot_distrho_utils.h"

#include "Window.hpp"
#include <string>

START_NAMESPACE_DISTRHO

GodotDistrhoUI::GodotDistrhoUI() : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT) {
    client = NULL;

    // if (isVisible() || isEmbed()) {
    if (isVisible()) {
        visibilityChanged(true);
    }
}

GodotDistrhoUI::~GodotDistrhoUI() {
    if (client != NULL) {
        delete client;
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
            printf("wtf? window_id = %d\n", 0);
            client = new GodotDistrhoClient(DistrhoCommon::UI_TYPE, "0");
        }

        /*
        if (isEmbed()) {
            if (client == NULL) {
                window_id = getParentWindowHandle();
                printf("wtf? window_id = %ld\n", window_id);
                client = new GodotDistrhoClient(DistrhoCommon::UI_TYPE, std::to_string(window_id));
            }
        } else {
        }
        */
    } else {
        if (client != NULL) {
            client->shutdown();
            delete client;
            client = NULL;
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
