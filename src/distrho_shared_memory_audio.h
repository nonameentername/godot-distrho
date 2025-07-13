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
    boost::uuids::basic_random_generator<std::mt19937_64> generator;
    std::unique_ptr<boost::interprocess::managed_shared_memory> shared_memory;

public:
    AudioBuffer *buffer;

    std::string shared_memory_name;
    int num_input_channels;
    int num_output_channels;
    bool is_host;

protected:
public:
    DistrhoSharedMemoryAudio();
    ~DistrhoSharedMemoryAudio();

    void initialize(int p_number_of_input_channels = 0, int p_number_of_output_channels = 0,
                    std::string p_shared_memory_name = "");

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

    bool get_is_host();

    std::string get_shared_memory_name();
};

} // namespace godot

#endif
