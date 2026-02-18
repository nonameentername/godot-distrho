#ifndef GODOT_DISTRHO_UI_SERVER_H
#define GODOT_DISTRHO_UI_SERVER_H

#include "DistrhoDetails.hpp"
#include "DistrhoPlugin.hpp"
#include "DistrhoUI.hpp"
#include "distrho_common.h"
#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory_rpc.h"
#include "distrho_shared_memory_region.h"
#include "godot_distrho_schema.capnp.h"

#if defined(_WIN32)
#include <winsock2.h>
#endif

#include <boost/process.hpp>
#include <boost/process/v1/child.hpp>
#include <thread>

START_NAMESPACE_DISTRHO

class GodotDistrhoUIServer {
private:
    bool exit_thread;
    std::thread process_thread;
    std::thread rpc_thread;

    float parameters[godot::MAX_PARAMS];

    boost::process::v1::child *plugin;
    godot::DistrhoSharedMemoryRPC *godot_rpc_memory;
    godot::DistrhoSharedMemoryRegion *shared_memory_region;
    UI *godot_distrho_ui;

protected:
    template <typename T, typename R>
    void handle_rpc_call(std::function<void(typename T::Reader &, typename R::Builder &)> handle_request);

public:
    GodotDistrhoUIServer(UI *p_godot_distrho_ui, godot::DistrhoSharedMemoryRPC *p_godot_rpc_memory, godot::DistrhoSharedMemoryRegion *p_shared_memory_region);

    ~GodotDistrhoUIServer();

    void process_thread_func();
    void rpc_thread_func();

    void shutdown();
};

END_NAMESPACE_DISTRHO

#endif
