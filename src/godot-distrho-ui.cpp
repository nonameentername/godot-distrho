#include "godot-distrho-ui.h"
#include "godot-distrho-plugin.h"

START_NAMESPACE_DISTRHO

GodotDistrhoUI::GodotDistrhoUI() : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT)
{
}


void GodotDistrhoUI::parameterChanged(const uint32_t index, const float value)
{
}

void GodotDistrhoUI::stateChanged(const char* const key, const char* const value)
{
}

void GodotDistrhoUI::onResize(const ResizeEvent& event)
{
}

void GodotDistrhoUI::onDisplay()
{
    const uint width = getWidth();
    const uint height = getHeight();
}

void GodotDistrhoUI::buttonClicked(SubWidget* const widget, int)
{
}

void GodotDistrhoUI::requestStateFile(const char* const stateKey, const String& lastDir, const char* const title)
{
}

void GodotDistrhoUI::knobDragStarted(SubWidget* const widget)
{
}

void GodotDistrhoUI::knobDragFinished(SubWidget* const widget)
{
}

void GodotDistrhoUI::knobValueChanged(SubWidget* const widget, float value)
{
}

void GodotDistrhoUI::knobDoubleClicked(SubWidget* const widget)
{
}

void GodotDistrhoUI::uiIdle()
{
    repaint();

    /*
    if (GodotDistrhoPlugin* const pluginPtr = (GodotDistrhoPlugin*)getPluginInstancePointer())
    {
        if (pluginPtr->libgodot == nullptr) {
            return;
        }

        //initialized = true;

        //const MutexLocker csm(dspPtr->fMutex);
        libGodot = pluginPtr->libgodot;
    }
    */

}

void GodotDistrhoUI::uiFileBrowserSelected(const char* const filename)
{
}

UI* createUI()
{
    return new GodotDistrhoUI();
}

END_NAMESPACE_DISTRHO
