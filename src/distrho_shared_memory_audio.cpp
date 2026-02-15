#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory.h"

using namespace godot;

namespace bip = boost::interprocess;

static boost::uuids::random_generator &uuid_gen() {
    static boost::uuids::random_generator gen;
    return gen;
}

DistrhoSharedMemoryAudio::DistrhoSharedMemoryAudio() {
}

DistrhoSharedMemoryAudio::~DistrhoSharedMemoryAudio() {
}

void DistrhoSharedMemoryAudio::initialize(DistrhoSharedMemory *p_distrho_shared_memory,
        int p_number_of_input_channels, int p_number_of_output_channels) {
    if (p_distrho_shared_memory->get_is_host()) {
        buffer = p_distrho_shared_memory->create_buffer<AudioBuffer>("AudioBuffer");
        buffer->num_input_channels = p_number_of_input_channels;
        buffer->num_output_channels = p_number_of_output_channels;

        // TODO: remove these variables?
        num_input_channels = p_number_of_input_channels;
        num_output_channels = p_number_of_output_channels;

    } else {
        buffer = p_distrho_shared_memory->create_buffer<AudioBuffer>("AudioBuffer");

        if (!buffer) {
            num_input_channels = p_number_of_input_channels;
            num_output_channels = p_number_of_output_channels;
        } else {
            num_input_channels = buffer->num_input_channels;
            num_output_channels = buffer->num_output_channels;
        }
    }
}

void DistrhoSharedMemoryAudio::write_input_channel(const float **p_buffer, int p_frames, int offset) {
    for (int channel = 0; channel < num_input_channels; channel++) {
        for (int frame = 0; frame < p_frames; frame++) {
            buffer->input[channel * BUFFER_SIZE + (buffer->input_write_index + frame) % BUFFER_SIZE] =
                p_buffer[channel][offset + frame];
        }
    }
}

void DistrhoSharedMemoryAudio::read_input_channel(float **p_buffer, int p_frames, int offset) {
    for (int channel = 0; channel < num_input_channels; channel++) {
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[channel][offset + frame] =
                buffer->input[channel * BUFFER_SIZE + (buffer->input_read_index + frame) % BUFFER_SIZE];
        }
    }
}

void DistrhoSharedMemoryAudio::write_output_channel(float **p_buffer, int p_frames, int offset) {
    for (int channel = 0; channel < num_output_channels; channel++) {
        for (int frame = 0; frame < p_frames; frame++) {
            buffer->output[channel * BUFFER_SIZE + (buffer->output_write_index + frame) % BUFFER_SIZE] =
                p_buffer[channel][offset + frame];
        }
    }
}

void DistrhoSharedMemoryAudio::read_output_channel(float **p_buffer, int p_frames, int offset) {
    for (int channel = 0; channel < num_output_channels; channel++) {
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[channel][offset + frame] =
                buffer->output[channel * BUFFER_SIZE + (buffer->output_read_index + frame) % BUFFER_SIZE];
        }
    }
}

void DistrhoSharedMemoryAudio::advance_input_write_index(int p_frames) {
    buffer->input_write_index = (buffer->input_write_index + p_frames) % BUFFER_SIZE;
}

void DistrhoSharedMemoryAudio::advance_input_read_index(int p_frames) {
    buffer->input_read_index = (buffer->input_read_index + p_frames) % BUFFER_SIZE;
}

void DistrhoSharedMemoryAudio::advance_output_write_index(int p_frames) {
    buffer->output_write_index = (buffer->output_write_index + p_frames) % BUFFER_SIZE;
}

void DistrhoSharedMemoryAudio::advance_output_read_index(int p_frames) {
    buffer->output_read_index = (buffer->output_read_index + p_frames) % BUFFER_SIZE;
}

int DistrhoSharedMemoryAudio::get_num_input_channels() {
    return buffer->num_input_channels;
}

int DistrhoSharedMemoryAudio::get_num_output_channels() {
    return buffer->num_output_channels;
}

void DistrhoSharedMemoryAudio::write_input_midi(const MidiEvent *p_midi_events, int p_midi_event_count) {
    for (int i = 0; i < p_midi_event_count; i++) {
        buffer->midi_input[(i + buffer->midi_input_write_index) % MIDI_BUFFER_SIZE] = p_midi_events[i];
    }
    buffer->midi_input_event_count = p_midi_event_count;
    buffer->midi_input_write_index = (buffer->midi_input_write_index + p_midi_event_count) % MIDI_BUFFER_SIZE;
}

int DistrhoSharedMemoryAudio::read_input_midi(MidiEvent *p_midi_events) {
    for (int i = 0; i < buffer->midi_input_event_count; i++) {
        p_midi_events[i] = buffer->midi_input[(i + buffer->midi_input_read_index) % MIDI_BUFFER_SIZE];
    }
    buffer->midi_input_read_index = (buffer->midi_input_read_index + buffer->midi_input_event_count) % MIDI_BUFFER_SIZE;

    return buffer->midi_input_event_count;
}

void DistrhoSharedMemoryAudio::write_output_midi(const MidiEvent *p_midi_events, int p_midi_event_count) {
    for (int i = 0; i < p_midi_event_count; i++) {
        buffer->midi_output[(i + buffer->midi_output_write_index) % MIDI_BUFFER_SIZE] = p_midi_events[i];
    }
    buffer->midi_output_event_count = p_midi_event_count;
    buffer->midi_output_write_index = (buffer->midi_output_write_index + p_midi_event_count) % MIDI_BUFFER_SIZE;
}

int DistrhoSharedMemoryAudio::read_output_midi(MidiEvent *p_midi_events) {
    for (int i = 0; i < buffer->midi_output_event_count; i++) {
        p_midi_events[i] = buffer->midi_output[(i + buffer->midi_output_read_index) % MIDI_BUFFER_SIZE];
    }
    buffer->midi_output_read_index =
        (buffer->midi_output_read_index + buffer->midi_output_event_count) % MIDI_BUFFER_SIZE;

    return buffer->midi_output_event_count;
}

int DistrhoSharedMemoryAudio::get_memory_size() {
    return SIZE_SHARED_MEMORY * 2;
}
