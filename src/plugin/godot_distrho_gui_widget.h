#ifndef GODOT_DISTRHO_GUI_WIDGET_H
#define GODOT_DISTRHO_GUI_WIDGET_H

#include "TopLevelWidget.hpp"

START_NAMESPACE_DGL

class GodotDistrhoGuiWidget : public TopLevelWidget {
private:
protected:
    void onDisplay() override;

public:
    GodotDistrhoGuiWidget(Window &p_window);
    ~GodotDistrhoGuiWidget() override;
};

END_NAMESPACE_DGL

#endif
