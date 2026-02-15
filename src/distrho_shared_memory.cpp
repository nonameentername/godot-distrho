#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "distrho_shared_memory.h"
#include "distrho_shared_memory_audio.h"
#include "distrho_shared_memory_rpc.h"
#include "distrho_shared_memory_region.h"

using namespace godot;

namespace bip = boost::interprocess;

static boost::uuids::random_generator &uuid_gen() {
    static boost::uuids::random_generator gen;
    return gen;
}

DistrhoSharedMemory::DistrhoSharedMemory() {
}

DistrhoSharedMemory::~DistrhoSharedMemory() {
    if (is_host && !shared_memory_name.empty()) {
#ifndef _WIN32
        bip::shared_memory_object::remove(shared_memory_name.c_str());
#endif
    }
}

void DistrhoSharedMemory::initialize( std::string p_shared_memory_name, int size) {
    if (p_shared_memory_name.length() == 0) {
        is_host = true;
        boost::uuids::uuid uuid = uuid_gen()();
        shared_memory_name = boost::uuids::to_string(uuid);

#if !DISTRHO_PLUGIN_ENABLE_SUBPROCESS && DEBUG
        printf("export DISTRHO_SHARED_MEMORY_UUID=%s\n", shared_memory_name.c_str());
#endif

#ifdef _WIN32
        shared_memory = std::make_unique<boost::interprocess::managed_windows_shared_memory>(
            bip::create_only, shared_memory_name.c_str(), size);
#else
        bip::shared_memory_object::remove(shared_memory_name.c_str());
        shared_memory = std::make_unique<boost::interprocess::managed_shared_memory>(
            bip::create_only, shared_memory_name.c_str(), size);
#endif
    } else {
        is_host = false;
        shared_memory_name = std::move(p_shared_memory_name);

#ifdef _WIN32
        shared_memory = std::make_unique<bip::managed_windows_shared_memory>(bip::open_only, shared_memory_name.c_str());
#else
        shared_memory = std::make_unique<bip::managed_shared_memory>(bip::open_only, shared_memory_name.c_str());
#endif
    }
}

bool DistrhoSharedMemory::get_is_host() {
    return is_host;
}

template <class T> T *DistrhoSharedMemory::create_buffer(const std::string &p_name) {
    T *buffer;
    if (is_host) {
        buffer = shared_memory->construct<T>(p_name.c_str())();
    } else {
        buffer = shared_memory->find<T>(p_name.c_str()).first;
    }
    return buffer;
}

template AudioBuffer* DistrhoSharedMemory::create_buffer<AudioBuffer>(const std::string &p_name);
template RPCBuffer* DistrhoSharedMemory::create_buffer<RPCBuffer>(const std::string &p_name);
template SharedMemoryRegion* DistrhoSharedMemory::create_buffer<SharedMemoryRegion>(const std::string &p_name);
