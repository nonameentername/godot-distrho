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

build:
	cd build && make && rm -rf ~/.lv2/godot-distrho.lv2/ && cp -r bin/godot-distrho.lv2/ ~/.lv2/ && jalv.gtk https://github.com/nonameentername/godot-distrho

#godot library

dev-build:
	scons platform=$(PLATFORM) target=template_debug dev_build=yes debug_symbols=yes compiledb=true

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
	docker run -it --rm -v ${CURDIR}:${CURDIR} --user ${UID}:${GID} -w ${CURDIR} godot-distrho-ubuntu ${SHELL_COMMAND}

ubuntu:
	$(MAKE) shell-ubuntu SHELL_COMMAND='./platform/ubuntu/build_debug.sh'
	$(MAKE) shell-ubuntu SHELL_COMMAND='./platform/ubuntu/build_release.sh'

docker-osxcross:
	docker build -t godot-distrho-osxcross ./platform/osxcross

shell-osxcross: docker-osxcross
	docker run -it --rm -v ${CURDIR}:${CURDIR} --user ${UID}:${GID} -w ${CURDIR} godot-distrho-osxcross ${SHELL_COMMAND}

osxcross:
	$(MAKE) shell-osxcross SHELL_COMMAND='./platform/osxcross/build_debug.sh'
	$(MAKE) shell-osxcross SHELL_COMMAND='./platform/osxcross/build_release.sh'

clean:
	rm -rf addons/distrho/bin modules/godot/bin vcpkg_installed
