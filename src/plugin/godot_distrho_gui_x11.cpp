#include "godot_distrho_gui_x11.h"
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>

void x11_disable_input(uint64_t win_id) {
	// 1. Get the X11 Display connection
	Display* dpy = XOpenDisplay(nullptr); 
    
    // 2. Get the DPF window handle (The "Glass" we want to punch a hole in)
    Window host_window = (Window)win_id;

    // 3. Create a 'Map of Nothing'
    // This region is empty. It has 0 width and 0 height.
    Region region = XCreateRegion();

    // 4. Punch the hole
    // We tell the X Server to take the host_window's Input Shape 
    // and REPLACE it (ShapeSet) with our 'Map of Nothing'.
    XShapeCombineRegion(dpy, host_window, ShapeInput, 0, 0, region, ShapeSet);

    // 5. Cleanup
    XDestroyRegion(region); // Free the map memory
    XCloseDisplay(dpy);     // Close our temporary connection
}

void get_host_position(uint64_t window_id, int& outX, int& outY) {
    // 1. Get the X11 Display handle from DPF
    // (If getApp() is missing, use XOpenDisplay(nullptr))
    Display* dpy = XOpenDisplay(nullptr);
    if (!dpy) return;

    // 2. Get the DPF Window ID
    Window hostWin = (Window)window_id;
    
    // 3. Get the Root Window (the Desktop)
    Window rootWin = DefaultRootWindow(dpy);

    // 4. Variables to hold the result
    Window child; 
    int screenX, screenY;

    // 5. Translate (0,0) of hostWin to screen coordinates
    XTranslateCoordinates(dpy, hostWin, rootWin, 0, 0, &screenX, &screenY, &child);

    outX = screenX;
    outY = screenY;

    XCloseDisplay(dpy);
}

void set_always_on_top(Display* dpy, Window win) {
    Atom wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
    Atom state_above = XInternAtom(dpy, "_NET_WM_STATE_ABOVE", False);

    XEvent xev;
    xev.type = ClientMessage;
    xev.xclient.window = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
    xev.xclient.data.l[1] = state_above;
    xev.xclient.data.l[2] = 0;
    xev.xclient.data.l[3] = 1; // Source indication

    XSendEvent(dpy, DefaultRootWindow(dpy), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    XFlush(dpy);
}

void set_godot_position(uint64_t godot_window_id, int x, int y) {
	Display* dpy = XOpenDisplay(nullptr);
	Window godot_xid = (Window)godot_window_id; 

	if (dpy && godot_xid) {
		// 3. Move the window "remotely"
		XMoveWindow(dpy, godot_xid, x, y);
		XRaiseWindow(dpy, (Window)godot_xid);

		//set_always_on_top(dpy, godot_window_id);

		XFlush(dpy);
		XCloseDisplay(dpy);
	}
}

void set_godot_transient(uint64_t godot_xid, uint64_t dpf_xid) {
    Display* dpy = XOpenDisplay(nullptr);
    if (!dpy) return;

    // Set the transient hint
    XSetTransientForHint(dpy, (Window)godot_xid, (Window)dpf_xid);
    
    // Force the Window Manager to update the stacking order immediately
    XRaiseWindow(dpy, (Window)godot_xid);
    
    XFlush(dpy);
    XCloseDisplay(dpy);
}

void update_godot_window(uint64_t host_window_id, uint64_t godot_window_id) {
	Display* dpy = XOpenDisplay(nullptr);
	Window hostWin = (Window)host_window_id;
	Window godotWin = (Window)godot_window_id;

	// 1. Check the Parent's state
	XWindowAttributes attr;
	XGetWindowAttributes(dpy, hostWin, &attr);

	// 2. Mirror the visibility
	if (attr.map_state == IsUnmapped) {
		// If Parent is minimized or closed, hide Godot
		XUnmapWindow(dpy, godotWin);
	} else {
		// If Parent is visible, ensure Godot is visible AND on top
		XMapWindow(dpy, godotWin);

		// This 'Raise' only happens while the parent is active
		XRaiseWindow(dpy, godotWin);
	}

	XFlush(dpy);
	XCloseDisplay(dpy);
}

void get_godot_size(uint64_t godot_window_id, int &out_width, int &out_height) {
    // 1. Open a connection to the X Server
    Display* dpy = XOpenDisplay(nullptr);
    if (!dpy) return;

    // 2. Prepare the attributes structure
    XWindowAttributes attrs;
    
    // 3. Query the server for the window's current state
    if (XGetWindowAttributes(dpy, (Window)godot_window_id, &attrs)) {
        out_width = attrs.width;
        out_height = attrs.height;
    }

    // 4. Clean up
    XCloseDisplay(dpy);
}

void set_host_size(uint64_t window_id, int width, int height) {
    Display* dpy = XOpenDisplay(nullptr);
    Window hostWin = (Window)window_id;
    XResizeWindow(dpy, hostWin, width, height);
    XFlush(dpy);
    XCloseDisplay(dpy);
}
