#ifndef DISTRHO_UI_INSTANCE_H
#define DISTRHO_UI_INSTANCE_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class DistrhoUIInstance : public Node {
    GDCLASS(DistrhoUIInstance, Node);

private:
protected:
public:
    DistrhoUIInstance();
    ~DistrhoUIInstance();

    // TODO: delete once real methods are implemented
    String _get_some_text();

    static void _bind_methods();
};
} // namespace godot

#endif
