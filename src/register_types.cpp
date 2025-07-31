#include "register_types.h"

#include <gdextension_interface.h>

#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "audio_effect_send_distrho_channel.h"
#include "audio_stream_distrho.h"
#include "audio_stream_player_distrho.h"
#include "distrho_audio_port.h"
#include "distrho_config.h"
#include "distrho_launcher.h"
#include "distrho_midi_event.h"
#include "distrho_parameter.h"
#include "distrho_plugin_instance.h"
#include "distrho_plugin_server.h"
#include "distrho_plugin_server_node.h"
#include "distrho_ui_instance.h"
#include "distrho_ui_server.h"
#include "distrho_ui_server_node.h"

using namespace godot;

static DistrhoPluginServer *distrho_plugin_server;
static DistrhoUIServer *distrho_ui_server;

void initialize_godot_distrho_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<AudioStreamDistrho>();
    ClassDB::register_class<AudioStreamPlaybackDistrho>();
    ClassDB::register_class<AudioEffectSetDistrhoChannel>();
    ClassDB::register_class<AudioEffectSetDistrhoChannelInstance>();
    ClassDB::register_class<DistrhoConfig>();
    ClassDB::register_class<DistrhoAudioPort>();
    ClassDB::register_class<DistrhoParameter>();
    ClassDB::register_class<DistrhoLauncher>();
    ClassDB::register_class<DistrhoMidiEvent>();

    ClassDB::register_class<DistrhoPluginInstance>();
    ClassDB::register_class<DistrhoPluginServerNode>();
    ClassDB::register_class<DistrhoPluginServer>();
    distrho_plugin_server = memnew(DistrhoPluginServer);
    Engine::get_singleton()->register_singleton("DistrhoPluginServer", DistrhoPluginServer::get_singleton());

    ClassDB::register_class<DistrhoUIInstance>();
    ClassDB::register_class<DistrhoUIServerNode>();
    ClassDB::register_class<DistrhoUIServer>();
    distrho_ui_server = memnew(DistrhoUIServer);
    Engine::get_singleton()->register_singleton("DistrhoUIServer", DistrhoUIServer::get_singleton());
}

void uninitialize_godot_distrho_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    Engine::get_singleton()->unregister_singleton("DistrhoPluginServer");
    distrho_plugin_server->finish();
    memdelete(distrho_plugin_server);

    Engine::get_singleton()->unregister_singleton("DistrhoUIServer");
    distrho_ui_server->finish();
    memdelete(distrho_ui_server);
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT godot_distrho_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                                      const GDExtensionClassLibraryPtr p_library,
                                                      GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_godot_distrho_module);
    init_obj.register_terminator(uninitialize_godot_distrho_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}
