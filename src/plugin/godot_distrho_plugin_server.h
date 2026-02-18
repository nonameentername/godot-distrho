#ifndef GODOT_DISTRHO_PLUGIN_SERVER_H
#define GODOT_DISTRHO_PLUGIN_SERVER_H

#include "DistrhoDetails.hpp"
#include "DistrhoPlugin.hpp"
#include "distrho_common.h"
#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory_rpc.h"
#include "godot_distrho_schema.capnp.h"

#if defined(_WIN32)
#include <winsock2.h>
#endif

#include <boost/process.hpp>
#include <boost/process/v1/child.hpp>
#include <thread>

START_NAMESPACE_DISTRHO

class GodotDistrhoPluginServer {
private:
    std::atomic<bool> is_shutting_down{false};

    bool exit_thread;
    std::thread rpc_thread;

    boost::process::v1::child *plugin;
    godot::DistrhoSharedMemoryRPC *godot_rpc_memory;
    Plugin *godot_distrho_plugin;

protected:
    template <typename T, typename R>
    void handle_rpc_call(std::function<void(typename T::Reader &, typename R::Builder &)> handle_request);

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GodotDistrhoPluginServer)

public:
    GodotDistrhoPluginServer(Plugin *p_godot_distrho_plugin, godot::DistrhoSharedMemoryRPC *p_rpc_memory);

    ~GodotDistrhoPluginServer();

    void rpc_thread_func();

    bool shutdown();
};

END_NAMESPACE_DISTRHO

#endif
