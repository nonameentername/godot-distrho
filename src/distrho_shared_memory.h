#ifndef DISTRHO_SHARED_MEMORY_H
#define DISTRHO_SHARED_MEMORY_H

#include <memory>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace godot {

const int BUFFER_SIZE = 2048;
const int BUFFER_FRAME_SIZE = 512;
const int MAX_CHANNELS = 16;
const int SIZE_SHARED_MEMORY = 262144; //pow(2, 18);


enum INPUT_SYNC {
    INPUT_WAIT,
    INPUT_READY
};

enum OUTPUT_SYNC {
    OUTPUT_WAIT,
    OUTPUT_READY
};

struct AudioBuffer {
    int num_input_channels;
    int num_output_channels;

    int input_write_index;
    int input_read_index;

    int output_write_index;
    int output_read_index;

    float input[MAX_CHANNELS * BUFFER_SIZE];
    float output[MAX_CHANNELS * BUFFER_SIZE];
    int input_sync = INPUT_WAIT;
    int output_sync = OUTPUT_WAIT;
    boost::interprocess::interprocess_mutex mutex;
};

class DistrhoSharedMemory {

private:
    boost::uuids::random_generator generator;
	std::unique_ptr<boost::interprocess::managed_shared_memory> shared_memory;
    std::string shared_memory_name;
	AudioBuffer* buffer;

	int num_input_channels;
	int num_output_channels;
	bool is_host;

protected:

public:
    DistrhoSharedMemory();
    ~DistrhoSharedMemory();

    void initialize(int p_number_of_input_channels = 0, int p_number_of_output_channels = 0, std::string p_shared_memory_name = "");

	void write_input_channel(const float **p_buffer, int p_frames);
	void read_input_channel(float **p_buffer, int p_frames);

	void write_output_channel(float **p_buffer, int p_frames);
	void read_output_channel(float **p_buffer, int p_frames);

	void advance_input_read_index(int p_frames);
	void advance_input_write_index(int p_frames);

	void advance_output_read_index(int p_frames);
	void advance_output_write_index(int p_frames);

    void set_input_flag(INPUT_SYNC p_sync_flag);
    INPUT_SYNC get_input_flag();

    void set_output_flag(OUTPUT_SYNC p_sync_flag);
    OUTPUT_SYNC get_output_flag();

    int get_num_input_channels();
    int get_num_output_channels();

    std::string get_shared_memory_name();
};

}

#endif
