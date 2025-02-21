cmake_minimum_required(VERSION 3.30)
project(kuro)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(thirdparty)

add_library(${PROJECT_NAME} SHARED
        ${KITTYMEMORY_SRC}
        main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${KITTYMEMORY_PATH})
target_link_libraries(${PROJECT_NAME}
        ${KEYSTONE_LIB}
        dobby_static
        magic_enum::magic_enum
        android
        log)
