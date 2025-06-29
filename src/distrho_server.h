#ifndef DISTRHO_SERVER_H
#define DISTRHO_SERVER_H

#include "distrho_circular_buffer.h"
#include "distrho_config.h"
#include "distrho_launcher.h"
#include "distrho_plugin_instance.h"
#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory_rpc.h"
#include "godot_cpp/classes/mutex.hpp"
#include "godot_cpp/classes/semaphore.hpp"
#include "godot_cpp/classes/thread.hpp"
#include <functional>
#include <godot_cpp/classes/audio_frame.hpp>
#include <godot_cpp/classes/node.hpp>

namespace godot {

const int num_channels = 16;
const int buffer_size = 2048;

class DistrhoServer : public Object {
    GDCLASS(DistrhoServer, Object);

private:
    bool initialized;

    DistrhoConfig *distrho_config;
    DistrhoPluginInstance *distrho_plugin;
    DistrhoLauncher *distrho_launcher;
    DistrhoSharedMemoryAudio *audio_memory;
    DistrhoSharedMemoryRPC *rpc_memory;

    float temp_buffer[BUFFER_FRAME_SIZE];

    float input_data[num_channels][buffer_size];
    float *input_buffer[num_channels];

    float output_data[num_channels][buffer_size];
    float *output_buffer[num_channels];

    bool active;

    mutable bool exit_thread;
    Ref<Thread> audio_thread;
    Ref<Mutex> audio_mutex;

    Ref<Thread> rpc_thread;

    Vector<DistrhoCircularBuffer *> input_channels;
    Vector<DistrhoCircularBuffer *> output_channels;

protected:
    static DistrhoServer *singleton;

public:
    DistrhoServer();
    ~DistrhoServer();

    static DistrhoServer *get_singleton();

    static void _bind_methods();

    void initialize();
    void audio_thread_func();
    void rpc_thread_func();

    template <typename T, typename R>
    void handle_rpc_call(std::function<void(typename T::Reader &, typename R::Builder &)> handle_request);

    int process_sample(AudioFrame *p_buffer, float p_rate, int p_frames);

    void set_channel_sample(AudioFrame *p_buffer, float p_rate, int p_frames, int left, int right);
    int get_channel_sample(AudioFrame *p_buffer, float p_rate, int p_frames, int left, int right);

    Error start();
    void lock_audio();
    void unlock_audio();
    void finish();

    DistrhoConfig *get_config();

    String get_version();
    String get_build();

    void set_distrho_launcher(DistrhoLauncher *p_distrho_launcher);
    DistrhoLauncher *get_distrho_launcher();

    void set_distrho_plugin(DistrhoPluginInstance *p_distrho_plugin);
    DistrhoPluginInstance *get_distrho_plugin();
};
} // namespace godot

#endif
