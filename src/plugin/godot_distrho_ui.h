#ifndef GODOT_DISTRHO_UI_H
#define GODOT_DISTRHO_UI_H

#include "DistrhoUI.hpp"
#include "DistrhoPlugin.hpp"
#include "DistrhoPluginUtils.hpp"
#include "DistrhoStandaloneUtils.hpp"

//#include "libgodot_distrho.h"
#include <thread>

START_NAMESPACE_DISTRHO

class GodotDistrhoUI : public UI
{

private:
    uintptr_t window_id;

public:
    GodotDistrhoUI();

    ~GodotDistrhoUI();

    void create_godot_instance();

protected:
    void parameterChanged(const uint32_t index, const float value) override;

    void uiIdle() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GodotDistrhoUI)
};

END_NAMESPACE_DISTRHO

#endif
