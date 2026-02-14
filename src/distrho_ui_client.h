#ifndef DISTRHO_UI_CLIENT_H
#define DISTRHO_UI_CLIENT_H

#include "distrho_shared_memory_rpc.h"
#include "distrho_shared_memory_region.h"
#include <functional>
#include <godot_cpp/classes/node.hpp>

namespace godot {

class DistrhoUIClient {

private:
    DistrhoSharedMemoryRPC *rpc_memory;
    DistrhoSharedMemoryRegion *shared_memory_region;

protected:
    template <typename T, typename R>
    capnp::FlatArrayMessageReader rpc_call(bool &result, std::function<void(typename T::Builder &)> build_request = nullptr) const;

public:
    DistrhoUIClient(DistrhoSharedMemoryRPC *p_rpc_memory, DistrhoSharedMemoryRegion *p_shared_memory_region);
    ~DistrhoUIClient();

    void edit_parameter(int p_index, bool p_started);
    void set_parameter_value(int p_index, float p_value);
    void send_note(int p_channel, int p_note, int p_velocity);
    void set_state(String p_key, String p_value);

    static void _bind_methods();
};

} // namespace godot
#endif
