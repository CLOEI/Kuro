cmake_minimum_required(VERSION 3.28)
project(kuro)

set(CMAKE_CXX_STANDARD 17)

set(KITTYMEMORY_PATH ${CMAKE_SOURCE_DIR}/thirdparty/KittyMemory/KittyMemory)
file(GLOB KITTYMEMORY_SRC ${KITTYMEMORY_PATH}/*.cpp)
set(KEYSTONE_LIB ${KITTYMEMORY_PATH}/Deps/Keystone/libs-android/${CMAKE_ANDROID_ARCH_ABI}/libkeystone.a)

add_subdirectory(thirdparty/magic_enum)
add_subdirectory(thirdparty/Dobby)
add_subdirectory(thirdparty/enet)
include_directories(thirdparty/enet/include)

add_library(${PROJECT_NAME} SHARED
        ${KITTYMEMORY_SRC}
        main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC 
        ${KITTYMEMORY_PATH})

target_link_libraries(${PROJECT_NAME}
        ${KEYSTONE_LIB}
        magic_enum::magic_enum
        dobby_static
        enet
        android
        log)
