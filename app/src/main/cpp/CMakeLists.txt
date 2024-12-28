cmake_minimum_required(VERSION 3.22.1)

project("kuro")

set(KITTYMEMORY_PATH ${CMAKE_SOURCE_DIR}/vendor/KittyMemory/KittyMemory)
file(GLOB KITTYMEMORY_SRC ${KITTYMEMORY_PATH}/*.cpp)
set(KEYSTONE_LIB ${KITTYMEMORY_PATH}/Deps/Keystone/libs-android/${CMAKE_ANDROID_ARCH_ABI}/libkeystone.a)

add_library(${CMAKE_PROJECT_NAME} SHARED
        ${KITTYMEMORY_SRC}
        native-lib.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
        ${KEYSTONE_LIB}
        android
        log)