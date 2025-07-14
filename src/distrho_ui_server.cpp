#include "distrho_ui_server.h"
#include "distrho_common.h"
#include "distrho_launcher.h"
#include "distrho_shared_memory_rpc.h"
#include "distrho_ui_client.h"
#include "distrho_ui_instance.h"
#include "distrho_ui_server_node.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_distrho_schema.capnp.h"
#include "version_generated.gen.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <capnp/serialize.h>
#include <cstdlib>
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/core/mutex_lock.hpp>
#include <kj/string.h>
#include <unistd.h>

using namespace godot;
using namespace boost::interprocess;
using namespace boost::posix_time;

DistrhoUIServer *DistrhoUIServer::singleton = NULL;

DistrhoUIServer::DistrhoUIServer() {
    const char *module_type = std::getenv("DISTRHO_MODULE_TYPE");
    if (module_type == NULL) {
        module_type = std::to_string(DistrhoCommon::PLUGIN_TYPE).c_str();
    }

    is_ui = std::stoi(module_type) == DistrhoCommon::UI_TYPE;

    initialized = false;
    active = false;
    exit_thread = false;
    singleton = this;

    distrho_ui = memnew(DistrhoUIInstance);

    if (is_ui) {
        const char *rpc_shared_memory = std::getenv("DISTRHO_SHARED_MEMORY_RPC");
        if (rpc_shared_memory == NULL) {
            rpc_shared_memory = "";
        }
        rpc_memory = new DistrhoSharedMemoryRPC();
        rpc_memory->initialize("DISTRHO_SHARED_MEMORY_RPC", rpc_shared_memory);

        const char *godot_rpc_shared_memory = std::getenv("GODOT_SHARED_MEMORY_RPC");
        if (godot_rpc_shared_memory == NULL) {
            godot_rpc_shared_memory = "";
        }
        godot_rpc_memory = new DistrhoSharedMemoryRPC();
        godot_rpc_memory->initialize("GODOT_SHARED_MEMORY_RPC", godot_rpc_shared_memory);

        client = new DistrhoUIClient(godot_rpc_memory);

        call_deferred("initialize");
    }
}

DistrhoUIServer::~DistrhoUIServer() {
    // delete distrho_shared_memory_audio;
    // delete distrho_shared_memory_rpc;

    delete client;
    singleton = NULL;
}

DistrhoUIServer *DistrhoUIServer::get_singleton() {
    return singleton;
}

void DistrhoUIServer::initialize() {
    if (!initialized) {
        // TODO: Does the ui need a node?
        Node *distrho_server_node = memnew(DistrhoUIServerNode);
        SceneTree *tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
        tree->get_root()->add_child(distrho_server_node);
        distrho_server_node->set_process(true);
    }

    start();
    initialized = true;
}

template <typename T, typename R>
void DistrhoUIServer::handle_rpc_call(std::function<void(typename T::Reader &, typename R::Builder &)> handle_request) {
    return DistrhoCommon::handle_rpc_call<T, R>(*rpc_memory, handle_request);
}

void DistrhoUIServer::rpc_thread_func() {
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

        case GetSomeTextRequest::_capnpPrivate::typeId: {
            handle_rpc_call<GetSomeTextRequest, GetSomeTextResponse>([this](auto &request, auto &response) {
                String value = DistrhoUIServer::get_singleton()->get_distrho_ui()->_get_some_text();
                response.setText(std::string(value.ascii()));
            });
            break;
        }

        case ShutdownRequest::_capnpPrivate::typeId: {
            handle_rpc_call<ShutdownRequest, ShutdownResponse>([this](auto &request, auto &response) {
                // TODO: remove duplicate
                exit_thread = true;
                SceneTree *tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
                tree->quit();
                response.setResult(true);
            });
            break;
        }
        }
    }
    delete rpc_memory;
}

void DistrhoUIServer::process() {
}

void DistrhoUIServer::send_note_on(int channel, int note, int velocity) {
    client->send_note(channel, note, velocity);
}

void DistrhoUIServer::send_note_off(int channel, int note) {
    client->send_note(channel, note, 0);
}

Error DistrhoUIServer::start() {
    if (!godot::Engine::get_singleton()->is_editor_hint()) {
        rpc_thread.instantiate();
        rpc_thread->start(callable_mp(this, &DistrhoUIServer::rpc_thread_func), Thread::PRIORITY_NORMAL);
    }
    return OK;
}

void DistrhoUIServer::finish() {
    if (!godot::Engine::get_singleton()->is_editor_hint()) {
        exit_thread = true;
        rpc_thread->wait_to_finish();
    }
}

godot::String DistrhoUIServer::get_version() {
    return GODOT_DISTRHO_VERSION;
}

godot::String DistrhoUIServer::get_build() {
    return GODOT_DISTRHO_BUILD;
}

void DistrhoUIServer::set_distrho_launcher(DistrhoLauncher *p_distrho_launcher) {
    distrho_launcher = p_distrho_launcher;
}

DistrhoLauncher *DistrhoUIServer::get_distrho_launcher() {
    return distrho_launcher;
}

void DistrhoUIServer::set_distrho_ui(DistrhoUIInstance *p_distrho_ui) {
    distrho_ui = p_distrho_ui;
}

DistrhoUIInstance *DistrhoUIServer::get_distrho_ui() {
    return distrho_ui;
}

void DistrhoUIServer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize"), &DistrhoUIServer::initialize);

    ClassDB::bind_method(D_METHOD("send_note_on", "channel", "note", "velocity"), &DistrhoUIServer::send_note_on);
    ClassDB::bind_method(D_METHOD("send_note_off", "channel", "note"), &DistrhoUIServer::send_note_off);

    ClassDB::bind_method(D_METHOD("set_distrho_ui", "distrho_ui"), &DistrhoUIServer::set_distrho_ui);

    ClassDB::bind_method(D_METHOD("get_version"), &DistrhoUIServer::get_version);
    ClassDB::bind_method(D_METHOD("get_build"), &DistrhoUIServer::get_build);
}
