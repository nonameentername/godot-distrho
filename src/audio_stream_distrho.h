#ifndef AUDIO_STREAM_DISTRHO_H
#define AUDIO_STREAM_DISTRHO_H

#include <godot_cpp/classes/audio_frame.hpp>
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/templates/vector.hpp>

namespace godot {

class AudioStreamDistrho : public AudioStream {
    GDCLASS(AudioStreamDistrho, AudioStream)

private:
    friend class AudioStreamPlaybackDistrho;
    String distrho_name;
    bool active;

public:
    virtual String get_stream_name() const;
    virtual float get_length() const;

    int process_sample(AudioFrame *p_buffer, float p_rate, int p_frames);

    AudioStreamDistrho();
    ~AudioStreamDistrho();

    virtual Ref<AudioStreamPlayback> _instantiate_playback() const override;

protected:
    static void _bind_methods();
};
} // namespace godot

#endif
