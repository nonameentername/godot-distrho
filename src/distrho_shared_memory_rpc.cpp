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
    if (is_host && !shared_memory_name.empty()) {
#ifndef _WIN32
        bip::shared_memory_object::remove(shared_memory_name.c_str());
#endif
    }
}

void DistrhoSharedMemoryRPC::initialize(std::string p_name, std::string p_shared_memory_name) {
    if (p_shared_memory_name.length() == 0) {
        is_host = true;
        boost::uuids::uuid uuid = uuid_gen()();
        shared_memory_name = boost::uuids::to_string(uuid);

#if !DISTRHO_PLUGIN_ENABLE_SUBPROCESS && DEBUG
        printf("export %s=%s\n", p_name.c_str(), shared_memory_name.c_str());
#endif

#ifdef _WIN32
        shared_memory = std::make_unique<boost::interprocess::managed_windows_shared_memory>(
            bip::create_only, shared_memory_name.c_str(), SHARED_MEMORY_SIZE);
#else
        bip::shared_memory_object::remove(shared_memory_name.c_str());
        shared_memory = std::make_unique<boost::interprocess::managed_shared_memory>(
            bip::create_only, shared_memory_name.c_str(), SHARED_MEMORY_SIZE);
#endif

        buffer = shared_memory->construct<RPCBuffer>("RPCBuffer")();
    } else {
        is_host = false;
        shared_memory_name = p_shared_memory_name;

#ifdef _WIN32
        shared_memory = std::make_unique<bip::managed_windows_shared_memory>(bip::open_only, shared_memory_name.c_str());
#else
        shared_memory = std::make_unique<bip::managed_shared_memory>(bip::open_only, shared_memory_name.c_str());
#endif
        buffer = shared_memory->find<RPCBuffer>("RPCBuffer").first;
    }
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

std::string DistrhoSharedMemoryRPC::get_shared_memory_name() {
    return shared_memory_name;
}

bool DistrhoSharedMemoryRPC::get_is_host() {
    return is_host;
}
