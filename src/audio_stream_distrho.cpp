#include "audio_stream_distrho.h"
#include "audio_stream_player_distrho.h"
#include "distrho_plugin_server.h"
#include "godot_cpp/classes/audio_stream.hpp"

using namespace godot;

AudioStreamDistrho::AudioStreamDistrho() {
    active = false;
}

AudioStreamDistrho::~AudioStreamDistrho() {
}

Ref<AudioStreamPlayback> AudioStreamDistrho::_instantiate_playback() const {
    Ref<AudioStreamPlaybackDistrho> talking_tree;
    talking_tree.instantiate();
    talking_tree->base = Ref<AudioStreamDistrho>(this);

    return talking_tree;
}

godot::String AudioStreamDistrho::get_stream_name() const {
    return "Distrho";
}

float AudioStreamDistrho::get_length() const {
    return 0;
}

int AudioStreamDistrho::process_sample(AudioFrame *p_buffer, float p_rate, int p_frames) {
    return DistrhoPluginServer::get_singleton()->process_sample(p_buffer, p_rate, p_frames);
}

void AudioStreamDistrho::_bind_methods() {
}
