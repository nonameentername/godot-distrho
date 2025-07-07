#ifndef DISTRHO_SERVER_NODE_H
#define DISTRHO_SERVER_NODE_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class DistrhoServerNode : public Node {
    GDCLASS(DistrhoServerNode, Node);

private:
protected:
public:
    DistrhoServerNode();
    ~DistrhoServerNode();

    void _process();

    static void _bind_methods();
};
} // namespace godot

#endif
