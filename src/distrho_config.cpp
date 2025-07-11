#include "distrho_config.h"
#include "godot_cpp/classes/config_file.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;

DistrhoConfig::DistrhoConfig() {
    config_file = memnew(ConfigFile);

    Error error = config_file->load("res://godot-distrho.cfg");

    if (error) {
        godot::UtilityFunctions::printerr("Could not find config file: godot-distrho.cfg");
    } else {
        if (!config_file->has_section("distrho_plugin")) {
            godot::UtilityFunctions::printerr("godot-distrho.cfg: Could not find section [distrho_plugin]");
            ERR_FAIL_MSG("godot-distrho.cfg: Could not find section [distrho_plugin]");
        } else {
            PackedStringArray keys = {"main_scene"};

            for (const String &key : keys) {
                if (!config_file->has_section_key("distrho_plugin", key)) {
                    godot::UtilityFunctions::printerr("godot-distrho.cfg: Could not find key `", key,
                                                      "` in section [distrho_plugin]");
                }
                plugin_values.insert(key, config_file->get_value("distrho_plugin", key, ""));
            }

            for (const String &key : keys) {
                if (!config_file->has_section_key("distrho_ui", key)) {
                    godot::UtilityFunctions::printerr("godot-distrho.cfg: Could not find key `", key,
                                                      "` in section [distrho_ui]");
                }
                ui_values.insert(key, config_file->get_value("distrho_ui", key, ""));
            }
        }
    }
}

DistrhoConfig::~DistrhoConfig() {
}

String DistrhoConfig::get_plugin_main_scene() {
    return plugin_values["main_scene"];
}

String DistrhoConfig::get_ui_main_scene() {
    return ui_values["main_scene"];
}

void DistrhoConfig::_bind_methods() {
}
