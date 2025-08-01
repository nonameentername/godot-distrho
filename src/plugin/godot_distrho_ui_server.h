#ifndef GODOT_DISTRHO_UI_SERVER_H
#define GODOT_DISTRHO_UI_SERVER_H

#include "DistrhoDetails.hpp"
#include "DistrhoPlugin.hpp"
#include "DistrhoUI.hpp"
#include "distrho_common.h"
#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory_rpc.h"
#include "godot_distrho_schema.capnp.h"
#include <boost/process.hpp>
#include <thread>

START_NAMESPACE_DISTRHO

class GodotDistrhoUIServer {
private:
    bool exit_thread;
    std::thread rpc_thread;

    boost::process::child *plugin;
    godot::DistrhoSharedMemoryRPC *godot_rpc_memory;
    UI *godot_distrho_ui;

protected:
    template <typename T, typename R>
    void handle_rpc_call(std::function<void(typename T::Reader &, typename R::Builder &)> handle_request);

public:
    GodotDistrhoUIServer(UI *p_godot_distrho_ui, godot::DistrhoSharedMemoryRPC *p_godot_rpc_memory);

    ~GodotDistrhoUIServer();

    void rpc_thread_func();

    void shutdown();
};

END_NAMESPACE_DISTRHO

#endif
