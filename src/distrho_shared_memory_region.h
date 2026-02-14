#ifndef DISTRHO_SHARED_MEMORY_REGION_H
#define DISTRHO_SHARED_MEMORY_REGION_H

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include "distrho_shared_memory.h"


namespace godot {

const int SHARED_MEMORY_REGION_SIZE = 65536;
const int MAX_PARAMS = 4096;


struct SharedMemoryRegion {
    std::atomic<uint32_t> count{0};
    std::atomic<uint32_t> parameters[MAX_PARAMS];
};


class DistrhoSharedMemoryRegion {

private:
    std::string default_value = "";

public:
    SharedMemoryRegion *buffer;

protected:
public:
    DistrhoSharedMemoryRegion();
    ~DistrhoSharedMemoryRegion();

    void initialize(DistrhoSharedMemory *p_distrho_shared_memory);
    void initialize_parameters(int p_size);

    void write_parameter_value(int p_index, float p_value);
    float read_parameter_value(int p_index);

    int get_parameter_count();

    void write_state_value(int p_index, const std::string &p_value);
    const std::string& read_state_value(int p_index);

    int get_memory_size();
};

} // namespace godot

#endif
