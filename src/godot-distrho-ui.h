#ifndef GODOT_DISTRHO_UI_H
#define GODOT_DISTRHO_UI_H

#include "DistrhoUI.hpp"
#include "DistrhoPluginCommon.hpp"
#include "DistrhoPluginUtils.hpp"
#include "DistrhoStandaloneUtils.hpp"

#include "libgodot_distrho.h"

START_NAMESPACE_DISTRHO

class GodotDistrhoUI : public UI,
                        public ButtonEventHandler::Callback,
                        public KnobEventHandler::Callback
{

private:

public:
    GodotDistrhoUI();

protected:
    void parameterChanged(const uint32_t index, const float value) override;

    void stateChanged(const char* const key, const char* const value) override;

    void onResize(const ResizeEvent& event) override;

    void onDisplay() override;

    void buttonClicked(SubWidget* const widget, int) override;

    void requestStateFile(const char* const stateKey, const String& lastDir, const char* const title);

    void knobDragStarted(SubWidget* const widget) override;

    void knobDragFinished(SubWidget* const widget) override;

    void knobValueChanged(SubWidget* const widget, float value) override;

    void knobDoubleClicked(SubWidget* const widget) override;

    void uiIdle() override;

    void uiFileBrowserSelected(const char* const filename) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GodotDistrhoUI)
};

END_NAMESPACE_DISTRHO

#endif
