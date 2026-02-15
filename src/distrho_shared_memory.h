#ifndef DISTRHO_SHARED_MEMORY_H
#define DISTRHO_SHARED_MEMORY_H

#include <DistrhoDetails.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>
#include <random>

#ifdef _WIN32
#include <boost/interprocess/managed_windows_shared_memory.hpp>
#endif

namespace godot {


class DistrhoSharedMemory {

private:
#ifdef _WIN32
    std::unique_ptr<boost::interprocess::managed_windows_shared_memory> shared_memory;
#else
    std::unique_ptr<boost::interprocess::managed_shared_memory> shared_memory;
#endif

public:
    std::string shared_memory_name;
    bool is_host;

protected:
public:
    DistrhoSharedMemory();
    ~DistrhoSharedMemory();

    void initialize(std::string p_shared_memory_name = "", int p_size = 0);

    bool get_is_host();

    template <class T> T *create_buffer(const std::string &p_name);

    std::string get_shared_memory_name();
};

} // namespace godot

#endif
