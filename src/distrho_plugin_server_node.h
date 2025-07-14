#ifndef DISTRHO_PLUGIN_SERVER_NODE_H
#define DISTRHO_PLUGIN_SERVER_NODE_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class DistrhoPluginServerNode : public Node {
    GDCLASS(DistrhoPluginServerNode, Node);

private:
protected:
public:
    DistrhoPluginServerNode();
    ~DistrhoPluginServerNode();

    void _process();

    static void _bind_methods();
};
} // namespace godot

#endif
