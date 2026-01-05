#include "distrho_launcher.h"
#include "distrho_common.h"
#include "distrho_config.h"
#include "distrho_plugin_server.h"
#include "godot_cpp/classes/canvas_item.hpp"
#include "godot_cpp/classes/display_server.hpp"
#include "godot_cpp/variant/node_path.hpp"

#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <string>

using namespace godot;

DistrhoLauncher::DistrhoLauncher() {
}

DistrhoLauncher::~DistrhoLauncher() {
}

void DistrhoLauncher::_ready() {
    DisplayServer::get_singleton()->window_set_flag(DisplayServer::WINDOW_FLAG_TRANSPARENT, true);

    initialize();
    DistrhoPluginServer::get_singleton()->set_distrho_launcher(this);
}

void DistrhoLauncher::initialize() {
    DistrhoConfig *config = DistrhoPluginServer::get_singleton()->get_config();

    const char *module_type = std::getenv("DISTRHO_MODULE_TYPE");
    if (module_type == NULL) {
        module_type = std::to_string(DistrhoCommon::BOTH_TYPE).c_str();
    }

    printf("DISTRHO_MODULE_TYPE = %s\n", module_type);

    if (std::stoi(module_type) == DistrhoCommon::BOTH_TYPE) {
        load_scene(config->get_plugin_main_scene(), false);
        load_scene(config->get_ui_main_scene());
    } else if (std::stoi(module_type) == DistrhoCommon::PLUGIN_TYPE) {
        load_scene(config->get_plugin_main_scene());
    } else {
        load_scene(config->get_ui_main_scene());
    }
}

void DistrhoLauncher::load_scene(String p_scene, bool p_visible) {
    Ref<PackedScene> packed_scene = ResourceLoader::get_singleton()->load(p_scene);

    if (packed_scene.is_valid()) {
        CanvasItem *instantiated_scene = cast_to<CanvasItem>(packed_scene->instantiate());
        if (instantiated_scene) {
            instantiated_scene->set_visible(p_visible);
            add_child(instantiated_scene);
        } else {
            print_line("Failed to instantiate the scene.");
        }
    } else {
        print_line("Failed to load the scene.");
    }
}

void DistrhoLauncher::_bind_methods() {
}
