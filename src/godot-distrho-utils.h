#ifndef GODOT_DISTRHO_UTILS_H
#define GODOT_DISTRHO_UTILS_H

#include "src/DistrhoDefines.h"

typedef struct _XDisplay Display;

START_NAMESPACE_DISTRHO

class GodotDistrhoUtils {

public:
    static Display* get_x11_display();

};


END_NAMESPACE_DISTRHO

#endif
