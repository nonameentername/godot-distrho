#include <boost/interprocess/sync/scoped_lock.hpp>
#include "distrho_server.h"
#include "distrho_config.h"
#include "distrho_plugin_instance.h"
#include "distrho_shared_memory.h"
#include "distrho_circular_buffer.h"
#include "godot_cpp/classes/audio_server.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/classes/os.hpp"
#include "distrho_shared_memory.h"
#include "version_generated.gen.h"
#include <godot_cpp/core/mutex_lock.hpp>
#include <godot_cpp/classes/mutex.hpp>

using namespace godot;

DistrhoServer *DistrhoServer::singleton = NULL;

DistrhoServer::DistrhoServer() {
    initialized = false;
	active = false;
    exit_thread = false;
    distrho_config = memnew(DistrhoConfig);
    distrho_plugin = memnew(DistrhoPluginInstance);
	distrho_shared_memory = new DistrhoSharedMemory();
	distrho_shared_memory->initialize(0, 0, "godot-distrho");
    singleton = this;

    //TODO: use the correct number of channels instad of num_channels (16)
	for (int i = 0; i < num_channels; ++i) {
		input_buffer[i] = input_data[i];
		output_buffer[i] = output_data[i];
    }

    input_channels.resize(distrho_shared_memory->get_num_input_channels());
    output_channels.resize(distrho_shared_memory->get_num_output_channels());

    for (int channel = 0; channel < distrho_shared_memory->get_num_input_channels(); channel++) {
        input_channels.write[channel] = distrhoCreateCircularBuffer(CIRCULAR_BUFFER_SIZE, sizeof(float));
    }

    for (int channel = 0; channel < distrho_shared_memory->get_num_output_channels(); channel++) {
        output_channels.write[channel] = distrhoCreateCircularBuffer(CIRCULAR_BUFFER_SIZE, sizeof(float));
    }

    call_deferred("initialize");
}

DistrhoServer::~DistrhoServer() {
	delete distrho_shared_memory;

    for (int channel = 0; channel < input_channels.size(); channel++) {
        distrhoDestroyCircularBuffer(input_channels[channel]);
    }

    for (int channel = 0; channel < output_channels.size(); channel++) {
        distrhoDestroyCircularBuffer(output_channels[channel]);
    }

    singleton = NULL;
}

DistrhoServer *DistrhoServer::get_singleton() {
    return singleton;
}

void DistrhoServer::initialize() {
    initialized = true;
}

void DistrhoServer::thread_func() {
    // Set the "ready" flag
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> shared_memory_lock(distrho_shared_memory->buffer->mutex);
    distrho_shared_memory->buffer->godot_ready = true;
    shared_memory_lock.unlock();

	active = true;

    while (!exit_thread) {
        // Acquire the mutex
        boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> shared_memory_lock(distrho_shared_memory->buffer->mutex);

        if (initialized) {
            AudioServer::get_singleton()->process_external(BUFFER_FRAME_SIZE);
        }

        //semaphore->wait();

        //lock();

        // Log before read
        //UtilityFunctions::print("Plugin: Before Read - input_read_index: ", distrho_shared_memory->buffer->input_read_index);
        //UtilityFunctions::print("Plugin: Before Read - input_write_index: ", distrho_shared_memory->buffer->input_write_index);

        // Wait for input data (release the mutex while waiting)
        distrho_shared_memory->buffer->input_condition.wait(shared_memory_lock);

		distrho_shared_memory->read_input_channel(input_buffer, BUFFER_FRAME_SIZE);
		distrho_shared_memory->advance_input_read_index(BUFFER_FRAME_SIZE);

          // Log after read
        //UtilityFunctions::print("Plugin: After Read - input_read_index: ", distrho_shared_memory->buffer->input_read_index);
        //UtilityFunctions::print("Plugin: After Read - input_write_index: ", distrho_shared_memory->buffer->input_write_index);

        //write to internal ring buffer

		//lock();

        for (int channel = 0; channel < distrho_shared_memory->get_num_input_channels(); channel++) {
            distrhoWriteCircularBuffer(input_channels.write[channel], input_buffer[channel], BUFFER_FRAME_SIZE);
        }

        //unlock();

        //semaphore->wait();

        //lock();

        for (int channel = 0; channel < distrho_shared_memory->get_num_output_channels(); channel++) {
            distrhoReadCircularBuffer(output_channels[channel], output_buffer[channel], BUFFER_FRAME_SIZE);

            //for (int frame = 0; frame < BUFFER_FRAME_SIZE; frame++) {
            //    output_buffer[channel][frame] = input_buffer[channel][frame];
            //}
        }

		//unlock();

		// Log before write
        //UtilityFunctions::print("Plugin: Before Write - output_read_index: ", distrho_shared_memory->buffer->output_read_index);
        //UtilityFunctions::print("Plugin: Before Write - output_write_index: ", distrho_shared_memory->buffer->output_write_index);


		distrho_shared_memory->write_output_channel(output_buffer, BUFFER_FRAME_SIZE);
		distrho_shared_memory->advance_output_write_index(BUFFER_FRAME_SIZE);

        // Log after write
        //UtilityFunctions::print("Plugin: After Write - output_read_index: ", distrho_shared_memory->buffer->output_read_index);
        //UtilityFunctions::print("Plugin: After Write - output_write_index: ", distrho_shared_memory->buffer->output_write_index);

        // Signal the output condition (wake up the host)
        distrho_shared_memory->buffer->output_condition.notify_one();

        // The mutex is automatically released when the scoped_lock goes out of scope

        //unlock();

        //semaphore->wait();
    }
	delete distrho_shared_memory;
}

int DistrhoServer::process_sample(AudioFrame *p_buffer, float p_rate, int p_frames) {
    lock();

    if (distrho_shared_memory->get_num_input_channels() == 0) {
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].left = 0;
            p_buffer[frame].right = 0;
        }
    } else if (distrho_shared_memory->get_num_input_channels() == 1) {
        distrhoReadCircularBuffer(input_channels[0], temp_buffer, p_frames);
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].left = temp_buffer[frame];
            p_buffer[frame].right = temp_buffer[frame];
        }
    } else {
        distrhoReadCircularBuffer(input_channels[0], temp_buffer, p_frames);
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].left = temp_buffer[frame];
        }

        distrhoReadCircularBuffer(input_channels[1], temp_buffer, p_frames);
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].right = temp_buffer[frame];
        }
    }

    unlock();

    //semaphore->post();

    return p_frames;
}

void DistrhoServer::set_channel_sample(AudioFrame *p_buffer, float p_rate, int p_frames, int left, int right) {
	bool has_left_channel = left >= 0 && left < output_channels.size();
	bool has_right_channel = right >= 0 && right < output_channels.size();

	if (!has_left_channel && !has_right_channel && !active) {
		return;
	}

	lock();

	if (has_left_channel) {
        for (int frame = 0; frame < p_frames; frame++) {
 			temp_buffer[frame] = p_buffer[frame].left;
        }
		distrhoWriteCircularBuffer(output_channels.write[left], temp_buffer, p_frames);
	}

	if (has_right_channel) {
        for (int frame = 0; frame < p_frames; frame++) {
 			temp_buffer[frame] = p_buffer[frame].right;
        }
		distrhoWriteCircularBuffer(output_channels.write[right], temp_buffer, p_frames);
	}

	unlock();
}

int DistrhoServer::get_channel_sample(AudioFrame *p_buffer, float p_rate, int p_frames, int left, int right) {
	bool has_left_channel = left >= 0 && left < input_channels.size();
	bool has_right_channel = right >= 0 && right < input_channels.size();

	lock();
	if (has_left_channel && active) {
		distrhoReadCircularBuffer(input_channels[left], temp_buffer, p_frames);
		for (int frame = 0; frame < p_frames; frame++) {
			p_buffer[frame].left = temp_buffer[frame];
		}
	} else {
		for (int frame = 0; frame < p_frames; frame++) {
			p_buffer[frame].left = 0;
		}
	}
	if (has_right_channel && active) {
		distrhoReadCircularBuffer(input_channels[right], temp_buffer, p_frames);
		for (int frame = 0; frame < p_frames; frame++) {
			p_buffer[frame].right = temp_buffer[frame];
		}
	} else {
		for (int frame = 0; frame < p_frames; frame++) {
			p_buffer[frame].right = 0;
		}
	}
	unlock();

	return p_frames;
}

Error DistrhoServer::start() {
    thread.instantiate();
    mutex.instantiate();
    semaphore.instantiate();

    thread->start(callable_mp(this, &DistrhoServer::thread_func), Thread::PRIORITY_HIGH);
    return OK;
}

void DistrhoServer::lock() {
    if (thread.is_null() || mutex.is_null()) {
        return;
    }
    mutex->lock();
}

void DistrhoServer::unlock() {
    if (thread.is_null() || mutex.is_null()) {
        return;
    }
    mutex->unlock();
}

void DistrhoServer::finish() {
    exit_thread = true;
    thread->wait_to_finish();
}

DistrhoConfig *DistrhoServer::get_config() {
    return distrho_config;
}

String DistrhoServer::get_version() {
    return GODOT_DISTRHO_VERSION;
}

String DistrhoServer::get_build() {
    return GODOT_DISTRHO_BUILD;
}

void DistrhoServer::set_distrho_launcher(DistrhoLauncher *p_distrho_launcher) {
	distrho_launcher = p_distrho_launcher;
}

DistrhoLauncher *DistrhoServer::get_distrho_launcher() {
	return distrho_launcher;
}

void DistrhoServer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize"), &DistrhoServer::initialize);

    ClassDB::bind_method(D_METHOD("get_config"), &DistrhoServer::get_config);

    ClassDB::bind_method(D_METHOD("get_version"), &DistrhoServer::get_version);
    ClassDB::bind_method(D_METHOD("get_build"), &DistrhoServer::get_build);
}
