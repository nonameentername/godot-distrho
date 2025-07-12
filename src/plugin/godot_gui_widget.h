#ifndef GODOT_GUI_WIDGET_H
#define GODOT_GUI_WIDGET_H

#include "TopLevelWidget.hpp"

START_NAMESPACE_DGL

class GodotGuiWidget : public TopLevelWidget {
private:
protected:
    void onDisplay() override;

public:
    GodotGuiWidget(Window &p_window);
    ~GodotGuiWidget() override;
};

END_NAMESPACE_DGL

#endif
