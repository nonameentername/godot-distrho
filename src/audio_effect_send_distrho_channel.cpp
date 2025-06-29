#include "audio_effect_send_distrho_channel.h"
#include "distrho_server.h"
#include "godot_cpp/classes/audio_server.hpp"

using namespace godot;

AudioEffectSetDistrhoChannel::AudioEffectSetDistrhoChannel() {
}

AudioEffectSetDistrhoChannel::~AudioEffectSetDistrhoChannel() {
}

void AudioEffectSetDistrhoChannel::set_channel_left(int p_channel_left) {
    channel_left = p_channel_left;
}

int AudioEffectSetDistrhoChannel::get_channel_left() {
    return channel_left;
}

void AudioEffectSetDistrhoChannel::set_channel_right(int p_channel_right) {
    channel_right = p_channel_right;
}

int AudioEffectSetDistrhoChannel::get_channel_right() {
    return channel_right;
}

void AudioEffectSetDistrhoChannel::set_forward_audio(bool p_forward_audio) {
    forward_audio = p_forward_audio;
}

bool AudioEffectSetDistrhoChannel::get_forward_audio() {
    return forward_audio;
}

void AudioEffectSetDistrhoChannel::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_channel_left", "channel"), &AudioEffectSetDistrhoChannel::set_channel_left);
    ClassDB::bind_method(D_METHOD("get_channel_left"), &AudioEffectSetDistrhoChannel::get_channel_left);
    ClassDB::add_property("AudioEffectSetDistrhoChannel", PropertyInfo(Variant::INT, "channel_left"),
                          "set_channel_left", "get_channel_left");
    ClassDB::bind_method(D_METHOD("set_channel_right", "channel"), &AudioEffectSetDistrhoChannel::set_channel_right);
    ClassDB::bind_method(D_METHOD("get_channel_right"), &AudioEffectSetDistrhoChannel::get_channel_right);
    ClassDB::add_property("AudioEffectSetDistrhoChannel", PropertyInfo(Variant::INT, "channel_right"),
                          "set_channel_right", "get_channel_right");
    ClassDB::bind_method(D_METHOD("set_forward_audio", "forward_audio"),
                         &AudioEffectSetDistrhoChannel::set_forward_audio);
    ClassDB::bind_method(D_METHOD("get_forward_audio"), &AudioEffectSetDistrhoChannel::get_forward_audio);
    ClassDB::add_property("AudioEffectSetDistrhoChannel", PropertyInfo(Variant::BOOL, "forward_audio"),
                          "set_forward_audio", "get_forward_audio");
}

Ref<AudioEffectInstance> AudioEffectSetDistrhoChannel::_instantiate() {
    Ref<AudioEffectSetDistrhoChannelInstance> ins;
    ins.instantiate();
    ins->base = Ref<AudioEffectSetDistrhoChannel>(this);

    return ins;
}

void AudioEffectSetDistrhoChannelInstance::_process(const void *p_src_frames, AudioFrame *p_dst_frames,
                                                    int p_frame_count) {
    AudioFrame *src_frames = (AudioFrame *)p_src_frames;

    for (int i = 0; i < p_frame_count; i++) {
        if (base->forward_audio) {
            p_dst_frames[i] = src_frames[i];
        } else {
            p_dst_frames[i].left = 0;
            p_dst_frames[i].right = 0;
        }
        if (src_frames[i].left > 0 || src_frames[i].right > 0) {
            has_data = true;
        }
    }

    if (!has_data) {
        return;
    }

    int p_rate = 1;
    DistrhoServer::get_singleton()->set_channel_sample(src_frames, p_rate, p_frame_count, base->channel_left,
                                                       base->channel_right);
}

bool AudioEffectSetDistrhoChannelInstance::_process_silence() const {
    return true;
}

void AudioEffectSetDistrhoChannelInstance::_bind_methods() {
}
