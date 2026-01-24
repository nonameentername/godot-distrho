#include "godot_distrho_gui_widget.h"
#include "godot_distrho_gui_x11.h"
#include "TopLevelWidget.hpp"
#include "Window.hpp"
#include "Widget.hpp"


START_NAMESPACE_DGL

GodotDistrhoGuiWidget::GodotDistrhoGuiWidget(Window &p_window) : TopLevelWidget(p_window) {
    window_id = p_window.getNativeWindowHandle();
	x11_disable_input(window_id);
}

GodotDistrhoGuiWidget::~GodotDistrhoGuiWidget() {
}

void GodotDistrhoGuiWidget::onDisplay() {
}

bool GodotDistrhoGuiWidget::onKeyboard(const Widget::KeyboardEvent&) {
	return false;
}

bool GodotDistrhoGuiWidget::onCharacterInput(const Widget::CharacterInputEvent&) {
	return false;
}

bool GodotDistrhoGuiWidget::onMouse(const Widget::MouseEvent& mouse_event) {
	return false;
}

bool GodotDistrhoGuiWidget::onMotion(const Widget::MotionEvent&) {
	return false;
}

bool GodotDistrhoGuiWidget::onScroll(const Widget::ScrollEvent&) {
	return false;
}

END_NAMESPACE_DGL
