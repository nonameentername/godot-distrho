#include "godot-distrho-plugin.h"
#include "libgodot_distrho.h"

START_NAMESPACE_DISTRHO

GodotDistrhoPlugin::GodotDistrhoPlugin() : Plugin(0, 0, 0) // parameters, programs, states
{
    libgodot;
}

GodotDistrhoPlugin::~GodotDistrhoPlugin()
{
}

const char* GodotDistrhoPlugin::getLabel() const
{
    return "godot-distrho";
}

const char* GodotDistrhoPlugin::getDescription() const
{
    return "Simple loader for neural models using RTNeural inference engine.";
}

const char* GodotDistrhoPlugin::getMaker() const
{
    return DISTRHO_PLUGIN_BRAND;
}

const char* GodotDistrhoPlugin::getHomePage() const
{
    return "https://github.com/nonameentername/godot-distrho";
}

const char* GodotDistrhoPlugin::getLicense() const
{
    return "GPL-3.0-or-later";
}

uint32_t GodotDistrhoPlugin::getVersion() const
{
    return d_version(1, 0, 0);
}

int64_t GodotDistrhoPlugin::getUniqueId() const
{
    return d_cconst('M', 'u', 't', 'e');
}


void GodotDistrhoPlugin::initAudioPort(const bool input, const uint32_t index, AudioPort& port)
{
    // treat meter audio ports as mono
    port.groupId = kPortGroupMono;

    // everything else is as default
    Plugin::initAudioPort(input, index, port);
}

void GodotDistrhoPlugin::initParameter(const uint32_t index, Parameter& parameter)
{
}


float GodotDistrhoPlugin::getParameterValue(const uint32_t index) const
{
    return 0;
}

void GodotDistrhoPlugin::setParameterValue(const uint32_t index, const float value)
{
}

void GodotDistrhoPlugin::activate()
{
}

void GodotDistrhoPlugin::run(const float** inputs, float** outputs, uint32_t numSamples)
{
}

Plugin* createPlugin()
{
    GodotDistrhoPlugin* const plugin = new GodotDistrhoPlugin();
    return plugin;
}

END_NAMESPACE_DISTRHO
