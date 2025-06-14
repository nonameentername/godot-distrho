#ifndef GODOT_DISTRHO_PLUGIN_H
#define GODOT_DISTRHO_PLUGIN_H

#include "DistrhoPlugin.hpp"
#include "godot_distrho_shared_memory.h"
//#include "libgodot_distrho.h"

#include <thread>

START_NAMESPACE_DISTRHO

class GodotDistrhoPlugin : public Plugin
{
private:
    godot::GodotDistrhoSharedMemory godot_distrho_shared_memory;
    //godot::GodotInstance *instance = NULL;
    std::thread godot_thread;

public:
    GodotDistrhoPlugin();

    ~GodotDistrhoPlugin();

protected:
    const char* getLabel() const override;

    const char* getDescription() const override;

    const char* getMaker() const override;

    const char* getHomePage() const override;

    const char* getLicense() const override;

    uint32_t getVersion() const override;

	int64_t getUniqueId() const override;

    void initAudioPort(const bool input, const uint32_t index, AudioPort& port) override;

    void initParameter(const uint32_t index, Parameter& parameter) override;

    float getParameterValue(const uint32_t index) const override;

    void setParameterValue(const uint32_t index, const float value) override;

    void activate() override;

    void run(const float** inputs, float** outputs, uint32_t numSamples) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GodotDistrhoPlugin)
};

END_NAMESPACE_DISTRHO

#endif
