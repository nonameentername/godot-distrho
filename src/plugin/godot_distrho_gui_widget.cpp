#include "godot_distrho_gui_widget.h"
#include "TopLevelWidget.hpp"
#include "Window.hpp"
#include "Widget.hpp"
#include "DistrhoUI.hpp"
#include "Image.hpp"

#include "godot_distrho_images.hpp"


START_NAMESPACE_DGL


GodotDistrhoGuiWidget::GodotDistrhoGuiWidget(Window &p_window) : TopLevelWidget(p_window) {
    window_id = p_window.getNativeWindowHandle();
}

GodotDistrhoGuiWidget::~GodotDistrhoGuiWidget() {
}

void GodotDistrhoGuiWidget::onDisplay() {
    Image godot_distrho(godot_distrho::godot_distrhoData,
            godot_distrho::godot_distrhoWidth,
            godot_distrho::godot_distrhoHeight,
            ImageFormat::kImageFormatRGBA);

    godot_distrho.draw();
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
