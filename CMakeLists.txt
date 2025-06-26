cmake_minimum_required(VERSION 3.15)
project(godot-distrho VERSION 1.1.0)

set(CMAKE_POLICY_DEFAULT_CMP0063 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDISTRHO_NAMESPACE=GodotDISTRHO -DDGL_NAMESPACE=GodotDGL -DDISTRHO_PLUGIN_ENABLE_SUBPROCESS=1")

add_subdirectory(modules/dpf)

find_package(Threads REQUIRED)

set(GODOT_LIBRARIES_SUFFIX ".linuxbsd.editor.x86_64")

set(GODOT_LINK_DIRECTORIES
  modules/godot/bin
  modules/godot/core
  modules/godot/drivers
  modules/godot/editor
  modules/godot/main
  modules/godot/modules
  modules/godot/modules/freetype
  modules/godot/modules/msdfgen
  modules/godot/modules/text_server_adv
  modules/godot/platform
  modules/godot/scene
  modules/godot/servers
)

set(GODOT_LIBRARIES_NAMES
  main
  modules
  platform
  drivers
  editor
  scene
  servers
  core
  module_mbedtls
  module_astcenc
  module_basis_universal
  module_bcdec
  module_betsy
  module_bmp
  module_camera
  module_csg
  module_cvtt
  module_dds
  module_enet
  module_etcpak
  module_fbx
  module_freetype
  module_gdscript
  module_glslang
  module_gltf
  module_godot_physics_2d
  module_godot_physics_3d
  module_gridmap
  module_hdr
  module_interactive_music
  module_jolt_physics
  module_jpg
  module_jsonrpc
  module_ktx
  module_lightmapper_rd
  module_meshoptimizer
  module_minimp3
  module_mobile_vr
  module_msdfgen
  module_multiplayer
  module_navigation
  module_noise
  module_ogg
  module_openxr
  module_raycast
  module_regex
  module_svg
  module_text_server_adv
  module_tga
  module_theora
  module_tinyexr
  module_upnp
  module_vhacd
  module_vorbis
  module_webp
  module_webrtc
  module_websocket
  module_webxr
  module_xatlas_unwrap
  module_zip
  freetype_builtin
  graphite_builtin
  harfbuzz_builtin
  icu_builtin
  msdfgen_builtin
  godot
)

set(GODOT_LIBRARIES "")
foreach(lib IN LISTS GODOT_LIBRARIES_NAMES)
    list(APPEND GODOT_LIBRARIES "${lib}${GODOT_LIBRARIES_SUFFIX}")
endforeach()

dpf_add_plugin(godot-distrho
  #TARGETS au clap lv2 vst2 vst3
  #TARGETS au lv2 vst2 vst3 jack
  TARGETS jack
  NO_SHARED_RESOURCES
  UI_TYPE opengl
  NO_USE_FILE_BROWSER
  FILES_DSP
    src/plugin/godot_distrho_plugin.cpp
    src/plugin/godot_distrho_utils.cpp
    src/distrho_shared_memory_audio.cpp
    #src/distrho_schema.capnp.cpp
    #src/plugin/libgodot_distrho.cpp
  FILES_UI
    src/plugin/godot_distrho_ui.cpp
    src/plugin/godot_distrho_utils.cpp
    #src/plugin/libgodot_distrho.cpp
)

target_compile_definitions(dgl-opengl PUBLIC
  DGL_NAMESPACE=GodotDGL
  DGL_WINDOWS_ICON_ID=401
  NVG_FONT_TEXTURE_FLAGS=NVG_IMAGE_NEAREST
)

target_include_directories(godot-distrho PUBLIC
  src/plugin
  src
  #modules/godot/core/extension
  #godot-cpp/include
  #godot-cpp/gen/src
  #godot-cpp/gen/include
  ${CMAKE_BINARY_DIR}
)

target_link_directories(godot-distrho PUBLIC
    #godot-cpp/bin
  #${GODOT_LINK_DIRECTORIES}
)

target_link_libraries(godot-distrho PUBLIC
  ${CMAKE_THREAD_LIBS_INIT}
  #godot.linuxbsd.editor.x86_64
  #godot-cpp.linux.template_debug.x86_64
  #${GODOT_LIBRARIES}
)

set(SOURCE src/plugin/libgodot_distrho.cpp src/plugin/main.cpp)
set(HEADER src/plugin/libgodot_distrho.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -g3")

add_executable(godot-plugin ${SOURCE} ${HEADER})

set(CMAKE_SKIP_RPATH TRUE)

target_include_directories(godot-plugin PUBLIC
  src/plugin
  src
  modules/godot/core/extension
  godot-cpp/include
  godot-cpp/gen/src
  godot-cpp/gen/include
  ${CMAKE_BINARY_DIR}
)

target_link_directories(godot-plugin PRIVATE
  godot-cpp/bin
  ${GODOT_LINK_DIRECTORIES}
)

target_link_libraries(godot-plugin PRIVATE
  ${CMAKE_THREAD_LIBS_INIT}
  godot.linuxbsd.editor.x86_64
  godot-cpp.linux.template_debug.x86_64
  ${GODOT_LIBRARIES}
)
