#ifndef GODOT_DISTRHO_DYNAMIC_INFO_H
#define GODOT_DISTRHO_DYNAMIC_INFO_H

#include "DistrhoPlugin.hpp"
#include <string>

START_NAMESPACE_DISTRHO

class DistrhoPluginInfo;

class GodotDistrhoDynamicInfo {
private:
    DistrhoPluginInfo *plugin_info;
    String plugin_uri = String(DISTRHO_PLUGIN_URI);
    String ui_uri = String(DISTRHO_PLUGIN_URI "#DPF_UI");
    int number_inputs = DISTRHO_PLUGIN_NUM_INPUTS;
    int number_outputs = DISTRHO_PLUGIN_NUM_OUTPUTS;


public:
    GodotDistrhoDynamicInfo();
    ~GodotDistrhoDynamicInfo();

    String get_plugin_uri();
    String get_ui_uri();

    const char* get_plugin_uri_cstr();
    const char* get_ui_uri_cstr();

    const int get_number_inputs();
    const int get_number_outputs();

    static GodotDistrhoDynamicInfo &get_instance();
};

END_NAMESPACE_DISTRHO

#endif
