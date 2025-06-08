#ifndef GODOT_DISTRHO_UI_H
#define GODOT_DISTRHO_UI_H

#include "DistrhoUI.hpp"
#include "DistrhoPluginCommon.hpp"
#include "DistrhoPluginUtils.hpp"
#include "DistrhoStandaloneUtils.hpp"

#include "libgodot_distrho.h"
#include <thread>

START_NAMESPACE_DISTRHO

class GodotDistrhoUI : public UI
{

private:
    std::thread godot_thread;
    uintptr_t window_id;

public:
    godot::GodotInstance *instance = NULL;

public:
    GodotDistrhoUI();

    ~GodotDistrhoUI();

    void create_godot_instance();

protected:
    void parameterChanged(const uint32_t index, const float value) override;

    void stateChanged(const char* const key, const char* const value);// override;

    //void onResize(const ResizeEvent& event);// override;

    void onDisplay();// override;

    void requestStateFile(const char* const stateKey, const String& lastDir, const char* const title);

    void uiIdle() override;

    void uiFileBrowserSelected(const char* const filename);// override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GodotDistrhoUI)
};

END_NAMESPACE_DISTRHO

#endif
