#ifndef GODOT_DISTRHO_CLIENT_H
#define GODOT_DISTRHO_CLIENT_H

#include "DistrhoDetails.hpp"
#include "DistrhoPlugin.hpp"
#include "distrho_common.h"
#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory_rpc.h"
#include "godot_distrho_schema.capnp.h"
#include <boost/process.hpp>

START_NAMESPACE_DISTRHO

class GodotDistrhoClient {
private:
    boost::process::child *plugin;
    mutable godot::DistrhoSharedMemoryAudio audio_memory;
    mutable godot::DistrhoSharedMemoryRPC rpc_memory;
    mutable godot::DistrhoSharedMemoryRPC godot_rpc_memory;

protected:
    template <typename T, typename R>
    capnp::FlatArrayMessageReader rpc_call(std::function<void(typename T::Builder &)> build_request = nullptr) const;

public:
    GodotDistrhoClient(DistrhoCommon::DISTRHO_MODULE_TYPE p_type);

    ~GodotDistrhoClient();

    const char *getLabel() const;

    const char *getDescription() const;

    const char *getMaker() const;

    const char *getHomePage() const;

    const char *getLicense() const;

    uint32_t getVersion() const;

    int64_t getUniqueId() const;

    void initAudioPort(const bool input, const uint32_t index, AudioPort &port);

    void initParameter(const uint32_t index, Parameter &parameter);

    float getParameterValue(const uint32_t index) const;

    void setParameterValue(const uint32_t index, const float value);

    void activate();

    void run(const float **inputs, float **outputs, uint32_t numSamples, const MidiEvent *input_midi,
             int input_midi_size, MidiEvent *output_midi, int &output_midi_size);

    int get_parameter_count();

    int get_program_count();

    int get_state_count();

    int get_number_of_input_ports();
    int get_number_of_output_ports();

    bool get_input_port(int p_index, AudioPort &port);
    bool get_output_port(int p_index, AudioPort &port);

    bool shutdown();

    godot::DistrhoSharedMemoryRPC *get_godot_rpc_memory();
};

END_NAMESPACE_DISTRHO

#endif
