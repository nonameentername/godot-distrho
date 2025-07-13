#ifndef GODOT_DISTRHO_PLUGIN_SERVER_H
#define GODOT_DISTRHO_PLUGIN_SERVER_H

#include "DistrhoDetails.hpp"
#include "DistrhoPlugin.hpp"
#include "distrho_common.h"
#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory_rpc.h"
#include "godot_distrho_schema.capnp.h"
#include <boost/process.hpp>

START_NAMESPACE_DISTRHO

class GodotDistrhoPluginServer {
private:
    bool exit_thread;

    boost::process::child *plugin;
    godot::DistrhoSharedMemoryRPC *rpc_memory;

protected:
    template <typename T, typename R>
    void handle_rpc_call(std::function<void(typename T::Reader &, typename R::Builder &)> handle_request);

public:
    GodotDistrhoPluginServer(godot::DistrhoSharedMemoryRPC *p_rpc_memory);

    ~GodotDistrhoPluginServer();

    void rpc_thread_func();

    bool shutdown();
};

END_NAMESPACE_DISTRHO

#endif
