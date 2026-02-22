#ifndef GODOT_DISTRHO_PLUGIN_INFO_H
#define GODOT_DISTRHO_PLUGIN_INFO_H

#include "DistrhoPlugin.hpp"

#include <string>
#include <map>
#include <vector>

START_NAMESPACE_DISTRHO

struct DistrhoParameter {
    int hints;
    std::string name;
    std::string short_name;
    std::string symbol;
    std::string unit;
    std::string description;

    float default_value;
    float min_value;
    float max_value;

    std::map<std::string, int> enumeration_values;
    int designation;

    int midi_cc;
    int group_id;
};

struct DistrhoAudioPort {
    int hints;
    std::string name;
    std::string symbol;
    int group_id;
};

class DistrhoPluginInfo {
private:
    std::string filename = "distrho_plugin_info.json";

public:
    std::string label = "godot-distrho";
    std::string description = "Godot DISTRHO plugin";
    std::string maker = "godot-distrho";
    std::string homepage = "https://github.com/nonameentername/godot-distrho";
    std::string license = "MIT";
    std::string version = "0.0.1";
    std::string unique_id = "godt";

    std::vector<DistrhoParameter> parameters;
    std::vector<DistrhoAudioPort> input_ports;
    std::vector<DistrhoAudioPort> output_ports;

    std::map<std::string, std::string> state_values;

    DistrhoPluginInfo();
    ~DistrhoPluginInfo();

    uint32_t get_version();
    uint32_t get_unique_id();

    void get_parameter(int p_index, Parameter &p_parameter);
    void get_input_port(int p_index, AudioPort &p_port);
    void get_output_port(int p_index, AudioPort &p_port);

    void load();
};

END_NAMESPACE_DISTRHO

#endif
