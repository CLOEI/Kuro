project(thirdparty)

set(KITTYMEMORY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/KittyMemory)
file(GLOB KITTYMEMORY_SRC ${KITTYMEMORY_PATH}/*.cpp)
set(KEYSTONE_LIB ${KITTYMEMORY_PATH}/Deps/Keystone/libs-android/${CMAKE_ANDROID_ARCH_ABI}/libkeystone.a)

add_subdirectory(magic_enum)
add_subdirectory(Dobby)