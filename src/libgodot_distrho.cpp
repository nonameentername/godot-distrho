#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string>
#include <vector>

#include "libgodot_distrho.h"

extern "C" {

static void initialize_default_module(godot::ModuleInitializationLevel p_level) {
    if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

}

static void uninitialize_default_module(godot::ModuleInitializationLevel p_level) {
    if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

GDExtensionBool GDE_EXPORT gdextension_default_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_object(p_get_proc_address, p_library, r_initialization);

    init_object.register_initializer(initialize_default_module);
    init_object.register_terminator(uninitialize_default_module);
    init_object.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_object.init();
}

}

LibGodot::LibGodot(std::string p_path) {
    handle = dlopen(p_path.c_str(), RTLD_LAZY);
    if (handle == nullptr) {
        fprintf(stderr, "Error opening libgodot: %s\n", dlerror());
        return;
    }
    *(void**)(&func_libgodot_create_godot_instance) = dlsym(handle, "libgodot_create_godot_instance");
    if (func_libgodot_create_godot_instance == nullptr) {
        fprintf(stderr, "Error acquiring function: %s\n", dlerror());
        dlclose(handle);
        handle == nullptr;
        return;
    }
    *(void**)(&func_libgodot_destroy_godot_instance) = dlsym(handle, "libgodot_destroy_godot_instance");
    if (func_libgodot_destroy_godot_instance == nullptr) {
        fprintf(stderr, "Error acquiring function: %s\n", dlerror());
        dlclose(handle);
        handle == nullptr;
        return;
    }
}

LibGodot::~LibGodot() {
    if (is_open()) {
        dlclose(handle);
    }
}

bool LibGodot::is_open() {
    return handle != nullptr && func_libgodot_create_godot_instance != nullptr;
}

godot::GodotInstance* LibGodot::create_godot_instance(int p_argc, char *p_argv[], GDExtensionInitializationFunction p_init_func) {

    if (!is_open()) {
        return nullptr;
    }
    GDExtensionObjectPtr instance = func_libgodot_create_godot_instance(p_argc, p_argv, p_init_func);
    if (instance == nullptr) {
        return nullptr;
    }
    return reinterpret_cast<godot::GodotInstance *>(godot::internal::get_object_instance_binding(instance));
}

void LibGodot::destroy_godot_instance(godot::GodotInstance* instance) {
    GDExtensionObjectPtr obj = godot::internal::gdextension_interface_object_get_instance_from_id(instance->get_instance_id());
    func_libgodot_destroy_godot_instance(obj);
}

int mmain(int argc, char **argv) {

    LibGodot libgodot;

    std::string program;
    if (argc > 0) {
        program = std::string(argv[0]);
    }
    std::vector<std::string> args = { program, "--path", "../../project/", "--rendering-method", "gl_compatibility", "--rendering-driver", "opengl3" };

    std::vector<char*> argvs;
    for (const auto& arg : args) {
        argvs.push_back((char*)arg.data());
    }
    argvs.push_back(nullptr);

    godot::GodotInstance *instance = libgodot.create_godot_instance(argvs.size(), argvs.data(), gdextension_default_init);
    if (instance == nullptr) {
        fprintf(stderr, "Error creating Godot instance\n");
        return EXIT_FAILURE;
    }

    instance->start();
    while (!instance->iteration()) {}
    libgodot.destroy_godot_instance(instance);

    return EXIT_SUCCESS;
}

LibGodot libgodot;
