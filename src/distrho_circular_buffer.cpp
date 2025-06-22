#include "distrho_circular_buffer.h"

using namespace godot;


DistrhoCircularBuffer::DistrhoCircularBuffer() {
    audio_buffer = new AudioRingBuffer();
}

DistrhoCircularBuffer::~DistrhoCircularBuffer() {
    delete audio_buffer;
}

void DistrhoCircularBuffer::write_channel(const float *p_buffer, int p_frames) {
    for (int frame = 0; frame < p_frames; frame++) {
        audio_buffer->buffer[(audio_buffer->input_write_index + frame) % CIRCULAR_BUFFER_SIZE] = p_buffer[frame];
    }
  	audio_buffer->input_write_index = (audio_buffer->input_write_index + p_frames) % CIRCULAR_BUFFER_SIZE;
}

void DistrhoCircularBuffer::read_channel(float *p_buffer, int p_frames) {
    for (int frame = 0; frame < p_frames; frame++) {
        p_buffer[frame] = audio_buffer->buffer[(audio_buffer->input_read_index + frame) % CIRCULAR_BUFFER_SIZE];
    }
  	audio_buffer->input_read_index = (audio_buffer->input_read_index + p_frames) % CIRCULAR_BUFFER_SIZE;
}
