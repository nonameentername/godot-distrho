#include "godot_distrho_ui.h"
#include "godot_distrho_plugin.h"
#include "godot_distrho_utils.h"

#include <string>


START_NAMESPACE_DISTRHO


GodotDistrhoUI::GodotDistrhoUI() : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT)
{
}


GodotDistrhoUI::~GodotDistrhoUI()
{
}

void GodotDistrhoUI::parameterChanged(const uint32_t index, const float value)
{
}

void GodotDistrhoUI::uiIdle()
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
