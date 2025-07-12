#include "distrho_common.h"
#include "libgodot_distrho.h"

extern LibGodot libgodot;

int main(int argc, char **argv) {
    std::string program;
    std::string shared_memory_id;
    if (argc > 0) {
        program = std::string(argv[0]);
    }

    if (argc != 1) {
        return EXIT_SUCCESS;
    }

    const char *window_id = std::getenv("DISTRHO_WINDOW_ID");
    if (window_id == NULL) {
        window_id = "";
    }

    printf("main: window_id = %s\n", window_id);

    const char *module_type = std::getenv("DISTRHO_MODULE_TYPE");
    if (module_type == NULL) {
        module_type = std::to_string(DistrhoCommon::PLUGIN_TYPE).c_str();
    }

    std::vector<std::string> args;

    if (std::stoi(module_type) == DistrhoCommon::PLUGIN_TYPE) {
        args = {program,
                "--display-driver",
                "headless",
                "--audio-driver",
                "Distrho",
                "--path",
                "/home/wmendiza/source/godot-distrho"};
    } else {
        args = {program,
                "--rendering-method",
                "gl_compatibility",
                "--rendering-driver",
                "opengl3",
                "--display-driver",
                "x11",
                "--audio-driver",
                "Dummy",
                "--path",
                "/home/wmendiza/source/godot-distrho"};

        /*
        if (std::stoi(window_id) != 0) {
            args.push_back("--wid");
            args.push_back(window_id);
        }
        */
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
    while (!instance->iteration()) {
    }
    libgodot.destroy_godot_instance(instance);

    return EXIT_SUCCESS;
}
