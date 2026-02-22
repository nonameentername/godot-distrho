#ifndef GODOT_DISTRHO_UI_CLIENT_H
#define GODOT_DISTRHO_UI_CLIENT_H

#include "DistrhoDetails.hpp"
#include "DistrhoPlugin.hpp"
#include "distrho_common.h"
#include "distrho_shared_memory_rpc.h"
#include "distrho_shared_memory_region.h"
#include "godot_distrho_schema.capnp.h"

#if defined(_WIN32)
#include <winsock2.h>
#endif

#include <boost/process.hpp>
#include <cstdint>
#include <boost/process/v1/group.hpp>
#include <boost/process/v1/child.hpp>

START_NAMESPACE_DISTRHO

class GodotDistrhoUIClient {
private:
    int64_t native_window_id = 0;
    boost::process::v1::group windows_group;
    boost::process::v1::child *plugin;
    mutable godot::DistrhoSharedMemory shared_memory;
    mutable godot::DistrhoSharedMemoryRPC rpc_memory;
    mutable godot::DistrhoSharedMemoryRPC godot_rpc_memory;
    mutable godot::DistrhoSharedMemoryRegion shared_memory_region;

protected:
    template <typename T, typename R>
    capnp::FlatArrayMessageReader rpc_call(bool &result, std::function<void(typename T::Builder &)> build_request = nullptr) const;

public:
    GodotDistrhoUIClient(DistrhoCommon::DISTRHO_MODULE_TYPE p_type, int64_t p_parent_window_id);

    ~GodotDistrhoUIClient();

    void run();

    bool is_ready();

    int64_t get_native_window_id();

    void parameter_changed(int p_index, float p_value);

    void state_changed(std::string p_index, std::string p_value);

    bool shutdown();

    godot::DistrhoSharedMemoryRPC *get_godot_rpc_memory();
    godot::DistrhoSharedMemoryRegion *get_shared_memory_region();
};

END_NAMESPACE_DISTRHO

#endif
