#ifndef DISTRHO_SHARED_MEMORY_RPC_H
#define DISTRHO_SHARED_MEMORY_RPC_H

#include <memory>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

namespace godot {

const int RPC_BUFFER_SIZE = 4096;
const int SHARED_MEMORY_SIZE = 65536;
const int SHARED_MEMORY_BUFFER_SIZE = 4096; //pow(2, 12);


struct RPCBuffer {
    uint8_t request_buffer[SHARED_MEMORY_BUFFER_SIZE];
    uint8_t response_buffer[SHARED_MEMORY_BUFFER_SIZE];
    int size;
    uint64_t request_id;

    bool godot_ready = false;
    boost::interprocess::interprocess_mutex mutex;
    boost::interprocess::interprocess_condition input_condition;
    boost::interprocess::interprocess_condition output_condition;
};

class DistrhoSharedMemoryRPC {

private:
    boost::uuids::random_generator generator;
	std::unique_ptr<boost::interprocess::managed_shared_memory> shared_memory;
public:
	RPCBuffer* buffer;

    std::string shared_memory_name;
	bool is_host;

protected:

public:
    DistrhoSharedMemoryRPC();
    ~DistrhoSharedMemoryRPC();

    void initialize(std::string p_shared_memory_name = "");

	void write_request(const float *p_buffer, int p_frames);
    capnp::FlatArrayMessageReader read_request();

	void write_reponse(capnp::MallocMessageBuilder *builder);
	void read_reponse(float *p_buffer, int p_frames);

    std::string get_shared_memory_name();
};

}

#endif
