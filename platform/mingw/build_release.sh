#!/bin/bash

echo TAG_VERSION=$TAG_VERSION
echo BUILD_SHA=$BUILD_SHA

dir=$(realpath .)

# build libgodot

cd $dir/modules/godot
scons platform=windows library_type=static_library verbose=yes d3d12=no

# configure godot-distrho (distrho)

build_dir=$dir/addons/distrho/bin/windows/release

mkdir -p $build_dir
mkdir -p $build_dir/bin
cd $build_dir

cmake -DCMAKE_TOOLCHAIN_FILE=$dir/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=$dir/vcpkg/scripts/toolchains/mingw.cmake \
    -DCMAKE_MAKE_PROGRAM=gmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_VERBOSE_MAKEFILE=1 \
    -DVCPKG_CMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_SYSTEM_NAME=MinGW \
    -DVCPKG_TARGET_ARCHITECTURE=x64 \
    -DVCPKG_TARGET_TRIPLET=x64-mingw-static \
    -DVCPKG_DEFAULT_TRIPLET=x64-mingw-static \
    -DCMAKE_CXX_COMPILER=/usr/bin/x86_64-w64-mingw32-g++ \
    -DCMAKE_C_COMPILER=/usr/bin/x86_64-w64-mingw32-gcc \
    -DCMAKE_SYSROOT=/usr/x86_64-w64-mingw32 \
    $dir

# build godot-distrho (gdextension)

cp /usr/lib/gcc/x86_64-w64-mingw32/*-posix/libstdc++-6.dll $build_dir/bin/
cp /usr/lib/gcc/x86_64-w64-mingw32/*-posix/libgcc_s_seh-1.dll $build_dir/bin/
cp /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll $build_dir/bin/

cd $dir
scons platform=windows target=template_release

# build godot-distrho (distrho)

export DISTRHO_PATH=$dir

cd $dir/addons/distrho/bin/windows/release

make
