#ifndef GODOT_DISTRHO_UI_CLIENT_H
#define GODOT_DISTRHO_UI_CLIENT_H

#include "DistrhoDetails.hpp"
#include "DistrhoPlugin.hpp"
#include "distrho_common.h"
#include "distrho_shared_memory_rpc.h"
#include "godot_distrho_schema.capnp.h"
#include <boost/process.hpp>

START_NAMESPACE_DISTRHO

class GodotDistrhoUIClient {
private:
    boost::process::child *plugin;
    mutable godot::DistrhoSharedMemoryRPC rpc_memory;
    mutable godot::DistrhoSharedMemoryRPC godot_rpc_memory;

protected:
    template <typename T, typename R>
    capnp::FlatArrayMessageReader rpc_call(std::function<void(typename T::Builder &)> build_request = nullptr) const;

public:
    GodotDistrhoUIClient(DistrhoCommon::DISTRHO_MODULE_TYPE p_type);

    ~GodotDistrhoUIClient();

    void run();

    std::string get_some_text();

    bool shutdown();

    godot::DistrhoSharedMemoryRPC *get_godot_rpc_memory();
};

END_NAMESPACE_DISTRHO

#endif
