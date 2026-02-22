#include "distrho_plugin_instance.h"
#include "distrho_audio_port.h"
#include "distrho_parameter.h"
#include "godot_cpp/templates/vector.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/variant.hpp"

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


Dictionary DistrhoPluginInstance::get_json() {
    Dictionary result;
    Array parameters;
    Array input_ports;
    Array output_ports;

    for (const Ref<DistrhoParameter>& p : _get_parameters()) {
        Dictionary parameter;
        parameter["hints"] = p->get_hints();
        parameter["name"] = p->get_name();
        parameter["short_name"] = p->get_short_name();
        parameter["symbol"] = p->get_symbol();
        parameter["unit"] = p->get_unit();
        parameter["description"] = p->get_description();
        parameter["default_value"] = p->get_default_value();
        parameter["min_value"] = p->get_min_value();
        parameter["max_value"] = p->get_max_value();
        parameter["enumeration_values"] = p->get_enumeration_values();
        parameter["designation"] = p->get_designation();
        parameter["midi_cc"] = p->get_midi_cc();
        parameter["group_id"] = p->get_group_id();

        parameters.append(parameter);
    }

    for (const Ref<DistrhoAudioPort>& p : _get_input_ports()) {
        Dictionary input_port;
        input_port["hints"] = p->get_hints();
        input_port["name"] = p->get_name();
        input_port["symbol"] = p->get_symbol();
        input_port["group_id"] = p->get_group_id();

        input_ports.append(input_port);
    }

    for (const Ref<DistrhoAudioPort>& p : _get_output_ports()) {
        Dictionary output_port;
        output_port["hints"] = p->get_hints();
        output_port["name"] = p->get_name();
        output_port["symbol"] = p->get_symbol();
        output_port["group_id"] = p->get_group_id();

        output_ports.append(output_port);
    }

    result["label"] = _get_label();
    result["description"] = _get_description();
    result["maker"] = _get_maker();
    result["homepage"] = _get_homepage();
    result["license"] = _get_license();
    result["version"] = _get_version();
    result["unique_id"] = _get_unique_id();
    result["parameters"] = parameters;
    result["input_ports"] = input_ports;
    result["output_ports"] = output_ports;
    result["state_values"] = _get_state_values();;

    return result;
}

void DistrhoPluginInstance::_bind_methods() {
}
