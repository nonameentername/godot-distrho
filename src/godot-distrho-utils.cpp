#include "godot-distrho-utils.h"
#include <X11/Xlib.h>

START_NAMESPACE_DISTRHO

Display *GodotDistrhoUtils::get_x11_display() {
    Display *display = XOpenDisplay(NULL);
    return display;
}

END_NAMESPACE_DISTRHO
