#!/bin/bash

echo TAG_VERSION=$TAG_VERSION
echo BUILD_SHA=$BUILD_SHA

dir=$(realpath .)

# build libgodot

cd $dir/modules/godot
scons platform=linux library_type=static_library verbose=yes

# configure godot-distrho (distrho)

mkdir -p $dir/addons/distrho/bin/linux/release 
cd $dir/addons/distrho/bin/linux/release 

cmake -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_VERBOSE_MAKEFILE=1 \
    $dir

# build godot-distrho (gdextension)

cd $dir
scons platform=linux target=template_release

#$dir/modules/godot/bin/godot.linuxbsd.editor.dev.x86_64 --headless --import || true
#$dir/modules/godot/bin/godot.linuxbsd.editor.dev.x86_64 --headless --import || true

# build godot-distrho (distrho)

export DISTRHO_PATH=$dir

cd $dir/addons/distrho/bin/linux/release
make
