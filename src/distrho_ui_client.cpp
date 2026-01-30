#include "distrho_ui_client.h"
#include "distrho_common.h"
#include "godot_distrho_schema.capnp.h"

using namespace godot;

DistrhoUIClient::DistrhoUIClient(DistrhoSharedMemoryRPC *p_rpc_memory) {
    rpc_memory = p_rpc_memory;
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
    bool result;
    capnp::FlatArrayMessageReader reader =
        rpc_call<SetParameterValueRequest, SetParameterValueResponse>(result, [p_index, p_value](auto &request) {
            request.setIndex(p_index);
            request.setValue(p_value);
        });
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

void DistrhoUIClient::DistrhoUIClient::_bind_methods() {
}
