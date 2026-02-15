#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "distrho_shared_memory_rpc.h"
#include "godot_distrho_schema.capnp.h"

using namespace godot;

namespace bip = boost::interprocess;

static boost::uuids::random_generator &uuid_gen() {
    static boost::uuids::random_generator gen;
    return gen;
}

DistrhoSharedMemoryRPC::DistrhoSharedMemoryRPC() {
}

DistrhoSharedMemoryRPC::~DistrhoSharedMemoryRPC() {
}

void DistrhoSharedMemoryRPC::initialize(DistrhoSharedMemory *p_distrho_shared_memory, std::string p_name) {
    buffer = p_distrho_shared_memory->create_buffer<RPCBuffer>(p_name);
}

void DistrhoSharedMemoryRPC::write_request(capnp::MallocMessageBuilder *builder, uint64_t request_id) {
    kj::Array<capnp::word> words = capnp::messageToFlatArray(*builder);
    kj::ArrayPtr<const kj::byte> bytes = words.asBytes();

    if (bytes.size() > RPC_BUFFER_SIZE) {
        // TODO: log error?
        // throw std::runtime_error("RPC buffer too small!");
        buffer->size = 0;
    } else {
        std::memcpy(buffer->request_buffer, bytes.begin(), bytes.size());
        buffer->size = bytes.size();
        buffer->request_id = request_id;
    }
}

capnp::FlatArrayMessageReader DistrhoSharedMemoryRPC::read_request() {
    auto wordPtr = reinterpret_cast<const capnp::word *>(buffer->request_buffer);
    std::size_t wordCount = buffer->size / sizeof(capnp::word);

    capnp::FlatArrayMessageReader reader(kj::arrayPtr(wordPtr, wordCount));

    return reader;
}

void DistrhoSharedMemoryRPC::write_reponse(capnp::MallocMessageBuilder *builder) {
    kj::Array<capnp::word> words = capnp::messageToFlatArray(*builder);
    kj::ArrayPtr<const kj::byte> bytes = words.asBytes();

    if (bytes.size() > RPC_BUFFER_SIZE) {
        // TODO: log error?
        // throw std::runtime_error("RPC buffer too small!");
        buffer->size = 0;
    } else {
        std::memcpy(buffer->response_buffer, bytes.begin(), bytes.size());
        buffer->size = bytes.size();
    }
}

capnp::FlatArrayMessageReader DistrhoSharedMemoryRPC::read_reponse() {
    auto wordPtr = reinterpret_cast<const capnp::word *>(buffer->response_buffer);
    std::size_t wordCount = buffer->size / sizeof(capnp::word);

    capnp::FlatArrayMessageReader reader(kj::arrayPtr(wordPtr, wordCount));

    return reader;
}

int DistrhoSharedMemoryRPC::get_memory_size() {
    return SHARED_MEMORY_SIZE;
}
