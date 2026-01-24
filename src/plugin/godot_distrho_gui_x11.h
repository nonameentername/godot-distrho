#ifndef GODOT_DISTRHO_GUI_X11_H
#define GODOT_DISTRHO_GUI_X11_H

#include <X11/X.h>
#include <cstdint>

void x11_disable_input(uint64_t win_id);
void get_host_position(uint64_t window_id, int& outX, int& outY);
void set_godot_position(uint64_t godot_window_id, int x, int y);
void set_godot_transient(uint64_t godot_xid, uint64_t dpf_xid);
void update_godot_window(uint64_t window_id, uint64_t godot_window_id);
void get_godot_size(uint64_t godot_window_id, int &out_width, int &out_height);
void set_host_size(uint64_t window_id, int width, int height);

#endif
