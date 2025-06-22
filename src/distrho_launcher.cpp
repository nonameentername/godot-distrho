#include "distrho_launcher.h"
#include "distrho_server.h"
#include "distrho_config.h"
#include "godot_cpp/classes/display_server.hpp"
#include "godot_cpp/classes/window.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/vector2i.hpp"

#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/resource_loader.hpp>


using namespace godot;

DistrhoLauncher::DistrhoLauncher() {
    window = memnew(Window);
    //window->set_size(Vector2i(1200, 900));
    //window->set_mode(Window::MODE_MINIMIZED);
    window->set_visible(false);

	Callable callback = Callable(this, "hide_ui");
	window->connect("close_requested", callback);

    add_child(window);
}

DistrhoLauncher::~DistrhoLauncher() {
}

void DistrhoLauncher::_ready() {
    DisplayServer::get_singleton()->window_set_flag(DisplayServer::WINDOW_FLAG_TRANSPARENT, true);

	//call_deferred("initialize");
	initialize();
    DistrhoServer::get_singleton()->set_distrho_launcher(this);
}

void DistrhoLauncher::initialize() {
    DistrhoConfig *config = DistrhoServer::get_singleton()->get_config();

	window->set_size(
			Vector2i(config->get_plugin_width().to_int(), config->get_plugin_height().to_int()));

	Ref<PackedScene> packed_scene = ResourceLoader::get_singleton()->load(
			config->get_plugin_main_scene());

    if (packed_scene.is_valid()) {
        Node *instantiated_scene = cast_to<Node>(packed_scene->instantiate());
        if (instantiated_scene) {
			window->add_child(instantiated_scene);
        } else {
            print_line("Failed to instantiate the scene.");
        }
    } else {
        print_line("Failed to load the scene.");
    }

	window->popup_centered();

    /*
    PackedStringArray args = OS::get_singleton()->get_cmdline_user_args();

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
    */
}

void DistrhoLauncher::show_ui() {
	window->show();
}

void DistrhoLauncher::hide_ui() {
	window->hide();
}

void DistrhoLauncher::_bind_methods() {
    ClassDB::bind_method(D_METHOD("show_ui"), &DistrhoLauncher::show_ui);
    ClassDB::bind_method(D_METHOD("hide_ui"), &DistrhoLauncher::hide_ui);
}
