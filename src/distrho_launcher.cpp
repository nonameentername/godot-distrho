#include "distrho_launcher.h"
#include "distrho_server.h"
#include "distrho_config.h"
#include "godot_cpp/variant/utility_functions.hpp"

#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

using namespace godot;

DistrhoLauncher::DistrhoLauncher() {
}

DistrhoLauncher::~DistrhoLauncher() {
}

void DistrhoLauncher::initialize() {
    PackedStringArray args = OS::get_singleton()->get_cmdline_args();

    bool start_distrho_plugin = false;
    bool start_distrho_ui = false;

    for (int i = 0; i < args.size(); i++) {
        if (args[i] == "--distrho-plugin") {
            start_distrho_plugin = true;
            break;
        }
        if (args[i] == "--distrho-ui") {
            start_distrho_ui = true;
            break;
        }
    }

    DistrhoConfig *config = DistrhoServer::get_singleton()->get_config();

    if (start_distrho_plugin) {
        get_tree()->call_deferred("change_scene_to_file", config->get_plugin_main_scene());
    } else if (start_distrho_ui) {
        get_tree()->call_deferred("change_scene_to_file", config->get_ui_main_scene());
    } else {
        godot::UtilityFunctions::printerr("missing argument --distrho-plugin or --distrho-ui");
    }
}

void DistrhoLauncher::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize"), &DistrhoLauncher::initialize);
}
