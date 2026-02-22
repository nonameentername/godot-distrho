#include "distrho_ui_server.h"
#include "distrho_common.h"
#include "distrho_launcher.h"
#include "distrho_plugin_server.h"
#include "distrho_shared_memory_region.h"
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
        module_type = std::to_string(DistrhoCommon::UI_TYPE).c_str();
    }

    is_ui = std::stoi(module_type) == DistrhoCommon::UI_TYPE;

    initialized = false;
    active = false;
    exit_thread = false;
    singleton = this;

    distrho_ui = memnew(DistrhoUIInstance);

    if (is_ui) {
        const char *shared_memory_uuid = std::getenv("DISTRHO_SHARED_MEMORY_UUID");
        if (shared_memory_uuid == NULL) {
            shared_memory_uuid = "";
        }

        shared_memory = new DistrhoSharedMemory();
        rpc_memory = new DistrhoSharedMemoryRPC();
        godot_rpc_memory = new DistrhoSharedMemoryRPC();
        shared_memory_region = new DistrhoSharedMemoryRegion();

        int memory_size = rpc_memory->get_memory_size() + godot_rpc_memory->get_memory_size();

        shared_memory->initialize(shared_memory_uuid, memory_size);
        rpc_memory->initialize(shared_memory, RPC_BUFFER_NAME);
        godot_rpc_memory->initialize(shared_memory, GODOT_RPC_BUFFER_NAME);
        shared_memory_region->initialize(shared_memory);

        client = new DistrhoUIClient(godot_rpc_memory, shared_memory_region);

        call_deferred("initialize");
    }
}

DistrhoUIServer::~DistrhoUIServer() {
    if (is_ui) {
        scoped_lock<interprocess_mutex> lock(godot_rpc_memory->buffer->mutex);
        exit_thread = true;
        godot_rpc_memory->buffer->input_condition.notify_all();
    }

    if (rpc_thread != NULL && rpc_thread->is_alive()) {
        rpc_thread->wait_to_finish();
    }

    //TODO: delete missing for these?
    rpc_memory = nullptr;
    godot_rpc_memory = nullptr;
    shared_memory_region = nullptr;

    if (is_ui) {
        delete client;
    }

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

        Vector<Ref<DistrhoParameter>> default_parameters = DistrhoPluginServer::get_singleton()->get_distrho_plugin()->_get_parameters();
        parameters.resize(default_parameters.size());
        shared_memory_region->initialize_parameters(default_parameters.size());

        for (int i = 0; i < default_parameters.size(); i++) {
            parameters.set(i, default_parameters.get(i)->get_default_value());
            shared_memory_region->write_parameter_value(i, default_parameters.get(i)->get_default_value());
        }
    }

    start();
    initialized = true;
}

template <typename T, typename R>
void DistrhoUIServer::handle_rpc_call(std::function<void(typename T::Reader &, typename R::Builder &)> handle_request) {
    return DistrhoCommon::handle_rpc_call<T, R>(*rpc_memory, handle_request);
}

void DistrhoUIServer::rpc_thread_func() {
    {
        scoped_lock<interprocess_mutex> shared_memory_lock(rpc_memory->buffer->mutex);
        rpc_memory->buffer->ready = true;
    }

    bool first_wait = true;

    while (!exit_thread) {
        scoped_lock<interprocess_mutex> shared_memory_lock(rpc_memory->buffer->mutex);

        ptime timeout = microsec_clock::universal_time() + milliseconds(first_wait ? 1000 : 100);

        bool result = rpc_memory->buffer->input_condition.timed_wait(
            shared_memory_lock, timeout, [this]() { return rpc_memory->buffer->request_id != 0; });
        first_wait = false;

        if (result) {
            // TODO: log in debug only
            // printf("Processing request_id: %ld", godot_rpc_memory->buffer->request_id);

            switch (rpc_memory->buffer->request_id) {

            case GetNativeWindowIdRequest::_capnpPrivate::typeId: {
                handle_rpc_call<GetNativeWindowIdRequest, GetNativeWindowIdResponse>([this](auto &request, auto &response) {
                    int64_t value = DisplayServer::get_singleton()->window_get_native_handle(DisplayServer::WINDOW_HANDLE, 0);
                    response.setId(value);
                });
                break;
            }

            case ParameterChangedRequest::_capnpPrivate::typeId: {
                handle_rpc_call<ParameterChangedRequest, ParameterChangedResponse>(
                    [this](auto &request, auto &response) {
                        call_deferred("emit_signal", "parameter_changed", request.getIndex(), request.getValue());
                    });
                break;
            }

            case StateChangedRequest::_capnpPrivate::typeId: {
                handle_rpc_call<StateChangedRequest, StateChangedResponse>(
                    [this](auto &request, auto &response) {
                        call_deferred("emit_signal", "state_changed", request.getKey().cStr(), request.getValue().cStr());
                    });
                break;
            }

            case ShutdownRequest::_capnpPrivate::typeId: {
                handle_rpc_call<ShutdownRequest, ShutdownResponse>([this](auto &request, auto &response) {
                    // exit_thread = true;
                    response.setResult(true);
                });
                break;
            }

            default: {
                // printf("Unknown request_id: %ld", rpc_memory->buffer->request_id);
                break;
            }
            }
            rpc_memory->buffer->request_id = 0;
        } else {
            // printf("Timed out waiting for request_id");
        }
    }

    SceneTree *tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
    if (tree != NULL) {
        tree->quit();
    }
}

void DistrhoUIServer::process() {
    for (int i = 0; i < shared_memory_region->get_parameter_count(); i++) {
        float value = shared_memory_region->read_parameter_value(i);
        if (parameters[i] != value) {
            parameters.ptrw()[i] = value;
            call_deferred("emit_signal", "parameter_changed", i, value);
            shared_memory_region->write_parameter_value(i, parameters[i]);
        }
    }
}

void DistrhoUIServer::send_note_on(int channel, int note, int velocity) {
    client->send_note(channel, note, velocity);
}

void DistrhoUIServer::send_note_off(int channel, int note) {
    client->send_note(channel, note, 0);
}

void DistrhoUIServer::set_parameter_value(int p_index, float p_value) {
    parameters.ptrw()[p_index] = p_value;
    shared_memory_region->write_parameter_value(p_index, p_value);
}

void DistrhoUIServer::set_state_value(String p_key, String p_value) {
    client->set_state(p_key, p_value);
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
        if (rpc_thread != NULL && rpc_thread->is_alive()) {
            rpc_thread->wait_to_finish();
        }
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

    ClassDB::bind_method(D_METHOD("set_parameter_value", "index", "value"), &DistrhoUIServer::set_parameter_value);
    ClassDB::bind_method(D_METHOD("set_state_value", "key", "value"), &DistrhoUIServer::set_state_value);

    ClassDB::bind_method(D_METHOD("set_distrho_ui", "distrho_ui"), &DistrhoUIServer::set_distrho_ui);

    ClassDB::bind_method(D_METHOD("get_version"), &DistrhoUIServer::get_version);
    ClassDB::bind_method(D_METHOD("get_build"), &DistrhoUIServer::get_build);

    ADD_SIGNAL(
        MethodInfo("parameter_changed", PropertyInfo(Variant::INT, "index"), PropertyInfo(Variant::FLOAT, "value")));

    ADD_SIGNAL(MethodInfo("state_changed", PropertyInfo(Variant::STRING, "key"), PropertyInfo(Variant::STRING, "value")));
}
