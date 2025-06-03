all: godot godot_dump_api godot_cpp godot_library

godot:
	cd modules/godot && scons platform=linux dev_build=yes debug_symbols=yes

godot_library:
	#cd modules/godot && scons platform=linux dev_build=yes debug_symbols=yes library_type=shared_library verbose=yes
	cd modules/godot && scons platform=linux debug_symbols=yes library_type=shared_library verbose=yes

#check if this is really needed
godot_dump_api:
	cd godot-cpp/gdextension && ../../modules/godot/bin/godot.linuxbsd.editor.dev.x86_64 --dump-extension-api

godot_cpp:
	cd godot-cpp && scons platform=linux dev_build=yes debug_symbols=yes

build:
	cd build && make && rm -rf ~/.lv2/godot-distrho.lv2/ && cp -r bin/godot-distrho.lv2/ ~/.lv2/ && jalv.gtk https://github.com/nonameentername/godot-distrho
