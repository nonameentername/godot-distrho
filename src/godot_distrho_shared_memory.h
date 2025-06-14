#ifndef GODOT_DISTRHO_SHARED_MEMORY_H
#define GODOT_DISTRHO_SHARED_MEMORY_H

#include <memory>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace godot {

const int BUFFER_SIZE = 1024;
const int MAX_CHANNELS = 16;
const int SIZE_SHARED_MEMORY = 131072; //pow(2, 17);


enum {
    HOST_TURN,
    PLUGIN_TURN
} typedef SYNC_FLAG;

struct AudioBuffer {
    int num_input_channels;
    int num_output_channels;
    int write_index;
    int read_index;

    float input[MAX_CHANNELS * BUFFER_SIZE];
    float output[MAX_CHANNELS * BUFFER_SIZE];
    int sync_flag = HOST_TURN;
    boost::interprocess::interprocess_mutex mutex;
};

class GodotDistrhoSharedMemory {

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
    GodotDistrhoSharedMemory();
    ~GodotDistrhoSharedMemory();

    void initialize(int p_number_of_input_channels = 0, int p_number_of_output_channels = 0, std::string p_shared_memory_name = "");

	void write_input_channel(const float **p_buffer, int p_frames);
	void read_input_channel(float **p_buffer, int p_frames);

	void write_output_channel(float **p_buffer, int p_frames);
	void read_output_channel(float **p_buffer, int p_frames);

	void advance_read_index(int p_frames);
	void advance_write_index(int p_frames);

    void set_sync_flag(SYNC_FLAG p_sync_flag);
    SYNC_FLAG get_sync_flag();

    std::string get_shared_memory_name();
};

}

#endif
