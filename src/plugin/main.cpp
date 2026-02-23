#include "distrho_common.h"
//#include "godot_cpp/classes/display_server.hpp"
//#include "godot_cpp/variant/dictionary.hpp"
#include "godot_distrho_utils.h"
#include "libgodot_distrho.h"

#include <dirent.h>
#include <string>
#include <vector>

#if defined(_WIN32)
#elif defined(__APPLE__)
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <thread>
#else
#include <sys/prctl.h>
#endif


extern LibGodot libgodot;

USE_NAMESPACE_DISTRHO

#if defined(__APPLE__)

static int watch_parent_with_kqueue(pid_t parent_pid) {
    int kq = kqueue();

	if (kq == -1) {
		return -1;
	}

    struct kevent ev;

    EV_SET(&ev,
           parent_pid,
           EVFILT_PROC,
           EV_ADD | EV_ENABLE | EV_ONESHOT,
           NOTE_EXIT,
           0,
           nullptr);

    if (kevent(kq, &ev, 1, nullptr, 0, nullptr) == -1) {
        close(kq);
        return -1;
    }

    if (getppid() != parent_pid) {
        close(kq);
        raise(SIGTERM);
        return -1;
    }

    return kq;
}

static void start_parent_death_watcher() {
	pid_t parent_pid = getppid();

    std::thread([parent_pid]() {
        int kq = watch_parent_with_kqueue(parent_pid);

        if (kq == -1) {
            raise(SIGTERM);
            return;
        }

        struct kevent out;
        int n = kevent(kq, nullptr, 0, &out, 1, nullptr);
        close(kq);

        if (n > 0 && out.filter == EVFILT_PROC && (out.fflags & NOTE_EXIT)) {
            raise(SIGTERM);
        }
    }).detach();
}

#endif

int main(int argc, char **argv) {

#if defined(_WIN32)
#elif defined(__APPLE__)
	start_parent_death_watcher();
#else
	prctl(PR_SET_PDEATHSIG, SIGTERM);

    if (getppid() == 1) {
        exit(0); 
    }
#endif

    std::string program;
    std::string shared_memory_id;
    if (argc > 0) {
        program = std::string(argv[0]);
    }

    if (argc != 1) {
        return EXIT_SUCCESS;
    }

    const char *parent_window_id = std::getenv("GODOT_PARENT_WINDOW_ID");

    const char *module_type = std::getenv("DISTRHO_MODULE_TYPE");
    if (module_type == NULL) {
        module_type = std::to_string(DistrhoCommon::PLUGIN_TYPE).c_str();
    }

    const char *distrho_path = std::getenv("DISTRHO_PATH");

    std::string godot_package = GodotDistrhoUtils::find_godot_package();

    std::vector<std::string> args;

    if (std::stoi(module_type) == DistrhoCommon::PLUGIN_TYPE) {
        args = {program, "--display-driver", "headless", "--audio-driver", "Distrho"};
    } else {
        args = {program,
                "--rendering-method",
                "gl_compatibility",
                "--rendering-driver",
                "opengl3",
                "--display-driver",
#if defined (_WIN32)
                "windows",
#elif defined (__APPLE__)
                "macos",
#else
                "x11",
#endif
                "--audio-driver",
                "Dummy"};

        if (parent_window_id != NULL) {
            args.push_back("--wid");
            args.push_back(parent_window_id);
        }
    }

    if (distrho_path != NULL) {
        args.push_back("--path");
        args.push_back(distrho_path);
    } else if (godot_package.size() > 0) {
        args.push_back("--main-pack");
        args.push_back(godot_package);
    }

    std::vector<char *> argvs;
    for (const auto &arg : args) {
        argvs.push_back((char *)arg.data());
    }
    argvs.push_back(nullptr);

    godot::GodotInstance *instance = libgodot.create_godot_instance(argvs.size(), argvs.data());
    if (instance == nullptr) {
        fprintf(stderr, "Error creating Godot instance\n");
        return EXIT_FAILURE;
    }

    instance->start();

    while (!instance->iteration()) {
    }
    libgodot.destroy_godot_instance(instance);

    return EXIT_SUCCESS;
}
