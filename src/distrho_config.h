#ifndef DISTRHO_CONFIG_H
#define DISTRHO_CONFIG_H

#include <godot_cpp/classes/config_file.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>

namespace godot {

class DistrhoConfig : public Object {
    GDCLASS(DistrhoConfig, Object);

private:
    ConfigFile *config_file;
    HashMap<String, String> plugin_values;
    HashMap<String, String> ui_values;

protected:
public:
    DistrhoConfig();
    ~DistrhoConfig();

    String get_plugin_main_scene();

    String get_ui_main_scene();

    static void _bind_methods();
};
} // namespace godot

#endif
