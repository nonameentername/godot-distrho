#ifndef GODOT_DISTRHO_UI_H
#define GODOT_DISTRHO_UI_H

#include "DistrhoPlugin.hpp"
#include "DistrhoPluginUtils.hpp"
#include "DistrhoStandaloneUtils.hpp"
#include "DistrhoUI.hpp"
#include "godot_distrho_ui_client.h"
#include "godot_distrho_ui_server.h"

// #include "libgodot_distrho.h"
#include <thread>

START_NAMESPACE_DISTRHO

class GodotDistrhoUI : public UI {

private:
    bool initialized = false;
    uintptr_t window_id = 0;
    mutable GodotDistrhoUIClient *client;
    mutable GodotDistrhoUIServer *server;

public:
    GodotDistrhoUI();

    ~GodotDistrhoUI();

    void visibilityChanged(const bool visible) override;

    uintptr_t get_window_id();

    // void onDisplay() override;

protected:
    void parameterChanged(const uint32_t index, const float value) override;

    void uiIdle() override;

    //uintptr_t getNativeWindowHandle() const noexcept override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GodotDistrhoUI)
};

END_NAMESPACE_DISTRHO

#endif
