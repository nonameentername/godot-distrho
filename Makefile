UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
PLATFORM=linux
else ifeq ($(UNAME), Darwin)
PLATFORM=osx
else
PLATFORM=windows
endif

all: common ubuntu

#common

common: capnproto schema godot

capnproto:
	$(MAKE) shell-ubuntu SHELL_COMMAND='./platform/ubuntu/build_capnproto.sh'

schema:
	$(MAKE) shell-ubuntu SHELL_COMMAND='./platform/ubuntu/build_schema.sh'

godot:
	$(MAKE) shell-ubuntu SHELL_COMMAND='./platform/ubuntu/build_godot.sh'

format:
	clang-format -i src/*.cpp src/*.h
	clang-format -i src/plugin/*.cpp src/plugin/*.h
	gdformat $(shell find -name '*.gd' ! -path './godot-cpp/*' ! -path './modules/godot/*')

#distrho plugin

.PHONY: lv2
lv2:
	cp distrho_plugin_info.json addons/distrho/bin/linux/debug/bin/godot-distrho.lv2 && \
		cd addons/distrho/bin/linux/debug && \
		make && \
		cp bin/godot-plugin bin/godot-distrho.lv2 && \
		cp ../libdistrhogodot.linux.template_debug.dev.x86_64.so bin/godot-distrho.lv2 && \
		rm -rf ~/.lv2/godot-distrho.lv2 && \
		cp -r bin/godot-distrho.lv2/ ~/.lv2 && \
		jalv.gtk3 -s https://github.com/nonameentername/godot-distrho

vst3:
	cp distrho_plugin_info.json addons/distrho/bin/linux/debug/bin/godot-distrho.vst3 && \
		cd addons/distrho/bin/linux/debug && \
		make && \
		cp bin/godot-plugin bin/godot-distrho.vst3/Contents/x86_64-linux && \
		cp ../libdistrhogodot.linux.template_debug.dev.x86_64.so bin/godot-distrho.vst3/Contents/x86_64-linux && \
		rm -rf ~/.vst3/godot-distrho.vst3 && \
		cp -r bin/godot-distrho.vst3/ ~/.vst3

#godot library

dev-build:
	$(MAKE) shell-ubuntu SHELL_COMMAND='scons platform=$(PLATFORM) target=template_debug dev_build=yes debug_symbols=yes compiledb=true'

UNAME := $(shell uname)
ifeq ($(UNAME), Windows)
    UID=1000
    GID=1000
else
    UID=`id -u`
    GID=`id -g`
endif

SHELL_COMMAND = bash

docker-ubuntu:
	docker build -t godot-distrho-ubuntu ./platform/ubuntu

shell-ubuntu: docker-ubuntu
	docker run -it --rm -v ${CURDIR}:${CURDIR} -w ${CURDIR} godot-distrho-ubuntu ${SHELL_COMMAND}

ubuntu: ubuntu-debug ubuntu-release

ubuntu-debug:
	$(MAKE) shell-ubuntu SHELL_COMMAND='./platform/ubuntu/build_debug.sh'

ubuntu-release:
	$(MAKE) shell-ubuntu SHELL_COMMAND='./platform/ubuntu/build_release.sh'

docker-osxcross:
	docker build -t godot-distrho-osxcross ./platform/osxcross

shell-osxcross: docker-osxcross
	docker run -it --rm -v ${CURDIR}:${CURDIR} -w ${CURDIR} godot-distrho-osxcross ${SHELL_COMMAND}

osxcross: osxcross-debug osxcross-release

osxcross-debug:
	$(MAKE) shell-osxcross SHELL_COMMAND='./platform/osxcross/build_debug.sh'

osxcross-release:
	$(MAKE) shell-osxcross SHELL_COMMAND='./platform/osxcross/build_release.sh'

docker-mingw:
	docker build -t godot-distrho-mingw ./platform/mingw

shell-mingw: docker-mingw
	docker run -it --rm -v ${CURDIR}:${CURDIR} -w ${CURDIR} godot-distrho-mingw ${SHELL_COMMAND}

mingw: mingw-debug mingw-release

mingw-debug:
	$(MAKE) shell-mingw SHELL_COMMAND='./platform/mingw/build_debug.sh'

mingw-release:
	$(MAKE) shell-mingw SHELL_COMMAND='./platform/mingw/build_release.sh'

clean:
	rm -rf addons/distrho/bin modules/godot/bin vcpkg_installed
