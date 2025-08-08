#!/bin/bash

dir=$(realpath .)
bin_dir=$dir/addons/distrho/bin/linux/debug/vcpkg_installed/x64-linux/tools/capnproto

$bin_dir/capnp compile -o $bin_dir/capnpc-c++:src $dir/godot_distrho_schema.capnp

mv $dir/src/godot_distrho_schema.capnp.c++ $dir/src/godot_distrho_schema.capnp.cpp
