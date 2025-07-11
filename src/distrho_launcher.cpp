#include "distrho_launcher.h"
#include "distrho_common.h"
#include "distrho_config.h"
#include "distrho_server.h"
#include "godot_cpp/classes/display_server.hpp"
#include "godot_cpp/classes/window.hpp"

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
    DistrhoServer::get_singleton()->set_distrho_launcher(this);
}

void DistrhoLauncher::initialize() {
    DistrhoConfig *config = DistrhoServer::get_singleton()->get_config();

    Ref<PackedScene> packed_scene;

    const char *module_type = std::getenv("DISTRHO_MODULE_TYPE");
    if (module_type == NULL) {
        module_type = std::to_string(DistrhoCommon::PLUGIN_TYPE).c_str();
    }

    printf("DISTRHO_MODULE_TYPE = %s\n", module_type);

    if (std::stoi(module_type) == DistrhoCommon::PLUGIN_TYPE) {
        packed_scene = ResourceLoader::get_singleton()->load(config->get_plugin_main_scene());
    } else {
        packed_scene = ResourceLoader::get_singleton()->load(config->get_ui_main_scene());
    }

    if (packed_scene.is_valid()) {
        Node *instantiated_scene = cast_to<Node>(packed_scene->instantiate());
        if (instantiated_scene) {
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
