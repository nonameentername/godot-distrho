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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDISTRHO_NAMESPACE=GodotDISTRHO -DDGL_NAMESPACE=GodotDGL")

add_subdirectory(modules/dpf)

find_package(Threads REQUIRED)

dpf_add_plugin(godot-distrho
  #TARGETS au clap lv2 vst2 vst3
  #TARGETS au lv2 vst2 vst3
  TARGETS jack
  #NO_SHARED_RESOURCES
  UI_TYPE opengl
  # NO_USE_FILE_BROWSER
  FILES_DSP
    src/godot-distrho-plugin.cpp
    src/libgodot_distrho.cpp
  FILES_UI
    src/godot-distrho-ui.cpp)

target_compile_definitions(dgl-opengl PUBLIC
  DGL_NAMESPACE=GodotDGL
  DGL_WINDOWS_ICON_ID=401
  NVG_FONT_TEXTURE_FLAGS=NVG_IMAGE_NEAREST
)

target_include_directories(godot-distrho PUBLIC
  src
  modules/godot/core/extension
  godot-cpp/include
  godot-cpp/gen/src
  godot-cpp/gen/include
  ${CMAKE_BINARY_DIR}
)

target_link_directories(godot-distrho PUBLIC
  modules/godot/bin
  godot-cpp/bin
)

target_link_libraries(godot-distrho PUBLIC
  ${CMAKE_THREAD_LIBS_INIT}
  godot.linuxbsd.editor.x86_64
  godot-cpp.linux.template_debug.dev.x86_64
)



dpf_add_plugin(godot-distrho-standalone
  TARGETS jack
  #NO_SHARED_RESOURCES
  UI_TYPE opengl
  # NO_USE_FILE_BROWSER
  FILES_DSP
    src/godot-distrho-plugin.cpp
    src/libgodot_distrho.cpp
  FILES_UI
    src/godot-distrho-ui.cpp)

target_include_directories(godot-distrho-standalone PUBLIC
  src
  modules/godot/core/extension
  godot-cpp/include
  godot-cpp/gen/src
  godot-cpp/gen/include
  ${CMAKE_BINARY_DIR}
)

target_link_directories(godot-distrho-standalone PUBLIC
  modules/godot/bin
  godot-cpp/bin
)

target_link_libraries(godot-distrho-standalone PUBLIC
  ${CMAKE_THREAD_LIBS_INIT}
  godot.linuxbsd.editor.x86_64
  godot-cpp.linux.template_debug.dev.x86_64
)



set(SOURCE src/libgodot_distrho.cpp src/main.cpp)
set(HEADER src/libgodot_distrho.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -g3")

add_executable(godot ${SOURCE} ${HEADER})

target_include_directories(godot PUBLIC
  src
  modules/godot/core/extension
  godot-cpp/include
  godot-cpp/gen/src
  godot-cpp/gen/include
  ${CMAKE_BINARY_DIR}
)

target_link_directories(godot PUBLIC
  modules/godot/bin
  godot-cpp/bin
)

target_link_libraries(godot PUBLIC
  ${CMAKE_THREAD_LIBS_INIT}
  godot.linuxbsd.editor.x86_64
  godot-cpp.linux.template_debug.dev.x86_64
)
