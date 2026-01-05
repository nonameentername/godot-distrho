#ifndef DISTRHO_LAUNCHER_H
#define DISTRHO_LAUNCHER_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/window.hpp>

namespace godot {

class DistrhoLauncher : public Node2D {
    GDCLASS(DistrhoLauncher, Node2D);

private:
protected:
public:
    DistrhoLauncher();
    ~DistrhoLauncher();

    void _ready() override;
    void initialize();
    void load_scene(String p_scene, bool p_visible = true);

    static void _bind_methods();
};
} // namespace godot

#endif
