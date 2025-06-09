#include "distrho_server.h"
#include "distrho_config.h"
#include "distrho_plugin_instance.h"
#include "godot_cpp/core/memory.hpp"
#include "version_generated.gen.h"

using namespace godot;

DistrhoServer *DistrhoServer::singleton = NULL;

DistrhoServer::DistrhoServer() {
    distrho_config = memnew(DistrhoConfig);
    distrho_plugin = memnew(DistrhoPluginInstance);
    singleton = this;
}

DistrhoServer::~DistrhoServer() {
    singleton = NULL;
}

DistrhoServer *DistrhoServer::get_singleton() {
    return singleton;
}

void DistrhoServer::initialize() {
}

DistrhoConfig *DistrhoServer::get_config() {
    return distrho_config;
}

String DistrhoServer::get_version() {
    return GODOT_DISTRHO_VERSION;
}

String DistrhoServer::get_build() {
    return GODOT_DISTRHO_BUILD;
}

void DistrhoServer::set_distrho_launcher(DistrhoLauncher *p_distrho_launcher) {
	distrho_launcher = p_distrho_launcher;
}

DistrhoLauncher *DistrhoServer::get_distrho_launcher() {
	return distrho_launcher;
}

void DistrhoServer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize"), &DistrhoServer::initialize);

    ClassDB::bind_method(D_METHOD("get_config"), &DistrhoServer::get_config);

    ClassDB::bind_method(D_METHOD("get_version"), &DistrhoServer::get_version);
    ClassDB::bind_method(D_METHOD("get_build"), &DistrhoServer::get_build);
}
