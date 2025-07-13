#ifndef DISTRHO_COMMON_H
#define DISTRHO_COMMON_H

#include "distrho_shared_memory_rpc.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <capnp/serialize.h>
#include <functional>
#include <kj/string.h>

using namespace boost::interprocess;
using namespace boost::posix_time;

class DistrhoCommon {

private:
protected:
public:
    DistrhoCommon();
    ~DistrhoCommon();

    enum DISTRHO_MODULE_TYPE {
        PLUGIN_TYPE,
        UI_TYPE
    };

    template <typename T, typename R>
    static capnp::FlatArrayMessageReader rpc_call(godot::DistrhoSharedMemoryRPC &rpc_memory,
                                                  std::function<void(typename T::Builder &)> build_request) {
        if (rpc_memory.buffer->ready) {
            scoped_lock<interprocess_mutex> lock(rpc_memory.buffer->mutex);

            capnp::MallocMessageBuilder builder;
            typename T::Builder request = builder.initRoot<T>();

            if (build_request) {
                build_request(request);
            }

            rpc_memory.write_request(&builder, T::_capnpPrivate::typeId);
            rpc_memory.buffer->input_condition.notify_one();

            ptime timeout = microsec_clock::universal_time() + milliseconds(1000);
            bool result = rpc_memory.buffer->output_condition.timed_wait(lock, timeout);

            if (result) {
                return rpc_memory.read_reponse();
            }
        }

        kj::ArrayPtr<const capnp::word> emptyData;
        return capnp::FlatArrayMessageReader(emptyData);
    }

    template <typename T, typename R>
    static void handle_rpc_call(godot::DistrhoSharedMemoryRPC &rpc_memory,
                                std::function<void(typename T::Reader &, typename R::Builder &)> handle_request) {
        capnp::FlatArrayMessageReader reader = rpc_memory.read_request();
        typename T::Reader request = reader.getRoot<T>();

        capnp::MallocMessageBuilder builder;
        typename R::Builder response = builder.initRoot<R>();

        handle_request(request, response);

        rpc_memory.write_reponse(&builder);
        rpc_memory.buffer->output_condition.notify_one();
    }
};

#endif
