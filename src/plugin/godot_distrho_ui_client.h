#ifndef GODOT_DISTRHO_UI_CLIENT_H
#define GODOT_DISTRHO_UI_CLIENT_H

#include "DistrhoDetails.hpp"
#include "DistrhoPlugin.hpp"
#include "distrho_common.h"
#include "distrho_shared_memory_rpc.h"
#include "godot_distrho_schema.capnp.h"
#include <boost/process.hpp>
#include <cstdint>

START_NAMESPACE_DISTRHO

class GodotDistrhoUIClient {
private:
    int64_t native_window_id = 0;
    boost::process::child *plugin;
    mutable godot::DistrhoSharedMemory shared_memory;
    mutable godot::DistrhoSharedMemoryRPC rpc_memory;
    mutable godot::DistrhoSharedMemoryRPC godot_rpc_memory;

protected:
    template <typename T, typename R>
    capnp::FlatArrayMessageReader rpc_call(bool &result, std::function<void(typename T::Builder &)> build_request = nullptr) const;

public:
    GodotDistrhoUIClient(DistrhoCommon::DISTRHO_MODULE_TYPE p_type, int64_t parent_window_id);

    ~GodotDistrhoUIClient();

    void run();

    std::string get_some_text();

    bool is_ready();

    int64_t get_native_window_id();

    void parameter_changed(int p_index, float p_value);

    void state_changed(std::string p_index, std::string p_value);

    bool shutdown();

    godot::DistrhoSharedMemoryRPC *get_godot_rpc_memory();
};

END_NAMESPACE_DISTRHO

#endif
