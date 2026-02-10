#ifndef DISTRHO_UI_SERVER_H
#define DISTRHO_UI_SERVER_H

#include "DistrhoDetails.hpp"
#include "distrho_circular_buffer.h"
#include "distrho_config.h"
#include "distrho_launcher.h"
#include "distrho_midi_event.h"
#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory_rpc.h"
#include "distrho_ui_client.h"
#include "distrho_ui_instance.h"
#include "godot_cpp/classes/mutex.hpp"
#include "godot_cpp/classes/semaphore.hpp"
#include "godot_cpp/classes/thread.hpp"
#include <functional>
#include <godot_cpp/classes/audio_frame.hpp>
#include <godot_cpp/classes/node.hpp>
#include <mutex>
#include <queue>

namespace godot {

class DistrhoUIServer : public Object {
    GDCLASS(DistrhoUIServer, Object);

private:
    bool initialized;
    bool is_ui;

    DistrhoUIInstance *distrho_ui;
    DistrhoLauncher *distrho_launcher;

    DistrhoSharedMemoryRPC *rpc_memory;
    DistrhoSharedMemoryRPC *godot_rpc_memory;
    DistrhoUIClient *client;

    bool active;

    mutable bool exit_thread;
    Ref<Thread> rpc_thread;

protected:
    static DistrhoUIServer *singleton;

public:
    DistrhoUIServer();
    ~DistrhoUIServer();

    static DistrhoUIServer *get_singleton();

    static void _bind_methods();

    void initialize();
    void rpc_thread_func();

    void process();

    void send_note_on(int channel, int note, int velocity);
    void send_note_off(int channel, int note);

    void set_parameter_value(int p_index, float p_value);
    void set_state_value(String p_key, String p_value);

    template <typename T, typename R>
    void handle_rpc_call(std::function<void(typename T::Reader &, typename R::Builder &)> handle_request);

    Error start();
    void finish();

    String get_version();
    String get_build();

    void set_distrho_launcher(DistrhoLauncher *p_distrho_launcher);
    DistrhoLauncher *get_distrho_launcher();

    void set_distrho_ui(DistrhoUIInstance *p_distrho_ui);
    DistrhoUIInstance *get_distrho_ui();
};
} // namespace godot

#endif
