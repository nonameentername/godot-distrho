#ifndef DISTRHO_SHARED_MEMORY_H
#define DISTRHO_SHARED_MEMORY_H

#include <godot_cpp/classes/node.hpp>
#include "godot_distrho_shared_memory.h"


namespace godot {
    
class DistrhoSharedMemory : public Object {
    GDCLASS(DistrhoSharedMemory, Object);

private:
    GodotDistrhoSharedMemory *godot_distrho_shared_memory;

protected:

public:
    DistrhoSharedMemory(String p_shared_memory_name = "");
    ~DistrhoSharedMemory();

	void write_input_channel(float **p_buffer, int p_frames);
	void read_input_channel(float **p_buffer, int p_frames);

	void write_output_channel(float **p_buffer, int p_frames);
	void read_output_channel(float **p_buffer, int p_frames);

	void advance_read_index(int p_frames);
	void advance_write_index(int p_frames);

    void set_sync_flag(SYNC_FLAG p_sync_flag);
    SYNC_FLAG get_sync_flag();

    static void _bind_methods();
};
}

#endif
