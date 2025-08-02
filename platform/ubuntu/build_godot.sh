#!/bin/bash

dir=$(realpath .)

# build godot

cd $dir/modules/godot
scons platform=linux dev_build=yes debug_symbols=yes

# dump api

cd $dir/godot-cpp/gdextension 
$dir/modules/godot/bin/godot.linuxbsd.editor.dev.x86_64 --headless --dump-extension-api
