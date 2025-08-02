#!/bin/bash

echo TAG_VERSION=$TAG_VERSION
echo BUILD_SHA=$BUILD_SHA

dir=$(realpath .)
src_dir=$dir/modules/capnproto
build_dir=$src_dir/build/linux/release

# build capnproto

mkdir -p $build_dir
cd $build_dir

cmake -DCMAKE_INSTALL_PREFIX:PATH=$dir/addons/capnproto/bin/linux/release \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_VERBOSE_MAKEFILE=1 \
    $src_dir

make
make install

# build libgodot

cd $dir/modules/godot
scons platform=linux library_type=static_library verbose=yes

# build godot-distrho (gdextension)

cd $dir
scons platform=linux target=template_release

# build godot-distrho (distrho)

mkdir -p $dir/addons/distrho/bin/linux/release 
cd $dir/addons/distrho/bin/linux/release 

cmake -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_VERBOSE_MAKEFILE=1 \
    $dir

make
