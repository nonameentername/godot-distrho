#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "distrho_schema.capnp.h"
#include "distrho_shared_memory_rpc.h"


using namespace godot;

namespace bip = boost::interprocess;


DistrhoSharedMemoryRPC::DistrhoSharedMemoryRPC() {
}

DistrhoSharedMemoryRPC::~DistrhoSharedMemoryRPC() {
    if (is_host) {
        bip::shared_memory_object::remove(shared_memory_name.c_str());
    }
}

void DistrhoSharedMemoryRPC::initialize(std::string p_shared_memory_name) {
    if (p_shared_memory_name.length() == 0) {
		is_host = true;
		boost::uuids::uuid uuid = generator();
		shared_memory_name = boost::uuids::to_string(uuid);

#if !DISTRHO_PLUGIN_ENABLE_SUBPROCESS && DEBUG
        printf("shared_memory_name %s\n", shared_memory_name.c_str());
#endif

    	bip::shared_memory_object::remove(shared_memory_name.c_str());
		shared_memory = std::make_unique<boost::interprocess::managed_shared_memory>(
				bip::create_only, shared_memory_name.c_str(), SHARED_MEMORY_SIZE);

    	buffer = shared_memory->construct<RPCBuffer>("RPCBuffer")();
    } else {
		is_host = false;
    	shared_memory_name = p_shared_memory_name;

		shared_memory = std::make_unique<bip::managed_shared_memory>(
				bip::open_only, shared_memory_name.c_str());
		buffer = shared_memory->find<RPCBuffer>("RPCBuffer").first;
	}
}

void DistrhoSharedMemoryRPC::write_request(const float *p_buffer, int p_frames) {
}

capnp::FlatArrayMessageReader DistrhoSharedMemoryRPC::read_request() {
    auto wordPtr = reinterpret_cast<const capnp::word*>(buffer->request_buffer);
    std::size_t wordCount = buffer->size / sizeof(capnp::word);

    capnp::FlatArrayMessageReader reader(kj::arrayPtr(wordPtr, wordCount));

    return reader;
}

void DistrhoSharedMemoryRPC::write_reponse(capnp::MallocMessageBuilder *builder) {
    kj::Array<capnp::word> words = capnp::messageToFlatArray(*builder);
    kj::ArrayPtr<const kj::byte> bytes = words.asBytes();

    if (bytes.size() > RPC_BUFFER_SIZE) {
        //TODO: log error?
        //throw std::runtime_error("RPC buffer too small!");
        buffer->size = 0;
    } else {
        std::memcpy(buffer->response_buffer, bytes.begin(), bytes.size());
        buffer->size = bytes.size();
    }
}

void DistrhoSharedMemoryRPC::read_reponse(float *p_buffer, int p_frames) {
}

std::string DistrhoSharedMemoryRPC::get_shared_memory_name() {
	return shared_memory_name;
}
