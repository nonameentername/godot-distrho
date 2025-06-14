#ifndef DISTRHO_SERVER_H
#define DISTRHO_SERVER_H

#include "distrho_config.h"
#include "distrho_launcher.h"
#include "distrho_plugin_instance.h"
#include "godot_distrho_shared_memory.h"
//#include "distrho_shared_memory.h"
#include <godot_cpp/classes/node.hpp>
#include "godot_cpp/classes/thread.hpp"
#include "godot_cpp/classes/mutex.hpp"

namespace godot {

const int num_channels = 16;
const int buffer_size = 1024;
    
class DistrhoServer : public Object {
    GDCLASS(DistrhoServer, Object);

private:
    DistrhoConfig *distrho_config;
    DistrhoPluginInstance *distrho_plugin;
	DistrhoLauncher *distrho_launcher;
	GodotDistrhoSharedMemory *distrho_shared_memory;

	float data[num_channels][buffer_size];
	float *buffer[num_channels];

    mutable bool exit_thread;
    Ref<Thread> thread;
    Ref<Mutex> mutex;

protected:
    static DistrhoServer *singleton;

public:
    DistrhoServer();
    ~DistrhoServer();

    static DistrhoServer *get_singleton();

    static void _bind_methods();

    void initialize();
    void thread_func();

    Error start();
    void lock();
    void unlock();
    void finish();

    DistrhoConfig *get_config();

    String get_version();
    String get_build();

	void set_distrho_launcher(DistrhoLauncher *p_distrho_launcher);
	DistrhoLauncher *get_distrho_launcher();
};
}

#endif
