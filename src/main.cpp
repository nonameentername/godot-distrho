#include "libgodot_distrho.h"

extern LibGodot libgodot;

int main(int argc, char **argv) {
    std::string program;
    if (argc > 0) {
        program = std::string(argv[0]);
    }

    if (argc > 2) {
        return EXIT_SUCCESS;
    }

    std::vector<std::string> args = { program, "--rendering-method", "gl_compatibility", "--rendering-driver", "opengl3", "--display-driver", "x11" };

    std::vector<char*> argvs;
    for (const auto& arg : args) {
        argvs.push_back((char*)arg.data());
    }
    argvs.push_back(nullptr);

    godot::GodotInstance *instance = libgodot.create_godot_instance(argvs.size(), argvs.data());
    if (instance == nullptr) {
        fprintf(stderr, "Error creating Godot instance\n");
        return EXIT_FAILURE;
    }

    instance->start();
    while (!instance->iteration()) {}
    libgodot.destroy_godot_instance(instance);

    return EXIT_SUCCESS;
}
