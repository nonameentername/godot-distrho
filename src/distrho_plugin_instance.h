#ifndef DISTRHO_PLUGIN_INSTANCE_H
#define DISTRHO_PLUGIN_INSTANCE_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class DistrhoPluginInstance : public Object {
    GDCLASS(DistrhoPluginInstance, Object);

private:

protected:

public:
    DistrhoPluginInstance();
    ~DistrhoPluginInstance();

    static void _bind_methods();
};
} // namespace godot

#endif
