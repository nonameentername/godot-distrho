#ifndef DISTRHO_SERVER_H
#define DISTRHO_SERVER_H

#include "distrho_config.h"
#include "distrho_launcher.h"
#include "distrho_plugin_instance.h"
#include "distrho_shared_memory.h"
//#include "distrho_shared_memory.h"
#include <godot_cpp/classes/node.hpp>
#include "godot_cpp/classes/thread.hpp"
#include "godot_cpp/classes/mutex.hpp"
#include "godot_cpp/classes/semaphore.hpp"
#include <godot_cpp/classes/audio_frame.hpp>


namespace godot {

const int num_channels = 16;
const int buffer_size = 2048;

//static const int BUFFER_FRAME_SIZE = 512;
static const int CIRCULAR_BUFFER_SIZE = BUFFER_SIZE + 10;
    
class DistrhoServer : public Object {
    GDCLASS(DistrhoServer, Object);

private:
    DistrhoConfig *distrho_config;
    DistrhoPluginInstance *distrho_plugin;
	DistrhoLauncher *distrho_launcher;
	DistrhoSharedMemory *distrho_shared_memory;

    float temp_buffer[BUFFER_FRAME_SIZE];

	float input_data[num_channels][buffer_size];
	float *input_buffer[num_channels];

	float output_data[num_channels][buffer_size];
	float *output_buffer[num_channels];

    mutable bool exit_thread;
    Ref<Thread> thread;
    Ref<Mutex> mutex;
    Ref<Semaphore> semaphore;

    Vector<void *> input_channels;
    Vector<void *> output_channels;

protected:
    static DistrhoServer *singleton;

public:
    DistrhoServer();
    ~DistrhoServer();

    static DistrhoServer *get_singleton();

    static void _bind_methods();

    void initialize();
    void thread_func();

    int process_sample(AudioFrame *p_buffer, float p_rate, int p_frames);

    void set_channel_sample(AudioFrame *p_buffer, float p_rate, int p_frames, int left, int right);
    int get_channel_sample(AudioFrame *p_buffer, float p_rate, int p_frames, int left, int right);

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
