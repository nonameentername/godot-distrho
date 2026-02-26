#include <string>

#include "godot_distrho_dynamic_info.h"
#include "godot_distrho_plugin_info.h"

START_NAMESPACE_DISTRHO


GodotDistrhoDynamicInfo::GodotDistrhoDynamicInfo() {
    plugin_info = new DistrhoPluginInfo;
    plugin_info->load();

    plugin_uri = String(plugin_info->uri.c_str());

    std::string temp = plugin_info->uri + std::string("#DPF_UI");
    ui_uri = String(temp.c_str());

    number_inputs = plugin_info->input_ports.size();
    number_outputs = plugin_info->output_ports.size();
}

GodotDistrhoDynamicInfo::~GodotDistrhoDynamicInfo() {
    delete plugin_info;
}

GodotDistrhoDynamicInfo &GodotDistrhoDynamicInfo::get_instance() {
    static GodotDistrhoDynamicInfo instance;
    return instance;
}

String GodotDistrhoDynamicInfo::get_plugin_uri() {
    return plugin_uri;
}

String GodotDistrhoDynamicInfo::get_ui_uri() {
    return ui_uri;
}

const char* GodotDistrhoDynamicInfo::get_plugin_uri_cstr() {
    return plugin_uri;
}

const char* GodotDistrhoDynamicInfo::get_ui_uri_cstr() {
    return ui_uri;
}

const int GodotDistrhoDynamicInfo::get_number_inputs() {
    return number_inputs;
}

const int GodotDistrhoDynamicInfo::get_number_outputs() {
    return number_outputs;
}

END_NAMESPACE_DISTRHO
