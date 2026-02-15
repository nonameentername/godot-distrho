
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "distrho_shared_memory_region.h"
#include <bit>

using namespace godot;
using namespace boost::interprocess;


inline uint32_t float_to_u32(float v) {
    uint32_t u;
    std::memcpy(&u, &v, sizeof(u));
    return u;
}

inline float u32_to_float(uint32_t u) {
    float v;
    std::memcpy(&v, &u, sizeof(v));
    return v;
}


DistrhoSharedMemoryRegion::DistrhoSharedMemoryRegion() {
}

DistrhoSharedMemoryRegion::~DistrhoSharedMemoryRegion() {
}

void DistrhoSharedMemoryRegion::initialize(DistrhoSharedMemory *p_distrho_shared_memory) {
    buffer = p_distrho_shared_memory->create_buffer<SharedMemoryRegion>("SharedMemoryRegion");
}

void DistrhoSharedMemoryRegion::initialize_parameters(int p_size) {
    buffer->count.store(p_size, std::memory_order_relaxed);
}

void DistrhoSharedMemoryRegion::write_parameter_value(int p_index, float p_value) {
    if (p_index < MAX_PARAMS) {
        buffer->parameters[p_index].store(float_to_u32(p_value), std::memory_order_relaxed);
    }
}


float DistrhoSharedMemoryRegion::read_parameter_value(int p_index) {
    if (p_index < MAX_PARAMS) {
        return u32_to_float(buffer->parameters[p_index].load(std::memory_order_relaxed));
    } else {
        return 0;
    }
}

int DistrhoSharedMemoryRegion::get_parameter_count() {
    return buffer->count.load(std::memory_order_relaxed);
}

void DistrhoSharedMemoryRegion::write_state_value(int p_index, const std::string &p_value) {
}

const std::string& DistrhoSharedMemoryRegion::read_state_value(int p_index) {
	return default_value;
}

int DistrhoSharedMemoryRegion::get_memory_size() {
    return SHARED_MEMORY_REGION_SIZE;
}
