#ifndef GODOT_DISTRHO_PLUGIN_CLIENT_H
#define GODOT_DISTRHO_PLUGIN_CLIENT_H

#include "DistrhoDetails.hpp"
#include "DistrhoPlugin.hpp"
#include "distrho_common.h"
#include "distrho_shared_memory.h"
#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory_rpc.h"
#include "distrho_shared_memory_region.h"
#include "godot_distrho_schema.capnp.h"

#if defined(_WIN32)
#include <winsock2.h>
#endif

#include <boost/process.hpp>
#include <boost/process/v1/group.hpp>
#include <boost/process/v1/child.hpp>

START_NAMESPACE_DISTRHO

class GodotDistrhoPluginClient {
private:
    std::atomic<bool> is_shutting_down{false};

    mutable std::string label;
    mutable std::string description;
    mutable std::string maker;
    mutable std::string homepage;
    mutable std::string license;
    mutable int version;
    mutable int unique_id;

    boost::process::v1::group windows_group;
    boost::process::v1::child *plugin;
    mutable godot::DistrhoSharedMemory shared_memory;
    mutable godot::DistrhoSharedMemoryAudio audio_memory;
    mutable godot::DistrhoSharedMemoryRPC rpc_memory;
    mutable godot::DistrhoSharedMemoryRPC godot_rpc_memory;
    mutable godot::DistrhoSharedMemoryRegion shared_memory_region;

protected:
    template <typename T, typename R>
    capnp::FlatArrayMessageReader rpc_call(bool &result, std::function<void(typename T::Builder &)> build_request = nullptr) const;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GodotDistrhoPluginClient)

public:
    GodotDistrhoPluginClient(DistrhoCommon::DISTRHO_MODULE_TYPE p_type);

    ~GodotDistrhoPluginClient();

    void run(const float **inputs, float **outputs, uint32_t numSamples, const MidiEvent *input_midi,
             int input_midi_size, MidiEvent *output_midi, int &output_midi_size);

    float get_parameter_value(int p_index) const;
    void set_parameter_value(int p_index, float p_value);

    void set_state_value(const char* p_key, const char* p_value);

    bool shutdown();

    godot::DistrhoSharedMemoryRPC *get_godot_rpc_memory();
};

END_NAMESPACE_DISTRHO

#endif
