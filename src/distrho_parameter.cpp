#include "distrho_parameter.h"
#include "godot_cpp/core/class_db.hpp"

using namespace godot;

VARIANT_ENUM_CAST(DistrhoParameter::DistrhoParameterHint);
VARIANT_ENUM_CAST(DistrhoParameter::DistrhoParameterDesignation);
VARIANT_ENUM_CAST(DistrhoParameter::DistrhoPredefinedPortGroupsIds);

DistrhoParameter::DistrhoParameter(DistrhoParameterHint p_hints, String p_name, String p_short_name, String p_symbol,
                                   String p_unit, String p_description, float p_default_value, float p_min_value,
                                   float p_max_value, Dictionary p_enumeration_values, int p_designation, int p_midi_cc,
                                   int p_group_id) {
    hints = p_hints;
    name = p_name;
    short_name = p_short_name;
    symbol = p_symbol;
    unit = p_unit;
    description = p_description;

    default_value = p_default_value;
    min_value = p_min_value;
    max_value = p_max_value;

    // TODO: validate dictionary type <String, float>
    enumeration_values = p_enumeration_values;
    designation = p_designation;

    midi_cc = p_midi_cc;
    group_id = p_group_id;
}

DistrhoParameter *DistrhoParameter::create(DistrhoParameterHint p_hints, String p_name, String p_short_name,
                                           String p_symbol, String p_unit, String p_description, float p_default_value,
                                           float p_min_value, float p_max_value, Dictionary p_enumeration_values,
                                           int p_designation, int p_midi_cc, int p_group_id) {
    return memnew(DistrhoParameter(p_hints, p_name, p_short_name, p_symbol, p_unit, p_description, p_default_value,
                                   p_min_value, p_max_value, p_enumeration_values, p_designation, p_midi_cc,
                                   p_group_id));
}

void DistrhoParameter::set_hints(int p_hints) {
    hints = p_hints;
}

int DistrhoParameter::get_hints() {
    return hints;
}

void DistrhoParameter::set_name(String p_name) {
    name = p_name;
}

String DistrhoParameter::get_name() {
    return name;
}

void DistrhoParameter::set_short_name(String p_short_name) {
    short_name = p_short_name;
}

String DistrhoParameter::get_short_name() {
    return short_name;
}

void DistrhoParameter::set_symbol(String p_symbol) {
    symbol = p_symbol;
}

String DistrhoParameter::get_symbol() {
    return symbol;
}

void DistrhoParameter::set_unit(String p_unit) {
    unit = p_unit;
}

String DistrhoParameter::get_unit() {
    return unit;
}

void DistrhoParameter::set_description(String p_description) {
    description = p_description;
}

String DistrhoParameter::get_description() {
    return description;
}

void DistrhoParameter::set_default_value(float p_default_value) {
    default_value = p_default_value;
}

float DistrhoParameter::get_default_value() {
    return default_value;
}

void DistrhoParameter::set_min_value(float p_min_value) {
    min_value = p_min_value;
}

float DistrhoParameter::get_min_value() {
    return min_value;
}

void DistrhoParameter::set_max_value(float p_max_value) {
    max_value = p_max_value;
}

float DistrhoParameter::get_max_value() {
    return max_value;
}

void DistrhoParameter::set_enumeration_values(Dictionary p_enumeration_values) {
    enumeration_values = p_enumeration_values.duplicate(true);
}

Dictionary DistrhoParameter::get_enumeration_values() {
    return enumeration_values;
}

void DistrhoParameter::set_designation(int p_designation) {
    designation = p_designation;
}

int DistrhoParameter::get_designation() {
    return designation;
}

void DistrhoParameter::set_midi_cc(int p_midi_cc) {
    midi_cc = p_midi_cc;
}

int DistrhoParameter::get_midi_cc() {
    return midi_cc;
}

void DistrhoParameter::set_group_id(int p_group_id) {
    group_id = p_group_id;
}

int DistrhoParameter::get_group_id() {
    return group_id;
}

DistrhoParameter::~DistrhoParameter() {
}

void DistrhoParameter::_bind_methods() {
    BIND_ENUM_CONSTANT(HINT_NONE);
    BIND_ENUM_CONSTANT(HINT_PARAMETER_IS_AUTOMATABLE);
    BIND_ENUM_CONSTANT(HINT_PARAMETER_IS_BOOLEAN);
    BIND_ENUM_CONSTANT(HINT_PARAMETER_IS_INTEGER);
    BIND_ENUM_CONSTANT(HINT_PARAMETER_IS_LOGARITHMIC);
    BIND_ENUM_CONSTANT(HINT_PARAMETER_IS_OUTPUT);
    BIND_ENUM_CONSTANT(HINT_PARAMETER_IS_TRIGGER);
    BIND_ENUM_CONSTANT(HINT_PARAMETER_IS_HIDDEN);

    BIND_ENUM_CONSTANT(PARAMETER_DESIGNATION_NONE);
    BIND_ENUM_CONSTANT(PARAMETER_DESIGNATION_BYPASS);

    BIND_ENUM_CONSTANT(PORT_GROUP_NONE);
    BIND_ENUM_CONSTANT(PORT_GROUP_MONO);
    BIND_ENUM_CONSTANT(PORT_GROUP_STEREO);

    ClassDB::bind_static_method("DistrhoParameter",
                                D_METHOD("create", "hints", "name", "short_name", "symbol", "unit", "description",
                                         "default_value", "min_value", "max_value", "enumeration_values", "designation",
                                         "midi_cc", "group_id"),
                                &DistrhoParameter::create);
}
