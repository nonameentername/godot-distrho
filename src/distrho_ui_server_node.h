#ifndef DISTRHO_UI_SERVER_NODE_H
#define DISTRHO_UI_SERVER_NODE_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class DistrhoUIServerNode : public Node {
    GDCLASS(DistrhoUIServerNode, Node);

private:
protected:
public:
    DistrhoUIServerNode();
    ~DistrhoUIServerNode();

    void _process();

    static void _bind_methods();
};
} // namespace godot

#endif
