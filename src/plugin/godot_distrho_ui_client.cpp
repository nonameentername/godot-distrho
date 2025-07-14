#include "godot_distrho_ui_client.h"
#include "distrho_common.h"
#include "godot_distrho_schema.capnp.h"
#include "godot_distrho_utils.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <capnp/serialize.h>
#include <kj/string.h>

using namespace boost::interprocess;
using namespace boost::posix_time;

START_NAMESPACE_DISTRHO

GodotDistrhoUIClient::GodotDistrhoUIClient(DistrhoCommon::DISTRHO_MODULE_TYPE p_type) {
    rpc_memory.initialize("DISTRHO_SHARED_MEMORY_RPC");
    godot_rpc_memory.initialize("GODOT_SHARED_MEMORY_RPC");

#if DISTRHO_PLUGIN_ENABLE_SUBPROCESS
    boost::process::environment env = boost::this_process::environment();

    env["DISTRHO_MODULE_TYPE"] = std::to_string(p_type);
    env["DISTRHO_SHARED_MEMORY_RPC"] = rpc_memory.shared_memory_name.c_str();
    env["GODOT_SHARED_MEMORY_RPC"] = godot_rpc_memory.shared_memory_name.c_str();

#if defined(_WIN32)
    plugin = GodotDistrhoUtils::launch_process("godot-plugin.exe", env);
    // new boost::process::child("godot-plugin.exe", env);
#else
    plugin = GodotDistrhoUtils::launch_process("godot-plugin", env);
    // plugin = new boost::process::child("godot-plugin", env);
#endif
#endif

    while (!rpc_memory.buffer->ready) {
        sleep(1);
    }

    std::string some_text = get_some_text();
    printf("%s\n", some_text.c_str());
}

GodotDistrhoUIClient::~GodotDistrhoUIClient() {
    if (plugin != NULL) {
        if (plugin->running()) {
            plugin->terminate();
        }
        delete plugin;
        plugin = NULL;
    }
}

template <typename T, typename R>
capnp::FlatArrayMessageReader GodotDistrhoUIClient::rpc_call(
    std::function<void(typename T::Builder &)> build_request) const {
    return DistrhoCommon::rpc_call<T, R>(rpc_memory, build_request);
}

void GodotDistrhoUIClient::run() {
}

std::string GodotDistrhoUIClient::get_some_text() {
    capnp::FlatArrayMessageReader reader = rpc_call<GetSomeTextRequest, GetSomeTextResponse>();
    GetSomeTextResponse::Reader response = reader.getRoot<GetSomeTextResponse>();
    return response.getText();
}

bool GodotDistrhoUIClient::shutdown() {
    capnp::FlatArrayMessageReader reader = rpc_call<ShutdownRequest, ShutdownResponse>();
    ShutdownResponse::Reader response = reader.getRoot<ShutdownResponse>();
    return response.getResult();
}

godot::DistrhoSharedMemoryRPC *GodotDistrhoUIClient::get_godot_rpc_memory() {
    return &godot_rpc_memory;
}

END_NAMESPACE_DISTRHO
