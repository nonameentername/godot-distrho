#include "godot_distrho_utils.h"
#include <X11/Xlib.h>

START_NAMESPACE_DISTRHO

Display *GodotDistrhoUtils::get_x11_display() {
    Display *display = XOpenDisplay(NULL);
    return display;
}

Window GodotDistrhoUtils::get_x11_window(Display *display) {
    Window window = DefaultRootWindow(display);
    return window;
}

END_NAMESPACE_DISTRHO
