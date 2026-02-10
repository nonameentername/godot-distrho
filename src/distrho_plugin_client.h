#ifndef DISTRHO_PLUGIN_CLIENT_H
#define DISTRHO_PLUGIN_CLIENT_H

#include "distrho_shared_memory_rpc.h"
#include <functional>
#include <godot_cpp/classes/node.hpp>

namespace godot {

class DistrhoPluginClient {

private:
    DistrhoSharedMemoryRPC *rpc_memory;

protected:
    template <typename T, typename R>
    capnp::FlatArrayMessageReader rpc_call(bool &result, std::function<void(typename T::Builder &)> build_request = nullptr) const;

public:
    DistrhoPluginClient(DistrhoSharedMemoryRPC *p_rpc_memory);
    ~DistrhoPluginClient();

    void update_state_value(String p_key, String p_value);

    static void _bind_methods();
};

} // namespace godot
#endif
