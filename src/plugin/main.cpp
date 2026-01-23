#include "distrho_common.h"
//#include "godot_cpp/classes/display_server.hpp"
//#include "godot_cpp/variant/dictionary.hpp"
#include "godot_distrho_utils.h"
#include "libgodot_distrho.h"

#include <dirent.h>
#include <string>
#include <vector>

//#include <X11/Xlib.h>


extern LibGodot libgodot;

USE_NAMESPACE_DISTRHO

int main(int argc, char **argv) {
    std::string program;
    std::string shared_memory_id;
    if (argc > 0) {
        program = std::string(argv[0]);
    }

    if (argc != 1) {
        return EXIT_SUCCESS;
    }

    const char *parent_window_id = std::getenv("GODOT_PARENT_WINDOW_ID");

    const char *module_type = std::getenv("DISTRHO_MODULE_TYPE");
    if (module_type == NULL) {
        module_type = std::to_string(DistrhoCommon::PLUGIN_TYPE).c_str();
    }

    const char *distrho_path = std::getenv("DISTRHO_PATH");

    std::string godot_package = GodotDistrhoUtils::find_godot_package();

    std::vector<std::string> args;

    if (std::stoi(module_type) == DistrhoCommon::PLUGIN_TYPE) {
        args = {program, "--display-driver", "headless", "--audio-driver", "Distrho"};
    } else {
        args = {program,
                "--rendering-method",
                "gl_compatibility",
                "--rendering-driver",
                "opengl3",
                "--display-driver",
#ifdef _WIN32
                "windows",
#else
                "x11",
#endif
                "--audio-driver",
                "Dummy"};

		/*
        if (parent_window_id != NULL) {
            args.push_back("--wid");
            args.push_back(parent_window_id);
        }
		*/
    }

    if (godot_package.size() > 0) {
        args.push_back("--main-pack");
        args.push_back(godot_package);
    } else if (distrho_path != NULL) {
        args.push_back("--path");
        args.push_back(distrho_path);
    }

    std::vector<char *> argvs;
    for (const auto &arg : args) {
        argvs.push_back((char *)arg.data());
    }
    argvs.push_back(nullptr);

    godot::GodotInstance *instance = libgodot.create_godot_instance(argvs.size(), argvs.data());
    if (instance == nullptr) {
        fprintf(stderr, "Error creating Godot instance\n");
        return EXIT_FAILURE;
    }

    instance->start();

	/*
    if (parent_window_id != NULL) {
        Window new_parent = std::stol(parent_window_id);

        Window godot_window_id = godot::DisplayServer::get_singleton()->window_get_native_handle(godot::DisplayServer::WINDOW_HANDLE, 0);
        fprintf(stderr, "werner:native_window_id = %ld\n", godot_window_id);

        Display *display = XOpenDisplay(NULL);

        XUnmapWindow(display, godot_window_id);
        XSync(display, false);

        XReparentWindow(display, godot_window_id, new_parent, 0, 0);

        XMapWindow(display, godot_window_id);

        XFlush(display);
        XSync(display, false);

        fprintf(stderr, "reparent = %ld %ld\n", godot_window_id, new_parent);
    }
	*/

    while (!instance->iteration()) {
    }
    libgodot.destroy_godot_instance(instance);

    return EXIT_SUCCESS;
}
