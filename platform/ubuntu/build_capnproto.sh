#!/bin/bash

dir=$(realpath .)

build_dir=$dir/addons/distrho/bin/linux/release 

mkdir -p $build_dir

vcpkg/vcpkg install --x-install-root=$build_dir
