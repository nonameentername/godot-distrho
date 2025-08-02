#!/bin/bash

dir=$(realpath .)
bin_dir=$dir/addons/capnproto/bin/linux/release/bin

$bin_dir/capnp compile -o $bin_dir/capnpc-c++:src $dir/godot_distrho_schema.capnp

mv $dir/src/godot_distrho_schema.capnp.c++ $dir/src/godot_distrho_schema.capnp.cpp
