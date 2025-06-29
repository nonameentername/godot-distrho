#ifndef DISTRHO_CIRCULAR_BUFFER_H
#define DISTRHO_CIRCULAR_BUFFER_H

namespace godot {

const int CIRCULAR_BUFFER_SIZE = 2048;

struct AudioRingBuffer {
    int input_write_index = 0;
    int input_read_index = 0;

    int output_write_index = 0;
    int output_read_index = 0;

    float buffer[CIRCULAR_BUFFER_SIZE];
};

class DistrhoCircularBuffer {

private:
    AudioRingBuffer *audio_buffer;

public:
    DistrhoCircularBuffer();
    ~DistrhoCircularBuffer();

    void write_channel(const float *p_buffer, int p_frames);

    void read_channel(float *p_buffer, int p_frames);
};

} // namespace godot

#endif
