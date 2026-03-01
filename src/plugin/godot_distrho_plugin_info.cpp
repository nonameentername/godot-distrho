#include <boost/json.hpp>
#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <boost/json/serialize.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "godot_distrho_plugin_info.h"
#include "DistrhoUtils.hpp"
#include "godot_distrho_utils.h"

START_NAMESPACE_DISTRHO


DistrhoPluginInfo::DistrhoPluginInfo() {
}

DistrhoPluginInfo::~DistrhoPluginInfo() {
}

std::vector<std::string> split_string_stream(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(input);

    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

uint32_t DistrhoPluginInfo::get_version() {
	std::vector<std::string> version_tokens = split_string_stream(version, '.');

	if (version_tokens.size() == 3) {
		return d_version(std::stoi(version_tokens[0]), std::stoi(version_tokens[1]), std::stoi(version_tokens[2]));
	} else {
		return d_version(0, 0, 1);
	}
}

uint32_t DistrhoPluginInfo::get_unique_id() {
	if (unique_id.size() >= 4) {
		return d_cconst(unique_id[0], unique_id[1], unique_id[2], unique_id[3]);
	} else {
		return d_cconst('n', 'o', 'n', 'e');
	}
}

void DistrhoPluginInfo::get_parameter(int p_index, Parameter &p_parameter) {
	p_parameter.hints = parameters[p_index].hints;
	p_parameter.name = parameters[p_index].name.c_str();
	p_parameter.symbol = parameters[p_index].symbol.c_str();
	p_parameter.unit = parameters[p_index].unit.c_str();
	p_parameter.description = parameters[p_index].description.c_str();

	p_parameter.ranges.def = parameters[p_index].default_value;
	p_parameter.ranges.min = parameters[p_index].min_value;
	p_parameter.ranges.max = parameters[p_index].max_value;

	int enumeration_count = parameters[p_index].enumeration_values.size();
	p_parameter.enumValues.count = enumeration_count;
	p_parameter.enumValues.values = new ParameterEnumerationValue[enumeration_count];

	int enum_index = 0;	

	for (const auto& enumeration_value : parameters[p_index].enumeration_values) {
		p_parameter.enumValues.values[enum_index].label = enumeration_value.first.c_str();
		p_parameter.enumValues.values[enum_index].value = enumeration_value.second;
		enum_index++;
	}

	p_parameter.designation = (ParameterDesignation)parameters[p_index].designation;

	p_parameter.midiCC = parameters[p_index].midi_cc;
	p_parameter.groupId = parameters[p_index].group_id;
}

void DistrhoPluginInfo::get_input_port(int p_index, AudioPort &p_port) {
	p_port.groupId = input_ports[p_index].group_id;
	p_port.name = input_ports[p_index].name.c_str();
	p_port.symbol = input_ports[p_index].symbol.c_str();
	p_port.groupId = input_ports[p_index].group_id;
}

void DistrhoPluginInfo::get_output_port(int p_index, AudioPort &p_port) {
	p_port.groupId = output_ports[p_index].group_id;
	p_port.name = output_ports[p_index].name.c_str();
	p_port.symbol = output_ports[p_index].symbol.c_str();
	p_port.groupId = output_ports[p_index].group_id;
}

void DistrhoPluginInfo::load() {
	std::ifstream file(GodotDistrhoUtils::get_full_path(filename));
    std::stringstream buffer;
    buffer << file.rdbuf();

    boost::system::error_code error;
    boost::json::value json_value = boost::json::parse(buffer.str(), error);
    
    if (error) {
        printf("Failed to parse JSON plugin info: %s\n", error.message().c_str());
		return;
    }

    boost::json::object& obj = json_value.as_object();

	if (obj.contains("uri")) {
		uri = boost::json::value_to<std::string>(obj.at("uri"));
	}

	if (obj.contains("label")) {
		label = boost::json::value_to<std::string>(obj.at("label"));
	}

	if (obj.contains("description")) {
		description = boost::json::value_to<std::string>(obj.at("description"));
	}

	if (obj.contains("maker")) {
		maker = boost::json::value_to<std::string>(obj.at("maker"));
	}

	if (obj.contains("homepage")) {
		homepage = boost::json::value_to<std::string>(obj.at("homepage"));
	}

	if (obj.contains("license")) {
		license = boost::json::value_to<std::string>(obj.at("license"));
	}

	if (obj.contains("version")) {
		version = boost::json::value_to<std::string>(obj.at("version"));
	}

	if (obj.contains("unique_id")) {
		unique_id = boost::json::value_to<std::string>(obj.at("unique_id"));
	}


    if (obj.contains("parameters")) {
        boost::json::array& parameters_array = obj.at("parameters").as_array();
        parameters.resize(parameters_array.size());

        for (size_t i = 0; i < parameters_array.size(); ++i) {
            boost::json::object& parameter = parameters_array[i].as_object();

            if (parameter.contains("hints")) {
                parameters[i].hints = boost::json::value_to<int>(parameter.at("hints"));
            }

            if (parameter.contains("name")) {
                parameters[i].name = boost::json::value_to<std::string>(parameter.at("name"));
            }

            if (parameter.contains("short_name")) {
                parameters[i].short_name = boost::json::value_to<std::string>(parameter.at("short_name"));
            }

            if (parameter.contains("symbol")) {
                parameters[i].symbol = boost::json::value_to<std::string>(parameter.at("symbol"));
            }

            if (parameter.contains("unit")) {
                parameters[i].unit = boost::json::value_to<std::string>(parameter.at("unit"));
            }

            if (parameter.contains("description")) {
                parameters[i].description = boost::json::value_to<std::string>(parameter.at("description"));
            }

            if (parameter.contains("default_value")) {
                parameters[i].default_value = boost::json::value_to<float>(parameter.at("default_value"));
            }

            if (parameter.contains("min_value")) {
                parameters[i].min_value = boost::json::value_to<float>(parameter.at("min_value"));
            }

            if (parameter.contains("max_value")) {
                parameters[i].max_value = boost::json::value_to<float>(parameter.at("max_value"));
            }

            if (parameter.contains("enumeration_values")) {
                boost::json::object& enumeration_values = parameter.at("enumeration_values").as_object();

                for (const auto& enumeration_value : enumeration_values) {
                    parameters[i].enumeration_values.insert_or_assign(
                           enumeration_value.key(), boost::json::value_to<int>(enumeration_value.value()));
                }
            }

            if (parameter.contains("designation")) {
                parameters[i].designation = boost::json::value_to<int>(parameter.at("designation"));
            }

            if (parameter.contains("midi_cc")) {
                parameters[i].midi_cc = boost::json::value_to<int>(parameter.at("midi_cc"));
            }

            if (parameter.contains("group_id")) {
                parameters[i].group_id = boost::json::value_to<int>(parameter.at("group_id"));
            }
        }
    }

    if (obj.contains("input_ports")) {
        boost::json::array& input_ports_array = obj.at("input_ports").as_array();
        input_ports.resize(input_ports_array.size());

        for (size_t i = 0; i < input_ports_array.size(); ++i) {
            boost::json::object& input_port = input_ports_array[i].as_object();

            if (input_port.contains("hints")) {
                input_ports[i].hints = boost::json::value_to<int>(input_port.at("hints"));
            }

            if (input_port.contains("name")) {
                input_ports[i].name = boost::json::value_to<std::string>(input_port.at("name"));
            }

            if (input_port.contains("symbol")) {
                input_ports[i].symbol = boost::json::value_to<std::string>(input_port.at("symbol"));
            }

            if (input_port.contains("group_id")) {
                input_ports[i].group_id = boost::json::value_to<int>(input_port.at("group_id"));
            }
        }
    }

    if (obj.contains("output_ports")) {
        boost::json::array& output_ports_array = obj.at("output_ports").as_array();
        output_ports.resize(output_ports_array.size());

        for (size_t i = 0; i < output_ports_array.size(); ++i) {
            boost::json::object& output_port = output_ports_array[i].as_object();

            if (output_port.contains("hints")) {
                output_ports[i].hints = boost::json::value_to<int>(output_port.at("hints"));
            }

            if (output_port.contains("name")) {
                output_ports[i].name = boost::json::value_to<std::string>(output_port.at("name"));
            }

            if (output_port.contains("symbol")) {
                output_ports[i].symbol = boost::json::value_to<std::string>(output_port.at("symbol"));
            }

            if (output_port.contains("group_id")) {
                output_ports[i].group_id = boost::json::value_to<int>(output_port.at("group_id"));
            }
        }
    }

    if (obj.contains("state_values")) {
        boost::json::object& state_values_array = obj.at("state_values").as_object();

        for (const auto& state_value : state_values_array) {
            state_values.insert_or_assign(state_value.key(), boost::json::value_to<std::string>(state_value.value()));
        }
    }
}

END_NAMESPACE_DISTRHO
