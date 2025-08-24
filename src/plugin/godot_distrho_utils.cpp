#include "godot_distrho_utils.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <dlfcn.h>
#include <limits.h>
#include <mach-o/dyld.h>
#endif

#include <boost/process.hpp>
#include <filesystem>

using namespace boost::process;

START_NAMESPACE_DISTRHO

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
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCSTR)&getSharedLibraryPath, &hModule);
    char path[MAX_PATH];
    GetModuleFileName(hModule, path, MAX_PATH);
    return std::string(path);
#elif defined(__linux__) || defined(__APPLE__)
    Dl_info info;
    if (dladdr(reinterpret_cast<void *>(&GodotDistrhoUtils::get_shared_library_path), &info) && info.dli_fname)
        return std::string(info.dli_fname);
    return {};
#else
    return {};
#endif
}

child *GodotDistrhoUtils::launch_process(const std::string &p_name, environment p_env,
                                         const std::vector<std::string> &p_args) {
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

    return new child(executable, boost::process::std_out > boost::process::null,
                     boost::process::std_err > boost::process::null, args(p_args), env = p_env);
}

std::string GodotDistrhoUtils::find_godot_package() {
    namespace fs = std::filesystem;

    std::vector<fs::path> search_dirs = {fs::path(get_shared_library_path()).parent_path(),
                                         fs::path(get_executable_path()).parent_path()};

    for (const fs::path &dir : search_dirs) {
        if (!fs::exists(dir) || !fs::is_directory(dir))
            continue;

        for (const auto &entry : fs::directory_iterator(dir)) {
            if (entry.path().extension() == ".pck") {
                return entry.path().string();
            }
        }
    }

    return "";
}

END_NAMESPACE_DISTRHO
