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

GodotDistrhoUIClient::GodotDistrhoUIClient(DistrhoCommon::DISTRHO_MODULE_TYPE p_type, int64_t parent_window_id) {
    rpc_memory.initialize("DISTRHO_SHARED_MEMORY_RPC");
    godot_rpc_memory.initialize("GODOT_SHARED_MEMORY_RPC");

#if DISTRHO_PLUGIN_ENABLE_SUBPROCESS
    boost::process::environment env = boost::this_process::environment();

    env["DISTRHO_MODULE_TYPE"] = std::to_string(p_type);
    env["DISTRHO_SHARED_MEMORY_RPC"] = rpc_memory.shared_memory_name.c_str();
    env["GODOT_SHARED_MEMORY_RPC"] = godot_rpc_memory.shared_memory_name.c_str();
    if (parent_window_id > 0) {
        env["GODOT_PARENT_WINDOW_ID"] = std::to_string(parent_window_id);
    }

#if defined(_WIN32)
    plugin = GodotDistrhoUtils::launch_process("godot-plugin.exe", env);
    // new boost::process::child("godot-plugin.exe", env);
#else
    plugin = GodotDistrhoUtils::launch_process("godot-plugin", env);
    // plugin = new boost::process::child("godot-plugin", env);
#endif
#endif

    //while (!rpc_memory.buffer->ready) {
    //    sleep(1);
    //}

    //std::string some_text = get_some_text();
    //printf("%s\n", some_text.c_str());

    //native_window_id = get_native_window_id();
    //printf("native_window_id = %ld\n", native_window_id);
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
    bool &result, std::function<void(typename T::Builder &)> build_request) const {
    return DistrhoCommon::rpc_call<T, R>(rpc_memory, build_request, result);
}

void GodotDistrhoUIClient::run() {
}

std::string GodotDistrhoUIClient::get_some_text() {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetSomeTextRequest, GetSomeTextResponse>(result);
    GetSomeTextResponse::Reader response = reader.getRoot<GetSomeTextResponse>();
    return response.getText();
}

bool GodotDistrhoUIClient::is_ready() {
    return rpc_memory.buffer->ready;
}

int64_t GodotDistrhoUIClient::get_native_window_id() {
    if (native_window_id != 0) {
        return native_window_id;
    }

    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<GetNativeWindowIdRequest, GetNativeWindowIdResponse>(result);
    GetNativeWindowIdResponse::Reader response = reader.getRoot<GetNativeWindowIdResponse>();
    native_window_id = response.getId();

    return native_window_id;
}

void GodotDistrhoUIClient::parameter_changed(int p_index, float p_value) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<ParameterChangedRequest, ParameterChangedResponse>(result, [p_index, p_value](auto &req) {
            req.setIndex(p_index);
            req.setValue(p_value);
        });
}

void GodotDistrhoUIClient::state_changed(std::string p_key, std::string p_value) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<StateChangedRequest, StateChangedResponse>(result, [p_key, p_value](auto &req) {
            req.setKey(p_key);
            req.setValue(p_value);
        });
}

bool GodotDistrhoUIClient::shutdown() {
    bool result;
    capnp::FlatArrayMessageReader reader = rpc_call<ShutdownRequest, ShutdownResponse>(result);
    if (result) {
        ShutdownResponse::Reader response = reader.getRoot<ShutdownResponse>();
        return response.getResult();
    }
    return false;
}

godot::DistrhoSharedMemoryRPC *GodotDistrhoUIClient::get_godot_rpc_memory() {
    return &godot_rpc_memory;
}

END_NAMESPACE_DISTRHO
