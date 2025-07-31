#include "distrho_audio_port.h"
#include "godot_cpp/core/class_db.hpp"

using namespace godot;

VARIANT_ENUM_CAST(DistrhoAudioPort::DistrhoAudioPortHint);
VARIANT_ENUM_CAST(DistrhoAudioPort::DistrhoPredefinedPortGroupsIds);

DistrhoAudioPort::DistrhoAudioPort() {
}

void DistrhoAudioPort::set_hints(int p_hints) {
    hints = p_hints;
}

int DistrhoAudioPort::get_hints() {
    return hints;
}

void DistrhoAudioPort::set_name(String p_name) {
    name = p_name;
}

String DistrhoAudioPort::get_name() {
    return name;
}

void DistrhoAudioPort::set_symbol(String p_symbol) {
    symbol = p_symbol;
}

String DistrhoAudioPort::get_symbol() {
    return symbol;
}

void DistrhoAudioPort::set_group_id(int p_group_id) {
    group_id = p_group_id;
}

int DistrhoAudioPort::get_group_id() {
    return group_id;
}

DistrhoAudioPort::~DistrhoAudioPort() {
}

void DistrhoAudioPort::_bind_methods() {
    BIND_ENUM_CONSTANT(HINT_NONE);
    BIND_ENUM_CONSTANT(HINT_AUDIO_PORT_IS_CV);
    BIND_ENUM_CONSTANT(HINT_AUDIO_PORT_IS_SIDECHAIN);
    BIND_ENUM_CONSTANT(HINT_CV_PORT_HAS_BIPOLAR_RANGE);
    BIND_ENUM_CONSTANT(HINT_CV_PORT_HAS_NEGATIVE_UNIPOLAR_RANGE);
    BIND_ENUM_CONSTANT(HINT_CV_PORT_HAS_POSITIVE_UNIPOLAR_RANGE);
    BIND_ENUM_CONSTANT(HINT_CV_PORT_HAS_SCALED_RANGE);
    BIND_ENUM_CONSTANT(HINT_CV_PORT_IS_OPTIONAL);

    BIND_ENUM_CONSTANT(PORT_GROUP_NONE);
    BIND_ENUM_CONSTANT(PORT_GROUP_MONO);
    BIND_ENUM_CONSTANT(PORT_GROUP_STEREO);
}
