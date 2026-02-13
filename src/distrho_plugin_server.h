#ifndef DISTRHO_PLUGIN_SERVER_H
#define DISTRHO_PLUGIN_SERVER_H

#include "DistrhoDetails.hpp"
#include "distrho_audio_port.h"
#include "distrho_circular_buffer.h"
#include "distrho_config.h"
#include "distrho_launcher.h"
#include "distrho_midi_event.h"
#include "distrho_plugin_client.h"
#include "distrho_plugin_instance.h"
#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory_rpc.h"
#include "distrho_ui_client.h"

#include "godot_cpp/classes/mutex.hpp"
#include "godot_cpp/classes/semaphore.hpp"
#include "godot_cpp/classes/thread.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include <functional>
#include <godot_cpp/classes/audio_frame.hpp>
#include <godot_cpp/classes/node.hpp>
#include <mutex>
#include <queue>

namespace godot {

const int num_channels = 16;
const int buffer_size = 2048;

class DistrhoPluginServer : public Object {
    GDCLASS(DistrhoPluginServer, Object);

private:
    Dictionary enumeration_values;
    bool initialized;
    bool is_plugin;

    uint64_t buffer_start_time_usec;
    int process_sample_frame_size;

    DistrhoConfig *distrho_config;
    DistrhoPluginInstance *distrho_plugin;
    DistrhoLauncher *distrho_launcher;

    DistrhoSharedMemory *shared_memory;
    DistrhoSharedMemoryAudio *audio_memory;
    DistrhoSharedMemoryRPC *rpc_memory;
    DistrhoSharedMemoryRPC *godot_rpc_memory;
    DistrhoPluginClient *client;

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
    Ref<Thread> client_thread;

    Vector<DistrhoCircularBuffer *> input_channels;
    Vector<DistrhoCircularBuffer *> output_channels;

    std::queue<MidiEvent> midi_input_queue;
    std::mutex midi_input_mutex;

    std::queue<MidiEvent> midi_output_queue;
    std::mutex midi_output_mutex;

    std::queue<std::pair<String, String>> state_queue;
    std::mutex state_mutex;

    Vector<float> parameters;
    Dictionary state_values;

protected:
    static DistrhoPluginServer *singleton;

public:
    DistrhoPluginServer();
    ~DistrhoPluginServer();

    static DistrhoPluginServer *get_singleton();

    static void _bind_methods();

    void initialize();
    void audio_thread_func();
    void rpc_thread_func();
    void client_thread_func();

    void process();
    void emit_midi_event(MidiEvent &p_midi_event);

    void send_midi_event(Ref<DistrhoMidiEvent> p_midi_event);
    void note_on(int p_channel, int p_note, int p_velocity);
    void note_off(int p_channel, int p_note, int p_velocity);
    void program_change(int p_channel, int p_program_number);
    void control_change(int p_channel, int p_controller, int p_value);
    void pitch_bend(int p_channel, int p_value);
    void channel_pressure(int p_channel, int p_pressure);
    void midi_poly_aftertouch(int p_channel, int p_note, int p_pressure);

    float get_parameter_value(int p_index);
    void set_parameter_value(int p_index, float p_value);

    void update_state_value(String p_key, String p_value);

    void start_buffer_processing();
    uint32_t get_frame_offset_for_event(uint64_t p_event_time_usec);

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

    Ref<DistrhoParameter> create_parameter(const Dictionary &p_data);
    Ref<DistrhoAudioPort> create_audio_port(const Dictionary &p_data);
};
} // namespace godot

#endif
