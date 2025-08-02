#ifndef GODOT_DISTRHO_UTILS_H
#define GODOT_DISTRHO_UTILS_H

#include "src/DistrhoDefines.h"
#include <boost/process.hpp>
#include <string>
#include <vector>

typedef struct _XDisplay Display;

typedef unsigned long XID;

typedef XID Window;

START_NAMESPACE_DISTRHO

class GodotDistrhoUtils {

public:
    static std::string get_executable_path();

    static boost::process::child *launch_process(const std::string &p_name, boost::process::environment p_env,
                                                 const std::vector<std::string> &p_args = {});

	static std::string find_godot_package();

    static std::string get_shared_library_path();
};

END_NAMESPACE_DISTRHO

#endif
