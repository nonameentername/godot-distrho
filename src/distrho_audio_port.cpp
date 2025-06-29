#include "distrho_audio_port.h"
#include "godot_cpp/core/class_db.hpp"

using namespace godot;

VARIANT_ENUM_CAST(DistrhoAudioPort::DistrhoAudioPortHint);
VARIANT_ENUM_CAST(DistrhoAudioPort::DistrhoPredefinedPortGroupsIds);

DistrhoAudioPort::DistrhoAudioPort(DistrhoAudioPortHint p_hints, String p_name, String p_symbol, int p_group_id) {
    hints = p_hints;
    name = p_name;
    symbol = p_symbol;
    group_id = p_group_id;
}

DistrhoAudioPort* DistrhoAudioPort::create(DistrhoAudioPortHint p_hints, String p_name, String p_symbol, int p_group_id) {
    return memnew(DistrhoAudioPort(p_hints, p_name, p_symbol, p_group_id));
}

int DistrhoAudioPort::get_hints() {
    return hints;
}

String DistrhoAudioPort::get_name() {
    return name;
}

String DistrhoAudioPort::get_symbol() {
    return symbol;
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

    ClassDB::bind_static_method("DistrhoAudioPort", D_METHOD("create", "hints", "name", "symbol", "group_id"), &DistrhoAudioPort::create);
}
