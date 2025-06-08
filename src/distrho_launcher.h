#ifndef DISTRHO_LAUNCHER_H
#define DISTRHO_LAUNCHER_H

#include <godot_cpp/classes/node.hpp>


namespace godot {

class DistrhoLauncher : public Node {
    GDCLASS(DistrhoLauncher, Node);

private:

protected:

public:
    DistrhoLauncher();
    ~DistrhoLauncher();

    void initialize();

    static void _bind_methods();
};
} // namespace godot

#endif
