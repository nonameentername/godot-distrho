#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "distrho_shared_memory.h"

using namespace godot;

namespace bip = boost::interprocess;


DistrhoSharedMemory::DistrhoSharedMemory() {
}

DistrhoSharedMemory::~DistrhoSharedMemory() {
    if (is_host) {
        bip::shared_memory_object::remove(shared_memory_name.c_str());
    }
}

void DistrhoSharedMemory::initialize(int p_number_of_input_channels, int p_number_of_output_channels, std::string p_shared_memory_name) {
    if (p_shared_memory_name.length() == 0) {
		is_host = true;
		boost::uuids::uuid uuid = generator();
		shared_memory_name = boost::uuids::to_string(uuid);

#if !DISTRHO_PLUGIN_ENABLE_SUBPROCESS
        printf("shared_memory_name %s", shared_memory_name.c_str());
#endif

    	bip::shared_memory_object::remove(shared_memory_name.c_str());
		shared_memory = std::make_unique<boost::interprocess::managed_shared_memory>(
				bip::create_only, shared_memory_name.c_str(), SIZE_SHARED_MEMORY * 2);

    	buffer = shared_memory->construct<AudioBuffer>("AudioBuffer")();
		buffer->num_input_channels = p_number_of_input_channels;
		buffer->num_output_channels = p_number_of_output_channels;

        //TODO: remove these variables?
        num_input_channels = p_number_of_input_channels;
        num_output_channels = p_number_of_output_channels;

    } else {
		is_host = false;
    	shared_memory_name = p_shared_memory_name;

		shared_memory = std::make_unique<bip::managed_shared_memory>(
				bip::open_only, shared_memory_name.c_str());
		buffer = shared_memory->find<AudioBuffer>("AudioBuffer").first;

		if (!buffer) {
			num_input_channels = p_number_of_input_channels;
			num_output_channels = p_number_of_output_channels;
		} else {
			num_input_channels = buffer->num_input_channels;
			num_output_channels = buffer->num_output_channels;
		}
	}
}

void DistrhoSharedMemory::write_input_channel(const float **p_buffer, int p_frames, int offset) {
    for (int channel = 0; channel < num_input_channels; channel++) {
        for (int frame = 0; frame < p_frames; frame++) {
            buffer->input[channel * BUFFER_SIZE + (buffer->input_write_index + frame) % BUFFER_SIZE] = p_buffer[channel][offset + frame ];
        }
    }
}

void DistrhoSharedMemory::read_input_channel(float **p_buffer, int p_frames, int offset) {
    for (int channel = 0; channel < num_input_channels; channel++) {
        for (int frame = 0; frame < p_frames; frame++) {
 			p_buffer[channel][offset + frame] = buffer->input[channel * BUFFER_SIZE + (buffer->input_read_index + frame) % BUFFER_SIZE];
        }
    }
}

void DistrhoSharedMemory::write_output_channel(float **p_buffer, int p_frames, int offset) {
    for (int channel = 0; channel < num_output_channels; channel++) {
        for (int frame = 0; frame < p_frames; frame++) {
            buffer->output[channel * BUFFER_SIZE + (buffer->output_write_index + frame) % BUFFER_SIZE] = p_buffer[channel][offset + frame];
        }
    }
}

void DistrhoSharedMemory::read_output_channel(float **p_buffer, int p_frames, int offset) {
    for (int channel = 0; channel < num_output_channels; channel++) {
        for (int frame = 0; frame < p_frames; frame++) {
 			p_buffer[channel][offset + frame] = buffer->output[channel * BUFFER_SIZE + (buffer->output_read_index + frame) % BUFFER_SIZE];
        }
    }
}

void DistrhoSharedMemory::advance_input_write_index(int p_frames) {
  	buffer->input_write_index = (buffer->input_write_index + p_frames) % BUFFER_SIZE;
}

void DistrhoSharedMemory::advance_input_read_index(int p_frames) {
  	buffer->input_read_index = (buffer->input_read_index + p_frames) % BUFFER_SIZE;
}

void DistrhoSharedMemory::advance_output_write_index(int p_frames) {
  	buffer->output_write_index = (buffer->output_write_index + p_frames) % BUFFER_SIZE;
}

void DistrhoSharedMemory::advance_output_read_index(int p_frames) {
  	buffer->output_read_index = (buffer->output_read_index + p_frames) % BUFFER_SIZE;
}

int DistrhoSharedMemory::get_num_input_channels() {
    return buffer->num_input_channels;
}

int DistrhoSharedMemory::get_num_output_channels() {
    return buffer->num_output_channels;
}

std::string DistrhoSharedMemory::get_shared_memory_name() {
	return shared_memory_name;
}
