#include "godot_distrho_utils.h"
#include <X11/Xlib.h>

#if defined(_WIN32)
#  include <windows.h>
#elif defined(__linux__)
#  include <unistd.h>
#  include <limits.h>
#  include <dlfcn.h>
#elif defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <limits.h>
#  include <dlfcn.h>
#endif

#include <boost/process.hpp>
#include <filesystem>

using namespace boost::process;

START_NAMESPACE_DISTRHO

Display *GodotDistrhoUtils::get_x11_display() {
    Display *display = XOpenDisplay(NULL);
    return display;
}

Window GodotDistrhoUtils::get_x11_window(Display *display) {
    Window window = DefaultRootWindow(display);
    return window;
}

std::string GodotDistrhoUtils::get_executable_path() {
#if defined(_WIN32)
    char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    return std::string(path);
#elif defined(__linux__)
    char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0';
        return std::string(path);
    }
    return {};
#elif defined(__APPLE__)
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        return std::string(path);
    }
    return {};
#else
    return {};
#endif
}

std::string GodotDistrhoUtils::get_shared_library_path() {
#if defined(_WIN32)
    HMODULE hModule = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                      GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCSTR)&getSharedLibraryPath, &hModule);
    char path[MAX_PATH];
    GetModuleFileName(hModule, path, MAX_PATH);
    return std::string(path);
#elif defined(__linux__) || defined(__APPLE__)
    Dl_info info;
    if (dladdr(reinterpret_cast<void*>(&GodotDistrhoUtils::get_shared_library_path), &info) && info.dli_fname)
        return std::string(info.dli_fname);
    return {};
#else
    return {};
#endif
}

child* GodotDistrhoUtils::launch_process(
    const std::string& p_name,
    environment p_env,
    const std::vector<std::string>& p_args
) {
    using std::filesystem::path;

    path lib_dir = path{get_shared_library_path()}.parent_path();
    path exe_dir = path{get_executable_path()}.parent_path();

    path primary_tool_path = lib_dir / path(p_name);
    path fallback_tool_path = exe_dir / path(p_name);

    std::string executable;
    if (std::filesystem::exists(primary_tool_path)) {
        executable = primary_tool_path.string();
    } else {
        executable = fallback_tool_path.string();
    }

    return new child(executable, args(p_args), env = p_env);
}

END_NAMESPACE_DISTRHO
