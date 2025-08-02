#!/bin/bash

dir=$(realpath .)
src_dir=$dir/modules/capnproto
build_dir=$src_dir/build/linux/release

mkdir -p $build_dir
cd $build_dir

cmake -DCMAKE_INSTALL_PREFIX:PATH=$dir/addons/capnproto/bin/linux/release \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DCMAKE_VERBOSE_MAKEFILE=1 \
    $src_dir

make
make install
