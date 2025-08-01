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

GodotDistrhoUIServer::GodotDistrhoUIServer(UI *p_godot_distrho_ui, godot::DistrhoSharedMemoryRPC *p_godot_rpc_memory) {
    exit_thread = false;
    godot_distrho_ui = p_godot_distrho_ui;
    godot_rpc_memory = p_godot_rpc_memory;

    rpc_thread = std::thread(&GodotDistrhoUIServer::rpc_thread_func, this);
}

GodotDistrhoUIServer::~GodotDistrhoUIServer() {
    exit_thread = true;
    rpc_thread.join();
}

template <typename T, typename R>
void GodotDistrhoUIServer::handle_rpc_call(
    std::function<void(typename T::Reader &, typename R::Builder &)> handle_request) {
    return DistrhoCommon::handle_rpc_call<T, R>(*godot_rpc_memory, handle_request);
}

void GodotDistrhoUIServer::rpc_thread_func() {
    {
        scoped_lock<interprocess_mutex> shared_memory_lock(godot_rpc_memory->buffer->mutex);
        godot_rpc_memory->buffer->ready = true;
    }

    bool first_wait = true;

    while (!exit_thread) {
        {
            scoped_lock<interprocess_mutex> shared_memory_lock(godot_rpc_memory->buffer->mutex);

            // if (godot_rpc_memory->buffer->is_alive) {
            if (true) {
                ptime timeout = microsec_clock::universal_time() + milliseconds(first_wait ? 1000 : 100);
                bool result = godot_rpc_memory->buffer->input_condition.timed_wait(
                    shared_memory_lock, timeout, [this]() { return godot_rpc_memory->buffer->request_id != 0; });
                first_wait = false;

                if (result) {
                    printf("Processing request_id: %ld", godot_rpc_memory->buffer->request_id);

                    switch (godot_rpc_memory->buffer->request_id) {
                    case EditParameterRequest::_capnpPrivate::typeId: {
                        handle_rpc_call<EditParameterRequest, EditParameterResponse>(
                            [this](auto &request, auto &response) {
                                godot_distrho_ui->editParameter(request.getIndex(), request.getStarted());
                            });
                        break;
                    }

                    case SetParameterValueRequest::_capnpPrivate::typeId: {
                        handle_rpc_call<SetParameterValueRequest, SetParameterValueResponse>(
                            [this](auto &request, auto &response) {
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
                    default: {
                        printf("Unknown request_id: %ld", godot_rpc_memory->buffer->request_id);
                        break;
                    }
                    }
                    godot_rpc_memory->buffer->request_id = 0;
                } else {
                    printf("Timed out waiting for request_id");
                }
            }
        }
    }
}

void GodotDistrhoUIServer::shutdown() {
    {
        scoped_lock<interprocess_mutex> lock(godot_rpc_memory->buffer->mutex);
        exit_thread = true;
        godot_rpc_memory->buffer->input_condition.notify_all();
    }
}

END_NAMESPACE_DISTRHO
