#include "distrho_ui_server_node.h"
#include "distrho_ui_server.h"

using namespace godot;

DistrhoUIServerNode::DistrhoUIServerNode() {
}

DistrhoUIServerNode::~DistrhoUIServerNode() {
}

void DistrhoUIServerNode::_process() {
    DistrhoUIServer::get_singleton()->process();
}

void DistrhoUIServerNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("process"), &DistrhoUIServerNode::_process);
}
