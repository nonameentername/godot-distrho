#include "audio_stream_player_distrho.h"
#include "audio_stream_distrho.h"

using namespace godot;

AudioStreamPlaybackDistrho::AudioStreamPlaybackDistrho() : active(false) {
}

AudioStreamPlaybackDistrho::~AudioStreamPlaybackDistrho() {
    _stop();
}

void AudioStreamPlaybackDistrho::_stop() {
    active = false;
}

void AudioStreamPlaybackDistrho::_start(double p_from_pos) {
    active = true;
}

void AudioStreamPlaybackDistrho::_seek(double p_time) {
    if (p_time < 0) {
        p_time = 0;
    }
}

int AudioStreamPlaybackDistrho::_mix(AudioFrame *p_buffer, float p_rate, int p_frames) {
    ERR_FAIL_COND_V(!active, 0);
    if (!active) {
        return 0;
    }

    return base->process_sample(p_buffer, p_rate, p_frames);
}

void AudioStreamPlaybackDistrho::_tag_used_streams() {
}

int AudioStreamPlaybackDistrho::_get_loop_count() const {
    return 10;
}

double AudioStreamPlaybackDistrho::_get_playback_position() const {
    return 0;
}

float AudioStreamPlaybackDistrho::_get_length() const {
    return 0.0;
}

bool AudioStreamPlaybackDistrho::_is_playing() const {
    return active;
}

void AudioStreamPlaybackDistrho::_bind_methods() {
}
