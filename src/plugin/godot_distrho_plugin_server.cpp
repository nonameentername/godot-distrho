#include "godot_distrho_plugin_server.h"
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

GodotDistrhoPluginServer::GodotDistrhoPluginServer(Plugin *p_godot_distrho_plugin, godot::DistrhoSharedMemoryRPC *p_godot_rpc_memory) {
    exit_thread = false;
    godot_distrho_plugin = p_godot_distrho_plugin;
    godot_rpc_memory = p_godot_rpc_memory;

    if (p_godot_distrho_plugin != NULL) {
        rpc_thread = std::thread(&GodotDistrhoPluginServer::rpc_thread_func, this);
    }
}

GodotDistrhoPluginServer::~GodotDistrhoPluginServer() {
    if (godot_distrho_plugin != NULL) {
        exit_thread = true;
        rpc_thread.join();
    }
}

template <typename T, typename R>
void GodotDistrhoPluginServer::handle_rpc_call(
    std::function<void(typename T::Reader &, typename R::Builder &)> handle_request) {
    return DistrhoCommon::handle_rpc_call<T, R>(*godot_rpc_memory, handle_request);
}

void GodotDistrhoPluginServer::rpc_thread_func() {
    {
        scoped_lock<interprocess_mutex> shared_memory_lock(godot_rpc_memory->buffer->mutex);
        godot_rpc_memory->buffer->ready = true;
    }

    bool first_wait = true;

    while (!exit_thread) {
        {
            scoped_lock<interprocess_mutex> shared_memory_lock(godot_rpc_memory->buffer->mutex);

            if (godot_rpc_memory->buffer->ready) {
                ptime timeout = microsec_clock::universal_time() + milliseconds(first_wait ? 1000 : 100);
                bool result = godot_rpc_memory->buffer->input_condition.timed_wait(shared_memory_lock, timeout);
                first_wait = false;

                if (!result) {
                    // TODO: log in debug mode
                    //printf("Processing request_id: %ld\n", godot_rpc_memory->buffer->request_id);

                    switch (godot_rpc_memory->buffer->request_id) {
                    case UpdateStateValueRequest::_capnpPrivate::typeId: {
                        handle_rpc_call<UpdateStateValueRequest, UpdateStateValueResponse>(
                            [this](auto &request, auto &response) {
                                godot_distrho_plugin->updateStateValue(request.getKey().cStr(), request.getValue().cStr());
                            });
                        break;
                    }
                    default: {
                        //printf("Unknown request_id: %ld\n", godot_rpc_memory->buffer->request_id);
                        break;
                    }
                    }
                    godot_rpc_memory->buffer->request_id = 0;
                }

                switch (godot_rpc_memory->buffer->request_id) {}
            }
        }
    }
}

bool GodotDistrhoPluginServer::shutdown() {
    {
        scoped_lock<interprocess_mutex> lock(godot_rpc_memory->buffer->mutex);
        exit_thread = true;
        godot_rpc_memory->buffer->input_condition.notify_all();
    }
}

END_NAMESPACE_DISTRHO
