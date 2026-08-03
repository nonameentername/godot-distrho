#!/bin/sh

cd -- "$(dirname -- "$0")" || exit 1
exec ./lv2_ttl_generator ./godot-distrho_dsp.dylib
