#ifndef DISTRHO_SHARED_MEMORY_RPC_H
#define DISTRHO_SHARED_MEMORY_RPC_H

#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>
#include <random>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include "distrho_shared_memory.h"


namespace godot {

const std::string RPC_BUFFER_NAME = "RPCBuffer";
const std::string GODOT_RPC_BUFFER_NAME = "GodotRPCBuffer";

const int RPC_BUFFER_SIZE = 4096;
const int SHARED_MEMORY_SIZE = 65536;
const int SHARED_MEMORY_BUFFER_SIZE = 4096; // pow(2, 12);

struct RPCBuffer {
    uint8_t request_buffer[SHARED_MEMORY_BUFFER_SIZE];
    uint8_t response_buffer[SHARED_MEMORY_BUFFER_SIZE];
    int size;
    uint64_t request_id;

    bool ready = false;
    // bool is_alive = false;
    boost::interprocess::interprocess_mutex mutex;
    boost::interprocess::interprocess_condition input_condition;
    boost::interprocess::interprocess_condition output_condition;
};

class DistrhoSharedMemoryRPC {

private:
    std::string name;

public:
    RPCBuffer *buffer;

protected:
public:
    DistrhoSharedMemoryRPC();
    ~DistrhoSharedMemoryRPC();

    void initialize(DistrhoSharedMemory *p_distrho_shared_memory, std::string p_name);

    void write_request(capnp::MallocMessageBuilder *builder, uint64_t request_id);
    capnp::FlatArrayMessageReader read_request();

    void write_reponse(capnp::MallocMessageBuilder *builder);
    capnp::FlatArrayMessageReader read_reponse();

    int get_memory_size();
};

} // namespace godot

#endif
