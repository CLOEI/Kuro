#include <dlfcn.h>
#include <thread>
#include <android/log.h>
#include <string>
#include "vendor/KittyMemory/KittyMemory/KittyInclude.hpp"
#include "dobby.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>

#define install_hook(fn_name_t, fn_ret_t, fn_args_t...)                                                                    \
    fn_ret_t (*orig_##fn_name_t)(fn_args_t);                                                                               \
    fn_ret_t fake_##fn_name_t(fn_args_t);                                                                                  \
    static void install_hook_##fn_name_t(void* sym_addr)                                                                   \
    {                                                                                                                      \
        if (sym_addr) {                                                                                                    \
            if (DobbyHook(sym_addr, (dobby_dummy_func_t)fake_##fn_name_t, (dobby_dummy_func_t*)&orig_##fn_name_t) == 0) {  \
                __android_log_print(ANDROID_LOG_INFO, "Kuro", "%s hooked", #fn_name_t);                                    \
            } else {                                                                                                       \
                __android_log_print(ANDROID_LOG_ERROR, "Kuro", "Failed to hook %s", #fn_name_t);                           \
            }                                                                                                              \
        } else {                                                                                                           \
            __android_log_print(ANDROID_LOG_ERROR, "Kuro", "Symbol address for %s is null", #fn_name_t);                   \
        }                                                                                                                  \
    }                                                                                                                      \
    static void install_hook_##fn_name_t(const char* lib, const char* name)                                                \
    {                                                                                                                      \
        void *sym_addr = DobbySymbolResolver(lib, name);                                                                   \
        if (sym_addr) {                                                                                                    \
            __android_log_print(ANDROID_LOG_INFO, "Kuro", "%s symbol address: %p", name, sym_addr);                        \
            install_hook_##fn_name_t(sym_addr);                                                                            \
        } else {                                                                                                           \
            __android_log_print(ANDROID_LOG_ERROR, "Kuro", "Failed to resolve symbol %s in %s", name, lib);                \
        }                                                                                                                  \
    }                                                                                                                      \
    static void install_hook_##fn_name_t(const char* name)                                                                 \
    {                                                                                                                      \
        install_hook_##fn_name_t(nullptr, name);                                                                           \
    }                                                                                                                      \
    fn_ret_t fake_##fn_name_t(fn_args_t)

install_hook(enet_host_service, __int64_t, unsigned int *host, __int64_t event, int timeout) {
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "enet_host_service called");
    return orig_enet_host_service(host, event, timeout);
}

install_hook(eglSwapBuffers, EGLBoolean, EGLDisplay dpy, EGLSurface surface) {
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "eglSwapBuffers called");
    return orig_eglSwapBuffers(dpy, surface);
}

const std::string lib_game = "libgrowtopia.so";
const std::string lib_egl = "libEGL.so";
ElfScanner game_lib;
ElfScanner egl_lib;

__unused __attribute__((constructor))
void lib_main() {
    auto thread = std::thread([]() {
        do {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            game_lib = ElfScanner::createWithPath(lib_game);
            egl_lib = ElfScanner::createWithPath(lib_egl);
        } while (!game_lib.isValid() || !egl_lib.isValid());
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "Library %s loaded", lib_game.c_str());

        std::string processName = KittyMemory::getProcessName();
        void *baseAddress = (void*)game_lib.base();
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "Process Name: %s", processName.c_str());
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "Base Address: %p", baseAddress);

        void* enet_host_service_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0x168790C);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "enet_host_service address: %p", enet_host_service_address);

        install_hook_enet_host_service(enet_host_service_address);
        install_hook_eglSwapBuffers(lib_egl.c_str(), "eglSwapBuffers");
    });
    thread.detach();
}
