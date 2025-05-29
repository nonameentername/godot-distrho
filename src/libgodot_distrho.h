#ifndef LIBGODOT_DISTRHO_H
#define LIBGODOT_DISTRHO_H

#include <libgodot.h>

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/godot_instance.hpp>

#ifdef __APPLE__
#define LIBGODOT_LIBRARY_NAME "libgodot.dylib"
#else
#define LIBGODOT_LIBRARY_NAME "./libgodot.so"
#endif


class LibGodot {
public:
    LibGodot(std::string p_path = LIBGODOT_LIBRARY_NAME);

    ~LibGodot();

    bool is_open();

    godot::GodotInstance *create_godot_instance(int p_argc, char *p_argv[], GDExtensionInitializationFunction p_init_func);

    void destroy_godot_instance(godot::GodotInstance* instance);

private:
    void *handle = nullptr;
    GDExtensionObjectPtr (*func_libgodot_create_godot_instance)(int, char *[], GDExtensionInitializationFunction) = nullptr;
    void (*func_libgodot_destroy_godot_instance)(GDExtensionObjectPtr) = nullptr;
};

extern LibGodot libgodot;

#endif
