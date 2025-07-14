#include "distrho_plugin_server_node.h"
#include "distrho_plugin_server.h"

using namespace godot;

DistrhoPluginServerNode::DistrhoPluginServerNode() {
}

DistrhoPluginServerNode::~DistrhoPluginServerNode() {
}

void DistrhoPluginServerNode::_process() {
    DistrhoPluginServer::get_singleton()->process();
}

void DistrhoPluginServerNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("process"), &DistrhoPluginServerNode::_process);
}
