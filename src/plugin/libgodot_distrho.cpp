#include <cstdlib>
#include <dlfcn.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>

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

GDExtensionBool GDE_EXPORT gdextension_default_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                                    GDExtensionClassLibraryPtr p_library,
                                                    GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_object(p_get_proc_address, p_library, r_initialization);

    init_object.register_initializer(initialize_default_module);
    init_object.register_terminator(uninitialize_default_module);
    init_object.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_object.init();
}
}

LibGodot::LibGodot() {
}

LibGodot::~LibGodot() {
}

godot::GodotInstance *LibGodot::create_godot_instance(int p_argc, char *p_argv[]) {
    GDExtensionObjectPtr instance =
        libgodot_create_godot_instance(p_argc, p_argv, gdextension_default_init, NULL, NULL, NULL, NULL);
    if (instance == nullptr) {
        return nullptr;
    }
    return reinterpret_cast<godot::GodotInstance *>(godot::internal::get_object_instance_binding(instance));
}

void LibGodot::destroy_godot_instance(godot::GodotInstance *instance) {
    GDExtensionObjectPtr obj =
        godot::internal::gdextension_interface_object_get_instance_from_id(instance->get_instance_id());
    libgodot_destroy_godot_instance(obj);
}

char *LibGodot::get_absolute_path() {
    return absolute_path;
}

LibGodot libgodot;
