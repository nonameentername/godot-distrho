#ifndef GODOT_DISTRHO_PLUGIN_H
#define GODOT_DISTRHO_PLUGIN_H

#include "DistrhoPlugin.hpp"
#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory_rpc.h"
#include "godot_distrho_client.h"
#include <boost/process.hpp>

START_NAMESPACE_DISTRHO

class GodotDistrhoPlugin : public Plugin {
private:
    mutable GodotDistrhoClient *client;

public:
    GodotDistrhoPlugin(GodotDistrhoClient *p_client, uint32_t parameterCount, uint32_t programCount,
                       uint32_t stateCount);
    ~GodotDistrhoPlugin();

protected:
    const char *getLabel() const override;

    const char *getDescription() const override;

    const char *getMaker() const override;

    const char *getHomePage() const override;

    const char *getLicense() const override;

    uint32_t getVersion() const override;

    int64_t getUniqueId() const override;

    void initAudioPort(const bool input, const uint32_t index, AudioPort &port) override;

    void initParameter(const uint32_t index, Parameter &parameter) override;

    float getParameterValue(const uint32_t index) const override;

    void setParameterValue(const uint32_t index, const float value) override;

    void activate() override;

    void run(const float **inputs, float **outputs, uint32_t numSamples, const MidiEvent *midiEvents,
             uint32_t midiEventCount) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GodotDistrhoPlugin)
};

END_NAMESPACE_DISTRHO

#endif
