#ifndef DISTRHO_MIDI_EVENT_H
#define DISTRHO_MIDI_EVENT_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class DistrhoMidiEvent : public RefCounted {
    GDCLASS(DistrhoMidiEvent, RefCounted);

private:
    int channel;
    int status;
    int data1;
    int data2;
    int frame;

protected:
public:
    DistrhoMidiEvent();
    ~DistrhoMidiEvent();

    void set_channel(int p_channel);
    int get_channel();

    void set_status(int p_status);
    int get_status();

    void set_data1(int p_data1);
    int get_data1();

    void set_data2(int p_data2);
    int get_data2();

    void set_frame(int p_frame);
    int get_frame();

    static void _bind_methods();
};
} // namespace godot

#endif
