#ifndef GODOT_DISTRHO_UTILS_H
#define GODOT_DISTRHO_UTILS_H

#include "src/DistrhoDefines.h"

typedef struct _XDisplay Display;

typedef unsigned long XID;

typedef XID Window;

START_NAMESPACE_DISTRHO

class GodotDistrhoUtils {

public:
    static Display* get_x11_display();

    static ::Window get_x11_window(Display *display);
};


END_NAMESPACE_DISTRHO

#endif
