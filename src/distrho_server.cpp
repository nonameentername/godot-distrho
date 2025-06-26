#include <boost/interprocess/sync/scoped_lock.hpp>
#include "distrho_server.h"
#include "distrho_config.h"
#include "distrho_plugin_instance.h"
#include "distrho_schema.capnp.h"
#include "distrho_shared_memory_audio.h"
#include "distrho_circular_buffer.h"
#include "distrho_shared_memory_rpc.h"
#include "godot_cpp/classes/audio_server.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/classes/os.hpp"
#include "distrho_shared_memory_audio.h"
#include "version_generated.gen.h"
#include <capnp/serialize.h>
#include <cstdlib>
#include <godot_cpp/core/mutex_lock.hpp>
#include <godot_cpp/classes/mutex.hpp>
#include "godot_cpp/classes/engine.hpp"
#include <kj/string.h>
#include <unistd.h>


using namespace godot;

DistrhoServer *DistrhoServer::singleton = NULL;

DistrhoServer::DistrhoServer() {
    initialized = false;
	active = false;
    exit_thread = false;
    distrho_config = memnew(DistrhoConfig);
    distrho_plugin = memnew(DistrhoPluginInstance);
	distrho_shared_memory_audio = new DistrhoSharedMemoryAudio();
    const char* audio_shared_memory = std::getenv("DISTRHO_AUDIO_SHARED_MEMORY");
    if (audio_shared_memory == NULL) {
        audio_shared_memory = "";
    }
	distrho_shared_memory_audio->initialize(0, 0, audio_shared_memory);
    const char* rpc_shared_memory = std::getenv("DISTRHO_RPC_SHARED_MEMORY");
    if (rpc_shared_memory == NULL) {
        rpc_shared_memory = "";
    }
    distrho_shared_memory_rpc = new DistrhoSharedMemoryRPC();
	distrho_shared_memory_rpc->initialize(rpc_shared_memory);
    singleton = this;

    //TODO: use the correct number of channels instad of num_channels (16)
	for (int i = 0; i < num_channels; ++i) {
		input_buffer[i] = input_data[i];
		output_buffer[i] = output_data[i];
    }

    input_channels.resize(distrho_shared_memory_audio->get_num_input_channels());
    output_channels.resize(distrho_shared_memory_audio->get_num_output_channels());

    for (int channel = 0; channel < distrho_shared_memory_audio->get_num_input_channels(); channel++) {
        input_channels.write[channel] = new DistrhoCircularBuffer();;
    }

    for (int channel = 0; channel < distrho_shared_memory_audio->get_num_output_channels(); channel++) {
        output_channels.write[channel] = new DistrhoCircularBuffer();
    }

    call_deferred("initialize");
}

DistrhoServer::~DistrhoServer() {
	//delete distrho_shared_memory_audio;
    //delete distrho_shared_memory_rpc;

    for (int channel = 0; channel < input_channels.size(); channel++) {
        delete input_channels[channel];
    }

    for (int channel = 0; channel < output_channels.size(); channel++) {
        delete output_channels[channel];
    }

    singleton = NULL;
}

DistrhoServer *DistrhoServer::get_singleton() {
    return singleton;
}

void DistrhoServer::initialize() {
    initialized = true;
}

void DistrhoServer::audio_thread_func() {
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> shared_memory_lock(distrho_shared_memory_audio->buffer->mutex);
    distrho_shared_memory_audio->buffer->godot_ready = true;
    shared_memory_lock.unlock();

	active = true;

    while (!exit_thread) {
        // Acquire the mutex
        boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> shared_memory_lock(distrho_shared_memory_audio->buffer->mutex);

        if (initialized) {
            AudioServer::get_singleton()->process_external(BUFFER_FRAME_SIZE);
        }

        // Wait for input data (release the mutex while waiting)
        distrho_shared_memory_audio->buffer->input_condition.wait(shared_memory_lock);

		distrho_shared_memory_audio->read_input_channel(input_buffer, BUFFER_FRAME_SIZE);
		distrho_shared_memory_audio->advance_input_read_index(BUFFER_FRAME_SIZE);

        for (int channel = 0; channel < distrho_shared_memory_audio->get_num_input_channels(); channel++) {
            input_channels.write[channel]->write_channel(input_buffer[channel], BUFFER_FRAME_SIZE);
        }

        for (int channel = 0; channel < distrho_shared_memory_audio->get_num_output_channels(); channel++) {
            output_channels[channel]->read_channel(output_buffer[channel], BUFFER_FRAME_SIZE);
        }

		distrho_shared_memory_audio->write_output_channel(output_buffer, BUFFER_FRAME_SIZE);
		distrho_shared_memory_audio->advance_output_write_index(BUFFER_FRAME_SIZE);

        // Signal the output condition (wake up the host)
        distrho_shared_memory_audio->buffer->output_condition.notify_one();

        // The mutex is automatically released when the scoped_lock goes out of scope
    }
	delete distrho_shared_memory_audio;
}

void DistrhoServer::rpc_thread_func() {
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> shared_memory_lock(distrho_shared_memory_rpc->buffer->mutex);
    distrho_shared_memory_rpc->buffer->godot_ready = true;
    shared_memory_lock.unlock();

    while (!exit_thread) {
        // Acquire the mutex
        boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> shared_memory_lock(distrho_shared_memory_rpc->buffer->mutex);

        // Wait for input data (release the mutex while waiting)
        distrho_shared_memory_rpc->buffer->input_condition.wait(shared_memory_lock);

        capnp::FlatArrayMessageReader reader = distrho_shared_memory_rpc->read_request();
        capnp::MallocMessageBuilder builder;

        switch (distrho_shared_memory_rpc->buffer->request_id) {
            case PluginInterface::GetLabelParams::_capnpPrivate::typeId: {
                PluginInterface::GetLabelParams::Reader request = reader.getRoot<PluginInterface::GetLabelParams>();

                String label = get_label();

                PluginInterface::GetLabelResults::Builder response = builder.initRoot<PluginInterface::GetLabelResults>();
                response.setLabel(std::string(label.ascii()));
                break;
            }
            case PluginInterface::InitParameterParams::_capnpPrivate::typeId: {
                PluginInterface::InitParameterParams::Reader request = reader.getRoot<PluginInterface::InitParameterParams>();
                request.getIndex();

                //TODO: handle the request

                PluginInterface::InitParameterParams::Builder response = builder.initRoot<PluginInterface::InitParameterParams>();
                response.setIndex(32);
                break;
            }
        }

        //write the response
		distrho_shared_memory_rpc->write_reponse(&builder);

        // Signal the output condition (wake up the host)
        distrho_shared_memory_rpc->buffer->output_condition.notify_one();

        // The mutex is automatically released when the scoped_lock goes out of scope
    }
	delete distrho_shared_memory_rpc;
}

int DistrhoServer::process_sample(AudioFrame *p_buffer, float p_rate, int p_frames) {
    lock_audio();

    if (distrho_shared_memory_audio->get_num_input_channels() == 0) {
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].left = 0;
            p_buffer[frame].right = 0;
        }
    } else if (distrho_shared_memory_audio->get_num_input_channels() == 1) {
        input_channels[0]->read_channel(temp_buffer, p_frames);
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].left = temp_buffer[frame];
            p_buffer[frame].right = temp_buffer[frame];
        }
    } else {
        input_channels[0]->read_channel(temp_buffer, p_frames);
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].left = temp_buffer[frame];
        }

        input_channels[1]->read_channel(temp_buffer, p_frames);
        for (int frame = 0; frame < p_frames; frame++) {
            p_buffer[frame].right = temp_buffer[frame];
        }
    }

    unlock_audio();

    //semaphore->post();

    return p_frames;
}

void DistrhoServer::set_channel_sample(AudioFrame *p_buffer, float p_rate, int p_frames, int left, int right) {
	bool has_left_channel = left >= 0 && left < output_channels.size();
	bool has_right_channel = right >= 0 && right < output_channels.size();

	if (!has_left_channel && !has_right_channel && !active) {
		return;
	}

	lock_audio();

	if (has_left_channel) {
        for (int frame = 0; frame < p_frames; frame++) {
 			temp_buffer[frame] = p_buffer[frame].left;
        }
        output_channels.write[left]->write_channel(temp_buffer, p_frames);
	}

	if (has_right_channel) {
        for (int frame = 0; frame < p_frames; frame++) {
 			temp_buffer[frame] = p_buffer[frame].right;
        }
        output_channels.write[right]->write_channel(temp_buffer, p_frames);
	}

	unlock_audio();
}

int DistrhoServer::get_channel_sample(AudioFrame *p_buffer, float p_rate, int p_frames, int left, int right) {
	bool has_left_channel = left >= 0 && left < input_channels.size();
	bool has_right_channel = right >= 0 && right < input_channels.size();

	lock_audio();
	if (has_left_channel && active) {
        input_channels[left]->read_channel(temp_buffer, p_frames);
		for (int frame = 0; frame < p_frames; frame++) {
			p_buffer[frame].left = temp_buffer[frame];
		}
	} else {
		for (int frame = 0; frame < p_frames; frame++) {
			p_buffer[frame].left = 0;
		}
	}
	if (has_right_channel && active) {
        input_channels[right]->read_channel(temp_buffer, p_frames);
		for (int frame = 0; frame < p_frames; frame++) {
			p_buffer[frame].right = temp_buffer[frame];
		}
	} else {
		for (int frame = 0; frame < p_frames; frame++) {
			p_buffer[frame].right = 0;
		}
	}
	unlock_audio();

	return p_frames;
}

Error DistrhoServer::start() {
    if(!godot::Engine::get_singleton()->is_editor_hint()) {
        audio_thread.instantiate();
        audio_mutex.instantiate();
        audio_thread->start(callable_mp(this, &DistrhoServer::audio_thread_func), Thread::PRIORITY_HIGH);

        rpc_thread.instantiate();
        rpc_thread->start(callable_mp(this, &DistrhoServer::rpc_thread_func), Thread::PRIORITY_NORMAL);
    }
    return OK;
}

void DistrhoServer::lock_audio() {
    if (audio_thread.is_null() || audio_mutex.is_null()) {
        return;
    }
    audio_mutex->lock();
}

void DistrhoServer::unlock_audio() {
    if (audio_thread.is_null() || audio_mutex.is_null()) {
        return;
    }
    audio_mutex->unlock();
}

void DistrhoServer::finish() {
    if(!godot::Engine::get_singleton()->is_editor_hint()) {
        exit_thread = true;
        audio_thread->wait_to_finish();
        rpc_thread->wait_to_finish();
    }
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

String DistrhoServer::get_label() {
    //TODO: return a value set by the user.
    return "godot-distrho";
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
