#!/bin/bash

dir=$(realpath .)

mkdir -p $dir/addons/distrho/bin/linux/release 
cd $dir/addons/distrho/bin/linux/release 

cmake -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_VERBOSE_MAKEFILE=1 \
    $dir
