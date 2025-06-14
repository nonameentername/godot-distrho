#include "distrho_shared_memory.h"
#include "godot_distrho_shared_memory.h"

using namespace godot;


DistrhoSharedMemory::DistrhoSharedMemory(String p_shared_memory_name) {
    godot_distrho_shared_memory = new GodotDistrhoSharedMemory();
    godot_distrho_shared_memory->initialize(0, 0, std::string(p_shared_memory_name.utf8()));
}

DistrhoSharedMemory::~DistrhoSharedMemory() {
    delete godot_distrho_shared_memory;
}

void DistrhoSharedMemory::write_input_channel(float **p_buffer, int p_frames) {
    godot_distrho_shared_memory->write_input_channel((const float **)p_buffer, p_frames);
}

void DistrhoSharedMemory::read_input_channel(float **p_buffer, int p_frames) {
    godot_distrho_shared_memory->read_input_channel(p_buffer, p_frames);
}

void DistrhoSharedMemory::write_output_channel(float **p_buffer, int p_frames) {
    godot_distrho_shared_memory->write_output_channel(p_buffer, p_frames);
}

void DistrhoSharedMemory::read_output_channel(float **p_buffer, int p_frames) {
    godot_distrho_shared_memory->read_output_channel(p_buffer, p_frames);
}

void DistrhoSharedMemory::advance_write_index(int p_frames) {
    godot_distrho_shared_memory->advance_write_index(p_frames);
}

void DistrhoSharedMemory::advance_read_index(int p_frames) {
    godot_distrho_shared_memory->advance_read_index(p_frames);
}

void DistrhoSharedMemory::set_sync_flag(SYNC_FLAG p_sync_flag) {
    godot_distrho_shared_memory->set_sync_flag(p_sync_flag);
}

SYNC_FLAG DistrhoSharedMemory::get_sync_flag() {
    return godot_distrho_shared_memory->get_sync_flag();
}

void DistrhoSharedMemory::_bind_methods() {
}
