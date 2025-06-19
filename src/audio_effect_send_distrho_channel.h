#ifndef AUDIO_EFFECT_SEND_DISTRHO_CHANNEL_H
#define AUDIO_EFFECT_SEND_DISTRHO_CHANNEL_H

#include <godot_cpp/classes/audio_effect.hpp>
#include <godot_cpp/classes/audio_effect_instance.hpp>
#include <godot_cpp/classes/audio_frame.hpp>
#include <godot_cpp/classes/audio_server.hpp>

namespace godot {

class AudioEffectSetDistrhoChannel;

class AudioEffectSetDistrhoChannelInstance : public AudioEffectInstance {
    GDCLASS(AudioEffectSetDistrhoChannelInstance, AudioEffectInstance);

private:
    friend class AudioEffectSetDistrhoChannel;
    Ref<AudioEffectSetDistrhoChannel> base;
    bool has_data = false;

public:
    virtual void _process(const void *src_buffer, AudioFrame *dst_buffer, int32_t frame_count) override;
    virtual bool _process_silence() const override;

protected:
    static void _bind_methods();
};

class AudioEffectSetDistrhoChannel : public AudioEffect {
    GDCLASS(AudioEffectSetDistrhoChannel, AudioEffect)
    friend class AudioEffectSetDistrhoChannelInstance;

    String distrho_name;
    int channel_left = 0;
    int channel_right = 1;
    bool forward_audio = true;

protected:
    static void _bind_methods();

public:
    AudioEffectSetDistrhoChannel();
    ~AudioEffectSetDistrhoChannel();

    virtual Ref<AudioEffectInstance> _instantiate() override;

    void set_channel_left(int p_channel_left);
    int get_channel_left();

    void set_channel_right(int p_channel_right);
    int get_channel_right();

    void set_forward_audio(bool p_forward_audio);
    bool get_forward_audio();
};

} // namespace godot

#endif
