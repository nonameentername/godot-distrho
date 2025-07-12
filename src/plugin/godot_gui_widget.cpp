#include "godot_gui_widget.h"
#include "Window.hpp"

START_NAMESPACE_DGL

GodotGuiWidget::GodotGuiWidget(Window &p_window) : TopLevelWidget(p_window) {
}

GodotGuiWidget::~GodotGuiWidget() {
}

void GodotGuiWidget::onDisplay() {
}

END_NAMESPACE_DGL
