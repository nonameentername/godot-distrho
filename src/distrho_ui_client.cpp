#include "distrho_ui_client.h"
#include "distrho_common.h"
#include "godot_distrho_schema.capnp.h"
#include <cstdio>

using namespace godot;

DistrhoUIClient::DistrhoUIClient(DistrhoSharedMemoryRPC *p_rpc_memory, DistrhoSharedMemoryRegion *p_shared_memory_region) {
    rpc_memory = p_rpc_memory;
    shared_memory_region = p_shared_memory_region;
}

DistrhoUIClient::~DistrhoUIClient() {
}

template <typename T, typename R>
capnp::FlatArrayMessageReader DistrhoUIClient::rpc_call(
    bool &result, std::function<void(typename T::Builder &)> build_request) const {
    return DistrhoCommon::rpc_call<T, R>(*rpc_memory, build_request, result);
}

void DistrhoUIClient::edit_parameter(int p_index, bool p_started) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<EditParameterRequest, EditParameterResponse>(result, [p_index, p_started](auto &request) {
            request.setIndex(p_index);
            request.setStarted(p_started);
        });
}

void DistrhoUIClient::set_parameter_value(int p_index, float p_value) {
    shared_memory_region->write_parameter_value(p_index, p_value);
}

void DistrhoUIClient::send_note(int p_channel, int p_note, int p_velocity) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<SendNoteRequest, SendNoteResponse>(result, [p_channel, p_note, p_velocity](auto &request) {
            request.setChannel(p_channel);
            request.setNote(p_note);
            request.setVelocity(p_velocity);
        });
}

void DistrhoUIClient::set_state(String p_key, String p_value) {
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<SetStateRequest, SetStateResponse>(result, [p_key, p_value](auto &request) {
            request.setKey(std::string(p_key.ascii()));
            request.setValue(std::string(p_value.ascii()));
        });
}

void DistrhoUIClient::DistrhoUIClient::_bind_methods() {
}
