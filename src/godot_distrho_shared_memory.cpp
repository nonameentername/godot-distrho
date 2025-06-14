#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "godot_distrho_shared_memory.h"

using namespace godot;

namespace bip = boost::interprocess;


GodotDistrhoSharedMemory::GodotDistrhoSharedMemory() {
}

GodotDistrhoSharedMemory::~GodotDistrhoSharedMemory() {
    if (is_host) {
        bip::shared_memory_object::remove(shared_memory_name.c_str());
    }
}

void GodotDistrhoSharedMemory::initialize(int p_number_of_input_channels, int p_number_of_output_channels, std::string p_shared_memory_name) {
    if (p_shared_memory_name.length() == 0) {
		is_host = true;
		boost::uuids::uuid uuid = generator();
		shared_memory_name = boost::uuids::to_string(uuid);
		shared_memory_name = "godot-distrho";

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

void GodotDistrhoSharedMemory::write_input_channel(const float **p_buffer, int p_frames) {
    for (int channel = 0; channel < num_input_channels; channel++) {
        for (int frame = 0; frame < BUFFER_SIZE; frame++) {
            buffer->input[channel * BUFFER_SIZE + (buffer->write_index + frame) % BUFFER_SIZE] = p_buffer[channel][frame];
        }
    }
}

void GodotDistrhoSharedMemory::read_input_channel(float **p_buffer, int p_frames) {
    for (int channel = 0; channel < num_input_channels; channel++) {
        for (int frame = 0; frame < BUFFER_SIZE; frame++) {
 			p_buffer[channel][frame] = buffer->input[channel * BUFFER_SIZE + (buffer->write_index + frame) % BUFFER_SIZE];
        }
    }
}

void GodotDistrhoSharedMemory::write_output_channel(float **p_buffer, int p_frames) {
    for (int channel = 0; channel < num_output_channels; channel++) {
        for (int frame = 0; frame < BUFFER_SIZE; frame++) {
            buffer->output[channel * BUFFER_SIZE + (buffer->write_index + frame) % BUFFER_SIZE] = p_buffer[channel][frame];
        }
    }
}

void GodotDistrhoSharedMemory::read_output_channel(float **p_buffer, int p_frames) {
    for (int channel = 0; channel < num_output_channels; channel++) {
        for (int frame = 0; frame < BUFFER_SIZE; frame++) {
 			p_buffer[channel][frame] = buffer->output[channel * BUFFER_SIZE + (buffer->write_index + frame) % BUFFER_SIZE];
        }
    }
}

void GodotDistrhoSharedMemory::advance_write_index(int p_frames) {
  	buffer->write_index = (buffer->write_index + p_frames) % BUFFER_SIZE;
}

void GodotDistrhoSharedMemory::advance_read_index(int p_frames) {
  	buffer->read_index = (buffer->read_index + p_frames) % BUFFER_SIZE;
}


void GodotDistrhoSharedMemory::set_sync_flag(SYNC_FLAG p_sync_flag) {
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(buffer->mutex);
    buffer->sync_flag = p_sync_flag;
}

SYNC_FLAG GodotDistrhoSharedMemory::get_sync_flag() {
    if (!buffer) {
        return SYNC_FLAG::HOST_TURN;
    }
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(buffer->mutex);
    return (SYNC_FLAG)buffer->sync_flag;
}

std::string GodotDistrhoSharedMemory::get_shared_memory_name() {
	return shared_memory_name;
}
