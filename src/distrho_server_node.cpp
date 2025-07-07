#include "distrho_server_node.h"
#include "distrho_server.h"

using namespace godot;

DistrhoServerNode::DistrhoServerNode() {
}

DistrhoServerNode::~DistrhoServerNode() {
}

void DistrhoServerNode::_process() {
    DistrhoServer::get_singleton()->process();
}

void DistrhoServerNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("process"), &DistrhoServerNode::_process);
}
