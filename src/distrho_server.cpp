#include "distrho_server.h"
#include "distrho_config.h"
#include "distrho_plugin_instance.h"
#include "distrho_shared_memory.h"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/classes/os.hpp"
#include "godot_distrho_shared_memory.h"
#include "version_generated.gen.h"
#include <godot_cpp/core/mutex_lock.hpp>
#include <godot_cpp/classes/mutex.hpp>

using namespace godot;

DistrhoServer *DistrhoServer::singleton = NULL;

DistrhoServer::DistrhoServer() {
    exit_thread = false;
    distrho_config = memnew(DistrhoConfig);
    distrho_plugin = memnew(DistrhoPluginInstance);
	distrho_shared_memory = new GodotDistrhoSharedMemory();
	distrho_shared_memory->initialize(0, 0, "godot-distrho");
    singleton = this;

	for (int i = 0; i < num_channels; ++i)
		buffer[i] = data[i];
}

DistrhoServer::~DistrhoServer() {
	//delete distrho_shared_memory;
    singleton = NULL;
}

DistrhoServer *DistrhoServer::get_singleton() {
    return singleton;
}

void DistrhoServer::initialize() {
}

void DistrhoServer::thread_func() {
	GodotDistrhoSharedMemory *local_distrho_shared_memory;
	local_distrho_shared_memory = new GodotDistrhoSharedMemory();
	//local_distrho_shared_memory->initialize(0, 0, distrho_shared_memory->get_shared_memory_name());
	local_distrho_shared_memory->initialize(0, 0, "godot-distrho");

    while (!exit_thread) {
        lock();

		while(local_distrho_shared_memory->get_sync_flag() != godot::SYNC_FLAG::PLUGIN_TURN) {
			OS::get_singleton()->delay_msec(1);
		}

		local_distrho_shared_memory->read_input_channel(buffer, 1024);

		//TODO: Process the audio.  For now just forward it.

		for (int channel = 0; channel < 2; channel++) {
			for (int frame = 0; frame < BUFFER_SIZE; frame++) {
				buffer[channel][frame] = buffer[channel][frame];
			}
		}

		local_distrho_shared_memory->write_output_channel(buffer, 1024);

		local_distrho_shared_memory->set_sync_flag(godot::SYNC_FLAG::HOST_TURN);

        unlock();
    }
	delete distrho_shared_memory;
}

Error DistrhoServer::start() {
    thread.instantiate();
    mutex.instantiate();
    thread->start(callable_mp(this, &DistrhoServer::thread_func), Thread::PRIORITY_NORMAL);
    return OK;
}

void DistrhoServer::lock() {
    if (thread.is_null() || mutex.is_null()) {
        return;
    }
    mutex->lock();
}

void DistrhoServer::unlock() {
    if (thread.is_null() || mutex.is_null()) {
        return;
    }
    mutex->unlock();
}

void DistrhoServer::finish() {
    exit_thread = true;
    thread->wait_to_finish();
}

DistrhoConfig *DistrhoServer::get_config() {
    return distrho_config;
}

String DistrhoServer::get_version() {
    return GODOT_DISTRHO_VERSION;
}

String DistrhoServer::get_build() {
    return GODOT_DISTRHO_BUILD;
}

void DistrhoServer::set_distrho_launcher(DistrhoLauncher *p_distrho_launcher) {
	distrho_launcher = p_distrho_launcher;
}

DistrhoLauncher *DistrhoServer::get_distrho_launcher() {
	return distrho_launcher;
}

void DistrhoServer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize"), &DistrhoServer::initialize);

    ClassDB::bind_method(D_METHOD("get_config"), &DistrhoServer::get_config);

    ClassDB::bind_method(D_METHOD("get_version"), &DistrhoServer::get_version);
    ClassDB::bind_method(D_METHOD("get_build"), &DistrhoServer::get_build);
}
