#ifndef DISTRHO_PLUGIN_INSTANCE_H
#define DISTRHO_PLUGIN_INSTANCE_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class DistrhoPluginInstance : public Node {
    GDCLASS(DistrhoPluginInstance, Node);

private:

protected:

public:
    DistrhoPluginInstance();
    ~DistrhoPluginInstance();

    String _get_label();
    String _get_description();
    String _get_maker();
    String _get_homepage();
    String _get_license();
    String _get_version();
    String _get_unique_id();

    static void _bind_methods();
};
} // namespace godot

#endif
