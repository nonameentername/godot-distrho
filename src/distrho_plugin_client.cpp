#include "distrho_plugin_client.h"
#include "distrho_common.h"
#include "godot_distrho_schema.capnp.h"

using namespace godot;

DistrhoPluginClient::DistrhoPluginClient(DistrhoSharedMemoryRPC *p_rpc_memory) {
    rpc_memory = p_rpc_memory;
}

DistrhoPluginClient::~DistrhoPluginClient() {
}

template <typename T, typename R>
capnp::FlatArrayMessageReader DistrhoPluginClient::rpc_call(
    bool &result, std::function<void(typename T::Builder &)> build_request) const {
    return DistrhoCommon::rpc_call<T, R>(*rpc_memory, build_request, result);
}

void DistrhoPluginClient::update_state_value(String p_key, String p_value) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<UpdateStateValueRequest, UpdateStateValueResponse>(result, [p_key, p_value](auto &request) {
            request.setKey(std::string(p_key.ascii()));
            request.setValue(std::string(p_value.ascii()));
        });
}

void DistrhoPluginClient::DistrhoPluginClient::_bind_methods() {
}
