#ifndef GODOT_DISTRHO_GUI_WIDGET_H
#define GODOT_DISTRHO_GUI_WIDGET_H

#include "TopLevelWidget.hpp"
#include "Widget.hpp"

START_NAMESPACE_DGL

class GodotDistrhoGuiWidget : public TopLevelWidget {
private:
	uintptr_t window_id;

protected:
    void onDisplay() override;

    bool onKeyboard(const Widget::KeyboardEvent&) override;
    bool onCharacterInput(const Widget::CharacterInputEvent&) override;
    bool onMouse(const Widget::MouseEvent&) override;
    bool onMotion(const Widget::MotionEvent&) override;
    bool onScroll(const Widget::ScrollEvent&) override;

public:
    GodotDistrhoGuiWidget(Window &p_window);
    ~GodotDistrhoGuiWidget() override;
};

END_NAMESPACE_DGL

#endif
