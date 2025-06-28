#ifndef GODOT_DISTRHO_CLIENT_H
#define GODOT_DISTRHO_CLIENT_H

#include <boost/process.hpp>
#include "DistrhoPlugin.hpp"
#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory_rpc.h"


START_NAMESPACE_DISTRHO

class GodotDistrhoClient
{
private:
    boost::process::child *plugin;
    mutable godot::DistrhoSharedMemoryAudio audio_memory;
    mutable godot::DistrhoSharedMemoryRPC rpc_memory;

protected:
    template<typename T, typename R> capnp::FlatArrayMessageReader rpc_call(std::function<void(typename T::Builder&)> build_request = nullptr) const;

public:
    GodotDistrhoClient();

    ~GodotDistrhoClient();

    const char* getLabel() const;

    const char* getDescription() const;

    const char* getMaker() const;

    const char* getHomePage() const;

    const char* getLicense() const;

    uint32_t getVersion() const;

	int64_t getUniqueId() const;

    void initAudioPort(const bool input, const uint32_t index, AudioPort& port);

    void initParameter(const uint32_t index, Parameter& parameter);

    float getParameterValue(const uint32_t index) const;

    void setParameterValue(const uint32_t index, const float value);

    void activate();

    void run(const float** inputs, float** outputs, uint32_t numSamples);

    int get_parameter_count();

    int get_program_count();

    int get_state_count();
};

END_NAMESPACE_DISTRHO

#endif
