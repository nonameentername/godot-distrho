#!/bin/bash

echo TAG_VERSION=$TAG_VERSION
echo BUILD_SHA=$BUILD_SHA

$(eval /osxcross/tools/osxcross_conf.sh)

export MACOSX_DEPLOYMENT_TARGET=11.0

dir=$(realpath .)

# build libgodot

export OSXCROSS_ROOT=$OSXCROSS_BASE_DIR

cd $dir/modules/godot

for ARCH in x86_64 arm64; do
    scons platform=macos arch=$ARCH dev_build=yes debug_symbols=yes library_type=static_library verbose=yes \
        osxcross_sdk=$OSXCROSS_TARGET vulkan_sdk_path=/MoltenVK/MoltenVK/static/MoltenVK.xcframework metal=no
done

$dir/scripts/lipo-dir.py  \
    $dir/modules/godot/bin/libgodot.macos.editor.dev.arm64.a \
    $dir/modules/godot/bin/libgodot.macos.editor.dev.x86_64.a \
    $dir/modules/godot/bin/libgodot.macos.editor.dev.universal.a

# configure godot-distrho (distrho)

for ARCH in x86_64 arm64; do
    build_dir=$dir/addons/distrho/bin/osxcross-$ARCH/debug

    mkdir -p $build_dir
    cd $build_dir

    cmake -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_VERBOSE_MAKEFILE=1 \
        -DCMAKE_SYSTEM_NAME=Darwin \
        -DOSXCROSS_TARGET_DIR=${OSXCROSS_TARGET_DIR} \
        -DOSXCROSS_SDK=${OSXCROSS_SDK} \
        -DOSXCROSS_TARGET=${OSXCROSS_TARGET} \
        -DCMAKE_OSX_ARCHITECTURES=${ARCH} \
        $dir
done

$dir/scripts/lipo-dir.py  \
    $dir/addons/distrho/bin/osxcross-arm64/debug \
    $dir/addons/distrho/bin/osxcross-x86_64/debug \
    $dir/addons/distrho/bin/macos/debug 

# build godot-distrho (gdextension)

cd $dir
scons platform=macos target=template_debug dev_build=yes debug_symbols=yes osxcross_sdk=$OSXCROSS_TARGET 

# build godot-distrho (distrho)

export DISTRHO_PATH=$dir

for ARCH in x86_64 arm64; do
    build_dir=$dir/addons/distrho/bin/osxcross-$ARCH/debug
    cd $build_dir
	make
done

$dir/scripts/lipo-dir.py  \
    $dir/addons/distrho/bin/osxcross-arm64/debug \
    $dir/addons/distrho/bin/osxcross-x86_64/debug \
    $dir/addons/distrho/bin/macos/debug 

zsign -a $dir/addons/distrho/bin/macos/macos.framework/libdistrhogodot.macos.template_debug
