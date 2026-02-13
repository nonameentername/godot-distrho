cmake_minimum_required(VERSION 3.15)

set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "" FORCE)

set(VCPKG_FEATURE_FLAGS versions)
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)
set(VCPKG_OVERLAY_TRIPLETS ${CMAKE_CURRENT_SOURCE_DIR}/platform/vcpkg/triplets)
set(VCPKG_OVERLAY_PORTS ${CMAKE_CURRENT_SOURCE_DIR}/platform/vcpkg/ports)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

if(OSXCROSS_TARGET)
    if(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64")
        set(VCPKG_TARGET_TRIPLET "x64-osxcross")
        set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/platform/osxcross/osx-x86_64.cmake)
    elseif(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
        set(VCPKG_TARGET_TRIPLET "arm64-osxcross")
        set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/platform/osxcross/osx-arm64.cmake)
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(GODOT_LIBRARIES_SUFFIX ".macos.editor.dev.${CMAKE_OSX_ARCHITECTURES}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
    else()
        set(GODOT_LIBRARIES_SUFFIX ".macos.editor.${CMAKE_OSX_ARCHITECTURES}")
    endif()
elseif (CMAKE_SYSTEM_NAME STREQUAL "MinGW" AND UNIX)
    set(VCPKG_TARGET_TRIPLET "x64-mingw-static")
    set(VCPKG_INSTALL_OPTIONS "--allow-unsupported")
    set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/toolchains/mingw.cmake)
    set(VCPKG_CMAKE_SYSTEM_NAME MinGW)
    set(VCPKG_TARGET_ARCHITECTURE x64)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(GODOT_LIBRARIES_SUFFIX ".windows.editor.dev.x86_64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
    else()
        set(GODOT_LIBRARIES_SUFFIX ".windows.editor.x86_64")
    endif()
elseif(UNIX)
    set(VCPKG_TARGET_TRIPLET "x64-linux")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(GODOT_LIBRARIES_SUFFIX ".linuxbsd.editor.dev.x86_64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
    else()
        set(GODOT_LIBRARIES_SUFFIX ".linuxbsd.editor.x86_64")
    endif()
endif()

add_compile_definitions(BOOST_ASIO_DISABLE_BOOST_CONTEXT_FIBER)

message(STATUS "VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")
message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")

project(godot-distrho VERSION 1.1.0)

#if(CMAKE_CROSSCOMPILING)
set(CMAKE_CROSSCOMPILING_EMULATOR echo)
#endif()

set(CMAKE_POLICY_DEFAULT_CMP0063 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(KJ_USE_FIBERS=0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDISTRHO_NAMESPACE=GodotDISTRHO -DDGL_NAMESPACE=GodotDGL -DUSE_GDEXTENSION=1 -DDISTRHO_PLUGIN_ENABLE_SUBPROCESS=1")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

add_subdirectory(modules/dpf)

find_package(Threads REQUIRED)

find_package(CapnProto REQUIRED)

set(GODOT_LINK_DIRECTORIES
  modules/godot/bin
  modules/godot/bin/obj/core
  modules/godot/bin/obj/drivers
  modules/godot/bin/obj/editor
  modules/godot/bin/obj/main
  modules/godot/bin/obj/modules
  modules/godot/bin/obj/modules/freetype
  modules/godot/bin/obj/modules/msdfgen
  modules/godot/bin/obj/modules/text_server_adv
  modules/godot/bin/obj/platform
  modules/godot/bin/obj/scene
  modules/godot/bin/obj/servers
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
  module_objectdb_profiler
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
  module_mp3
  module_mobile_vr
  module_multiplayer
  module_navigation_2d
  module_navigation_3d
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
  msdfgen_builtin
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

#godot-plugin

set(SOURCE
  src/plugin/libgodot_distrho.cpp
  src/plugin/main.cpp
  src/distrho_common.cpp
  src/plugin/godot_distrho_utils.cpp
)
    
set(HEADER src/plugin/libgodot_distrho.h)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -g3")

add_executable(godot-plugin ${SOURCE} ${HEADER})

set_target_properties(godot-plugin PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_SKIP_RPATH TRUE)

target_include_directories(godot-plugin PUBLIC
  src/plugin
  src
  modules/dpf/distrho
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
  godot${GODOT_LIBRARIES_SUFFIX}
  CapnProto::capnp-rpc
  CapnProto::capnp
  CapnProto::kj-async
  CapnProto::kj
  ${GODOT_LIBRARIES}
)

find_package(Boost REQUIRED COMPONENTS filesystem)
target_link_libraries(godot-plugin PRIVATE Boost::filesystem)

if(OSXCROSS_TARGET)
  target_link_libraries(godot-plugin PRIVATE
    debug godot-cpp.macos.template_debug.dev.universal
    optimized godot-cpp.macos.template_release.universal
  )

  target_link_libraries(godot-plugin PRIVATE
    "-framework AppKit"
    "-framework Foundation"
    "-framework CoreFoundation"
    "-framework AVFoundation"
    "-framework CoreMedia"
    "-framework CoreVideo"
    "-framework CoreGraphics"
    "-framework IOSurface"
    "-framework IOKit"
    "-framework CoreMIDI"
    "-framework QuartzCore"
    "-framework UniformTypeIdentifiers"
    "-framework AudioToolbox"
    "-framework CoreAudio"
    "-framework GameController"
    "-framework Metal"
    "-framework MetalKit"
    "-framework Cocoa"
    "-framework CoreText"
    "-framework ApplicationServices"
    "-framework Security"
    "-framework Carbon"
    "-framework CoreHaptics"
    "-framework ForceFeedback"
	/MoltenVK/MoltenVK/static/MoltenVK.xcframework/macos-arm64_x86_64/libMoltenVK.a
  )

  if(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
    target_link_libraries(godot-plugin PRIVATE
      "-framework MetalFX"
    )
  endif()

elseif (MINGW)
  target_link_libraries(godot-plugin PRIVATE
    # Open the linker group "sandwich"
    "-Wl,--start-group"

    # These keywords work because we are inside target_link_libraries
    debug godot-cpp.windows.template_debug.dev.x86_64
    optimized godot-cpp.windows.template_release.x86_64

    # System libraries inside the group
    winmm
    dsound
    kernel32
    ole32
    oleaut32
    sapi
    user32
    gdi32
    iphlpapi
    shlwapi
    shcore
    wsock32
    ws2_32
    shell32
    advapi32
    dinput8
    dxguid
    imm32
    bcrypt
    crypt32
    avrt
    dwmapi
    dwrite
    wbemuuid
    ntdll
    hid
    hidparse
    setupapi
    ksuser
    uuid
    debug psapi
    #TODO: should be only for debug
    dbghelp
    #debug dbghelp

    # Close the linker group
    "-Wl,--end-group"
  )

elseif(UNIX)
  find_package(X11 REQUIRED)
  target_link_libraries(godot-plugin PRIVATE
    debug godot-cpp.linux.template_debug.dev.x86_64
    optimized godot-cpp.linux.template_release.x86_64
    ${X11_LIBRARIES}
  )
endif()

if (MINGW AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_custom_command(TARGET godot-plugin POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} --only-keep-debug $<TARGET_FILE:godot-plugin> $<TARGET_FILE:godot-plugin>.dbg
    COMMAND ${CMAKE_STRIP} --strip-debug $<TARGET_FILE:godot-plugin>
    COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE:godot-plugin>.dbg $<TARGET_FILE:godot-plugin>
  )
endif()

#godot-distrho

set(PLUGIN_FORMATS clap lv2 vst2 vst3)

if(NOT APPLE)
  list(APPEND PLUGIN_FORMATS jack)
endif()

dpf_add_plugin(godot-distrho
  TARGETS ${PLUGIN_FORMATS}
  NO_SHARED_RESOURCES
  UI_TYPE opengl
  NO_USE_FILE_BROWSER
  FILES_DSP
    src/godot_distrho_schema.capnp.cpp
    src/plugin/godot_distrho_plugin.cpp
    src/plugin/godot_distrho_utils.cpp
    src/plugin/godot_distrho_plugin_server.cpp
    src/plugin/godot_distrho_plugin_client.cpp
    src/distrho_shared_memory.cpp
    src/distrho_shared_memory_audio.cpp
    src/distrho_shared_memory_rpc.cpp
    src/distrho_common.cpp
    #src/plugin/libgodot_distrho.cpp
  FILES_UI
    src/plugin/godot_distrho_gui_widget.cpp
    src/plugin/godot_distrho_images.cpp
    src/plugin/godot_distrho_ui.cpp
    src/plugin/godot_distrho_utils.cpp
    src/plugin/godot_distrho_ui_server.cpp
    src/plugin/godot_distrho_ui_client.cpp
    src/distrho_shared_memory_audio.cpp
    src/distrho_shared_memory_rpc.cpp
    src/distrho_common.cpp
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
  CapnProto::capnp-rpc
  CapnProto::capnp
  CapnProto::kj-async
  CapnProto::kj
  #godot.linuxbsd.editor.x86_64
  #godot-cpp.linux.template_debug.x86_64
  #${GODOT_LIBRARIES}
)

target_link_libraries(godot-distrho PRIVATE Boost::filesystem)

#set(GODOT_PLUGIN_LV2_COPY_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin/godot-distrho.lv2")
#set(GODOT_PLUGIN_LV2_COPY_DEST "${GODOT_PLUGIN_LV2_COPY_DEST_DIR}/godot-plugin")
#set(GODOT_PLUGIN_VST3_COPY_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin/godot-distrho.vst3/Contents/x86_64-linux")
#set(GODOT_PLUGIN_VST3_COPY_DEST "${GODOT_PLUGIN_VST3_COPY_DEST_DIR}/godot-plugin")
#
#if (TARGET godot-plugin)
#  add_custom_target(copy_godot_plugin ALL
#    COMMENT "Copy built godot-plugin into LV2/VST3 bundles"
#    COMMAND ${CMAKE_COMMAND} -E echo "Copying $<TARGET_FILE:godot-plugin> to ${GODOT_PLUGIN_LV2_COPY_DEST_DIR} and ${GODOT_PLUGIN_VST3_COPY_DEST_DIR}"
#    COMMAND ${CMAKE_COMMAND} -E make_directory "${GODOT_PLUGIN_LV2_COPY_DEST_DIR}"
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:godot-plugin>" "${GODOT_PLUGIN_LV2_COPY_DEST}"
#    COMMAND ${CMAKE_COMMAND} -E make_directory "${GODOT_PLUGIN_VST3_COPY_DEST_DIR}"
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:godot-plugin>" "${GODOT_PLUGIN_VST3_COPY_DEST}"
#  )
#  add_dependencies(copy_godot_plugin godot-plugin)
#else()
#  message(WARNING "Target 'godot-plugin' not found; copy_godot_plugin not created")
#endif()
