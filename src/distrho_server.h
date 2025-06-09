#ifndef DISTRHO_SERVER_H
#define DISTRHO_SERVER_H

#include "distrho_config.h"
#include "distrho_launcher.h"
#include "distrho_plugin_instance.h"
#include <godot_cpp/classes/node.hpp>

namespace godot {

    
class DistrhoServer : public Object {
    GDCLASS(DistrhoServer, Object);

private:
    DistrhoConfig *distrho_config;
    DistrhoPluginInstance *distrho_plugin;
	DistrhoLauncher *distrho_launcher;

protected:
    static DistrhoServer *singleton;

public:
    DistrhoServer();
    ~DistrhoServer();

    static DistrhoServer *get_singleton();

    static void _bind_methods();

    void initialize();

    DistrhoConfig *get_config();

    String get_version();
    String get_build();

	void set_distrho_launcher(DistrhoLauncher *p_distrho_launcher);
	DistrhoLauncher *get_distrho_launcher();
};
}

#endif
