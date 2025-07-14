#include "distrho_ui_instance.h"

using namespace godot;

DistrhoUIInstance::DistrhoUIInstance() {
}

DistrhoUIInstance::~DistrhoUIInstance() {
}

String DistrhoUIInstance::_get_some_text() {
    if (has_method("get_some_text")) {
        return call("get_some_text");
    } else {
        return "godot-distrho";
    }
}

void DistrhoUIInstance::_bind_methods() {
}
