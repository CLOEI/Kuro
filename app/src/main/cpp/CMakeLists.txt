cmake_minimum_required(VERSION 3.22.1)

project("kuro")

set(KITTYMEMORY_PATH ${CMAKE_SOURCE_DIR}/vendor/KittyMemory/KittyMemory)
file(GLOB KITTYMEMORY_SRC ${KITTYMEMORY_PATH}/*.cpp)
set(KEYSTONE_LIB ${KITTYMEMORY_PATH}/Deps/Keystone/libs-android/${CMAKE_ANDROID_ARCH_ABI}/libkeystone.a)

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/Dobby)
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/enet)
include_directories(${CMAKE_SOURCE_DIR}/vendor/enet/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor/imgui)

set(IMGUI_PATH ${CMAKE_SOURCE_DIR}/vendor/imgui)
set(IMGUI_SRC
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.h
        ${IMGUI_PATH}/imgui.h
        ${IMGUI_PATH}/imgui.cpp
        ${IMGUI_PATH}/imgui_draw.cpp
        ${IMGUI_PATH}/imgui_tables.cpp
        ${IMGUI_PATH}/imgui_widgets.cpp
        ${IMGUI_PATH}/imgui_demo.cpp
)

add_library(${CMAKE_PROJECT_NAME} SHARED
        ${KITTYMEMORY_SRC}
        ${IMGUI_SRC}
        native-lib.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
        ${KEYSTONE_LIB}
        dobby_static
        enet
        android
        EGL
        GLESv3
        log)