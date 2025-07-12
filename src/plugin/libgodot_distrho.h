#ifndef LIBGODOT_DISTRHO_H
#define LIBGODOT_DISTRHO_H

#include <libgodot.h>

#include <godot_cpp/classes/godot_instance.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

class LibGodot {
public:
    LibGodot();

    ~LibGodot();

    godot::GodotInstance *create_godot_instance(int p_argc, char *p_argv[]);

    void destroy_godot_instance(godot::GodotInstance *instance);

    char *get_absolute_path();

private:
    void *handle = nullptr;
    char *absolute_path = nullptr;

    // GDExtensionObjectPtr (*func_libgodot_create_godot_instance)(int, char *[], GDExtensionInitializationFunction,
    // InvokeCallbackFunction, ExecutorData, InvokeCallbackFunction, ExecutorData);

    // void (*func_libgodot_destroy_godot_instance)(GDExtensionObjectPtr) = nullptr;
};

extern LibGodot libgodot;

#endif
