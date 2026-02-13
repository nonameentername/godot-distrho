#ifndef DISTRHO_SHARED_MEMORY_AUDIO_H
#define DISTRHO_SHARED_MEMORY_AUDIO_H

#include <DistrhoDetails.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>
#include <random>

#include "distrho_shared_memory.h"

#ifdef _WIN32
#include <boost/interprocess/managed_windows_shared_memory.hpp>
#endif

namespace godot {

const int BUFFER_SIZE = 4096;
const int BUFFER_FRAME_SIZE = 1024;
const int MAX_CHANNELS = 16;
const int SIZE_SHARED_MEMORY = 524288; // pow(2, 19);
const int MIDI_BUFFER_SIZE = 2048;

struct AudioBuffer {
    int num_input_channels = 0;
    int num_output_channels = 0;

    int input_write_index = 0;
    int input_read_index = 0;

    int output_write_index = 0;
    int output_read_index = 0;

    float input[MAX_CHANNELS * BUFFER_SIZE];
    float output[MAX_CHANNELS * BUFFER_SIZE];

    int midi_input_write_index = 0;
    int midi_input_read_index = 0;

    int midi_output_write_index = 0;
    int midi_output_read_index = 0;

    int midi_input_event_count = 0;
    int midi_output_event_count = 0;

    MidiEvent midi_input[MIDI_BUFFER_SIZE];
    MidiEvent midi_output[MIDI_BUFFER_SIZE];

    bool ready = false;
    boost::interprocess::interprocess_mutex mutex;
    boost::interprocess::interprocess_condition input_condition;
    boost::interprocess::interprocess_condition output_condition;
};

class DistrhoSharedMemoryAudio {

private:
#ifdef _WIN32
    std::unique_ptr<boost::interprocess::managed_windows_shared_memory> shared_memory;
#else
    std::unique_ptr<boost::interprocess::managed_shared_memory> shared_memory;
#endif

public:
    AudioBuffer *buffer;

    int num_input_channels;
    int num_output_channels;

protected:
public:
    DistrhoSharedMemoryAudio();
    ~DistrhoSharedMemoryAudio();

    void initialize(DistrhoSharedMemory *p_distrho_shared_memory, int p_number_of_input_channels = 0, int p_number_of_output_channels = 0);

    void write_input_channel(const float **p_buffer, int p_frames, int offset = 0);
    void read_input_channel(float **p_buffer, int p_frames, int offset = 0);

    void write_output_channel(float **p_buffer, int p_frames, int offset = 0);
    void read_output_channel(float **p_buffer, int p_frames, int offset = 0);

    void advance_input_read_index(int p_frames);
    void advance_input_write_index(int p_frames);

    void advance_output_read_index(int p_frames);
    void advance_output_write_index(int p_frames);

    int get_num_input_channels();
    int get_num_output_channels();

    void write_input_midi(const MidiEvent *p_midi_events, int p_midi_event_count);
    int read_input_midi(MidiEvent *p_midi_events);

    void write_output_midi(const MidiEvent *p_midi_events, int p_midi_event_count);
    int read_output_midi(MidiEvent *p_midi_events);

    int get_memory_size();
};

} // namespace godot

#endif
