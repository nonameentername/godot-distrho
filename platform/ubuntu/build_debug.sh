#!/bin/bash

echo TAG_VERSION=$TAG_VERSION
echo BUILD_SHA=$BUILD_SHA

dir=$(realpath .)

# build libgodot

cd $dir/modules/godot
scons platform=linux dev_build=yes debug_symbols=yes library_type=static_library verbose=yes

# configure godot-distrho (distrho)

build_dir=$dir/addons/distrho/bin/linux/debug

mkdir -p $build_dir
cd $build_dir

cmake -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_VERBOSE_MAKEFILE=1 \
    $dir

# build godot-distrho (gdextension)

cd $dir
scons platform=linux target=template_debug dev_build=yes debug_symbols=yes

#$dir/modules/godot/bin/godot.linuxbsd.editor.dev.x86_64 --headless --import || true
#$dir/modules/godot/bin/godot.linuxbsd.editor.dev.x86_64 --headless --import || true

# build godot-distrho (distrho)

export DISTRHO_PATH=$dir

cd $dir/addons/distrho/bin/linux/debug

make
