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

GodotDistrhoPluginServer::GodotDistrhoPluginServer(godot::DistrhoSharedMemoryRPC *p_rpc_memory) {
    exit_thread = false;
    rpc_memory = p_rpc_memory;

    // TODO: implement server and client for plugin and ui
}

GodotDistrhoPluginServer::~GodotDistrhoPluginServer() {
}

template <typename T, typename R>
void GodotDistrhoPluginServer::handle_rpc_call(
    std::function<void(typename T::Reader &, typename R::Builder &)> handle_request) {
    return DistrhoCommon::handle_rpc_call<T, R>(rpc_memory, handle_request);
}

void GodotDistrhoPluginServer::rpc_thread_func() {
    scoped_lock<interprocess_mutex> shared_memory_lock(rpc_memory->buffer->mutex);
    rpc_memory->buffer->ready = true;
    shared_memory_lock.unlock();

    bool first_wait = true;

    while (!exit_thread) {
        scoped_lock<interprocess_mutex> shared_memory_lock(rpc_memory->buffer->mutex);

        ptime timeout = microsec_clock::universal_time() + milliseconds(first_wait ? 1000 : 100);
        bool result = rpc_memory->buffer->input_condition.timed_wait(shared_memory_lock, timeout);
        first_wait = false;

        if (!result) {
            continue;
        }

        switch (rpc_memory->buffer->request_id) {}
    }
}

bool GodotDistrhoPluginServer::shutdown() {
    exit_thread = true;
}

END_NAMESPACE_DISTRHO
