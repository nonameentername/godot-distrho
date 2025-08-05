#!/bin/bash

echo TAG_VERSION=$TAG_VERSION
echo BUILD_SHA=$BUILD_SHA

dir=$(realpath .)
src_dir=$dir/modules/capnproto
build_dir=$src_dir/build/linux/debug

# build capnproto

mkdir -p $build_dir
cd $build_dir

cmake -DCMAKE_INSTALL_PREFIX:PATH=$dir/addons/capnproto/bin/linux/debug \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_VERBOSE_MAKEFILE=1 \
    $src_dir

make
make install

# build libgodot

cd $dir/modules/godot
scons platform=linux dev_build=yes debug_symbols=yes library_type=static_library verbose=yes

# build godot-distrho (gdextension)

cd $dir
scons platform=linux target=template_debug dev_build=yes debug_symbols=yes

$dir/modules/godot/bin/godot.linuxbsd.editor.dev.x86_64 --headless --import || true
$dir/modules/godot/bin/godot.linuxbsd.editor.dev.x86_64 --headless --import || true

# build godot-distrho (distrho)

export DISTRHO_PATH=$dir

mkdir -p $dir/addons/distrho/bin/linux/debug
cd $dir/addons/distrho/bin/linux/debug

cmake -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_VERBOSE_MAKEFILE=1 \
    $dir

make
