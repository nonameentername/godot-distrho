#include "distrho_midi_event.h"

using namespace godot;

DistrhoMidiEvent::DistrhoMidiEvent() {
    channel = 0;
    status = 0;
    data1 = 0;
    data2 = 0;
    frame = 0;
}

DistrhoMidiEvent::~DistrhoMidiEvent() {
}

void DistrhoMidiEvent::set_channel(int p_channel) {
    channel = p_channel;
}

int DistrhoMidiEvent::get_channel() {
    return channel;
}

void DistrhoMidiEvent::set_status(int p_status) {
    status = p_status;
}

int DistrhoMidiEvent::get_status() {
    return status;
}

void DistrhoMidiEvent::set_data1(int p_data1) {
    data1 = p_data1;
}

int DistrhoMidiEvent::get_data1() {
    return data1;
}

void DistrhoMidiEvent::set_data2(int p_data2) {
    data2 = p_data2;
}

int DistrhoMidiEvent::get_data2() {
    return data2;
}

void DistrhoMidiEvent::set_frame(int p_frame) {
    frame = p_frame;
}

int DistrhoMidiEvent::get_frame() {
    return frame;
}

void DistrhoMidiEvent::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_channel", "channel"), &DistrhoMidiEvent::set_channel);
    ClassDB::bind_method(D_METHOD("get_channel"), &DistrhoMidiEvent::get_channel);

    ClassDB::bind_method(D_METHOD("set_status", "status"), &DistrhoMidiEvent::set_status);
    ClassDB::bind_method(D_METHOD("get_status"), &DistrhoMidiEvent::get_status);

    ClassDB::bind_method(D_METHOD("set_data1", "data1"), &DistrhoMidiEvent::set_data1);
    ClassDB::bind_method(D_METHOD("get_data1"), &DistrhoMidiEvent::get_data1);

    ClassDB::bind_method(D_METHOD("set_data2", "data2"), &DistrhoMidiEvent::set_data2);
    ClassDB::bind_method(D_METHOD("get_data2"), &DistrhoMidiEvent::get_data2);

    ClassDB::bind_method(D_METHOD("set_frame", "frame"), &DistrhoMidiEvent::set_frame);
    ClassDB::bind_method(D_METHOD("get_frame"), &DistrhoMidiEvent::get_frame);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "channel"), "set_channel", "get_channel");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "status"), "set_status", "get_status");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "data1"), "set_data1", "get_data1");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "data2"), "set_data2", "get_data2");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "frame"), "set_frame", "get_frame");
}
