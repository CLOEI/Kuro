cmake_minimum_required(VERSION 3.30)
project(kuro)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_SYSTEM_VERSION 24)
set(ANDROID_ABI armeabi-v7a)
set(ANDROID_PLATFORM android-24)

add_subdirectory(thirdparty)

add_library(${PROJECT_NAME} SHARED
        ${KITTYMEMORY_SRC}
        main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${KITTYMEMORY_PATH})
target_link_libraries(${PROJECT_NAME}
        ${KEYSTONE_LIB}
        dobby
        magic_enum::magic_enum
        android
        log)
