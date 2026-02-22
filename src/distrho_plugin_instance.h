#ifndef DISTRHO_PLUGIN_INSTANCE_H
#define DISTRHO_PLUGIN_INSTANCE_H

#include "distrho_audio_port.h"
#include "distrho_parameter.h"
#include "godot_cpp/variant/dictionary.hpp"
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

    Vector<Ref<DistrhoParameter>> _get_parameters();
    Vector<Ref<DistrhoAudioPort>> _get_input_ports();
    Vector<Ref<DistrhoAudioPort>> _get_output_ports();

    Dictionary _get_state_values();

    Dictionary get_json();

    static void _bind_methods();
};
} // namespace godot

#endif
