#include "godot_distrho_ui_server.h"
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

GodotDistrhoUIServer::GodotDistrhoUIServer(UI *p_godot_distrho_ui, godot::DistrhoSharedMemoryRPC *p_rpc_memory) {
    exit_thread = false;
    godot_distrho_ui = p_godot_distrho_ui;
    rpc_memory = p_rpc_memory;

    rpc_thread = std::thread(&GodotDistrhoUIServer::rpc_thread_func, this);
}

GodotDistrhoUIServer::~GodotDistrhoUIServer() {
    exit_thread = true;
    rpc_thread.join();
}

template <typename T, typename R>
void GodotDistrhoUIServer::handle_rpc_call(
    std::function<void(typename T::Reader &, typename R::Builder &)> handle_request) {
    return DistrhoCommon::handle_rpc_call<T, R>(*rpc_memory, handle_request);
}

void GodotDistrhoUIServer::rpc_thread_func() {
    scoped_lock<interprocess_mutex> shared_memory_lock(rpc_memory->buffer->mutex);
    rpc_memory->buffer->ready = true;
    shared_memory_lock.unlock();

    while (!exit_thread) {
        scoped_lock<interprocess_mutex> shared_memory_lock(rpc_memory->buffer->mutex);

        ptime timeout = microsec_clock::universal_time() + milliseconds(5000);
        bool result = rpc_memory->buffer->input_condition.timed_wait(shared_memory_lock, timeout);

        if (!result) {
            continue;
        }

        switch (rpc_memory->buffer->request_id) {
        case EditParameterRequest::_capnpPrivate::typeId: {
            handle_rpc_call<EditParameterRequest, EditParameterResponse>([this](auto &request, auto &response) {
                godot_distrho_ui->editParameter(request.getIndex(), request.getStarted());
            });
            break;
        }

        case SetParameterValueRequest::_capnpPrivate::typeId: {
            handle_rpc_call<SetParameterValueRequest, SetParameterValueResponse>([this](auto &request, auto &response) {
                godot_distrho_ui->setParameterValue(request.getIndex(), request.getValue());
            });
            break;
        }

        case SendNoteRequest::_capnpPrivate::typeId: {
            handle_rpc_call<SendNoteRequest, SendNoteResponse>([this](auto &request, auto &response) {
                godot_distrho_ui->sendNote(request.getChannel(), request.getNote(), request.getVelocity());
            });
            break;
        }
        }
    }
}

void GodotDistrhoUIServer::shutdown() {
    exit_thread = true;
}

END_NAMESPACE_DISTRHO
