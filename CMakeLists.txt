cmake_minimum_required(VERSION 3.28)
project(kuro)

set(CMAKE_CXX_STANDARD 17)

if(${ANDROID_ABI} STREQUAL "arm64-v8a")
    set(ARCH "arm64")
    set(ARCH_LINK_FLAGS "-Wl,-z,max-page-size=16384")
elseif(${ANDROID_ABI} STREQUAL "armeabi-v7a")
    set(ARCH "arm")
    set(ARCH_LINK_FLAGS "")
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-declaration-after-statement)
    add_compile_options(-Wno-unsafe-buffer-usage)
    add_compile_options(-Wno-switch)
    add_compile_options(-Wno-switch-default)
    add_compile_options(-Wno-reserved-identifier)
    add_compile_options(-Wno-padded)
endif()

set(KITTYMEMORY_PATH ${CMAKE_SOURCE_DIR}/thirdparty/KittyMemory/KittyMemory)
file(GLOB KITTYMEMORY_SRC ${KITTYMEMORY_PATH}/*.cpp)
set(KEYSTONE_LIB ${KITTYMEMORY_PATH}/Deps/Keystone/libs-android/${CMAKE_ANDROID_ARCH_ABI}/libkeystone.a)

set(SHADOWHOOK_PATH ${CMAKE_SOURCE_DIR}/thirdparty/shadowhook)
file(GLOB SHADOWHOOK_SRC
    ${SHADOWHOOK_PATH}/*.c
    ${SHADOWHOOK_PATH}/arch/${ARCH}/*.c
    ${SHADOWHOOK_PATH}/common/*.c
    ${SHADOWHOOK_PATH}/third_party/*/*.c)

add_subdirectory(thirdparty/magic_enum)
add_subdirectory(thirdparty/enet)
include_directories(thirdparty/enet/include)

add_library(${PROJECT_NAME} SHARED
        ${KITTYMEMORY_SRC}
        ${SHADOWHOOK_SRC}
        main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC 
        ${KITTYMEMORY_PATH} 
        ${SHADOWHOOK_PATH}
        ${SHADOWHOOK_PATH}/include
        ${SHADOWHOOK_PATH}/arch/${ARCH}
        ${SHADOWHOOK_PATH}/common
        ${SHADOWHOOK_PATH}/third_party/xdl
        ${SHADOWHOOK_PATH}/third_party/bsd
        ${SHADOWHOOK_PATH}/third_party/lss)

target_link_libraries(${PROJECT_NAME}
        ${KEYSTONE_LIB}
        magic_enum::magic_enum
        enet
        android
        log)
