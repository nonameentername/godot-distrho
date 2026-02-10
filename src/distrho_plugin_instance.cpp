#include "distrho_plugin_instance.h"
#include "distrho_audio_port.h"
#include "godot_cpp/templates/vector.hpp"
#include "godot_cpp/variant/dictionary.hpp"

using namespace godot;

DistrhoPluginInstance::DistrhoPluginInstance() {
}

DistrhoPluginInstance::~DistrhoPluginInstance() {
}

String DistrhoPluginInstance::_get_label() {
    if (has_method("get_label")) {
        return call("get_label");
    } else {
        return "godot-distrho";
    }
}

String DistrhoPluginInstance::_get_description() {
    if (has_method("get_description")) {
        return call("get_description");
    } else {
        return "Godot DISTRHO plugin";
    }
}

String DistrhoPluginInstance::_get_maker() {
    if (has_method("get_maker")) {
        return call("get_maker");
    } else {
        return "godot-distrho";
    }
}

String DistrhoPluginInstance::_get_homepage() {
    if (has_method("get_homepage")) {
        return call("get_homepage");
    } else {
        return "https://github.com/nonameentername/godot-distrho";
    }
}

String DistrhoPluginInstance::_get_license() {
    if (has_method("get_license")) {
        return call("get_license");
    } else {
        return "MIT";
    }
}

String DistrhoPluginInstance::_get_version() {
    if (has_method("get_version")) {
        return call("get_version");
    } else {
        return "0.0.1";
    }
}

String DistrhoPluginInstance::_get_unique_id() {
    if (has_method("get_unique_id")) {
        return call("get_unique_id");
    } else {
        return "godt";
    }
}

Vector<Ref<DistrhoParameter>> DistrhoPluginInstance::_get_parameters() {
    Vector<Ref<DistrhoParameter>> result;

    if (has_method("get_parameters")) {
        Variant v = call("get_parameters");
        if (v.get_type() == Variant::ARRAY) {
            Array array = v;
            for (int i = 0; i < array.size(); i++) {
                Ref<DistrhoParameter> parameter = array[i];
                if (parameter.is_valid()) {
                    result.push_back(parameter);
                }
            }
        }
    }

    return result;
}

Vector<Ref<DistrhoAudioPort>> DistrhoPluginInstance::_get_input_ports() {
    Vector<Ref<DistrhoAudioPort>> result;

    if (has_method("get_input_ports")) {
        Variant v = call("get_input_ports");
        if (v.get_type() == Variant::ARRAY) {
            Array array = v;
            for (int i = 0; i < array.size(); i++) {
                Ref<DistrhoAudioPort> port = array[i];
                if (port.is_valid()) {
                    result.push_back(port);
                }
            }
        }
    }

    return result;
}

Vector<Ref<DistrhoAudioPort>> DistrhoPluginInstance::_get_output_ports() {
    Vector<Ref<DistrhoAudioPort>> result;

    if (has_method("get_output_ports")) {
        Variant v = call("get_output_ports");
        if (v.get_type() == Variant::ARRAY) {
            Array array = v;
            for (int i = 0; i < array.size(); i++) {
                Ref<DistrhoAudioPort> port = array[i];
                if (port.is_valid()) {
                    result.push_back(port);
                }
            }
        }
    }

    return result;
}


Dictionary DistrhoPluginInstance::_get_state_values() {
    Dictionary result;

    if (has_method("get_state_values")) {
        Variant v = call("get_state_values");
        if (v.get_type() == Variant::DICTIONARY) {
            Dictionary dictionary = v;

            for (int i = 0; i < dictionary.size(); i++) {
                String key = dictionary.keys().get(i);
                String value = dictionary.values().get(i);
                if (!key.is_empty() && !value.is_empty()) {
                    result[key] = value;
                }
            }
        }
    }

    return result;
}

void DistrhoPluginInstance::_bind_methods() {
}
