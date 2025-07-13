#include "godot_distrho_gui_widget.h"
#include "Window.hpp"

START_NAMESPACE_DGL

GodotDistrhoGuiWidget::GodotDistrhoGuiWidget(Window &p_window) : TopLevelWidget(p_window) {
}

GodotDistrhoGuiWidget::~GodotDistrhoGuiWidget() {
}

void GodotDistrhoGuiWidget::onDisplay() {
}

END_NAMESPACE_DGL
