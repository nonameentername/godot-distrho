#ifndef DISTRHO_LAUNCHER_H
#define DISTRHO_LAUNCHER_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/window.hpp>


namespace godot {

class DistrhoLauncher : public Node2D {
    GDCLASS(DistrhoLauncher, Node2D);

private:
    Window *window;

protected:

public:
    DistrhoLauncher();
    ~DistrhoLauncher();

	void _ready() override;
    void initialize();

	void show_ui();
	void hide_ui();

    static void _bind_methods();
};
} // namespace godot

#endif
