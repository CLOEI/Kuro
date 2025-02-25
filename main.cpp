#include "dobby.h"
#include "thirdparty/KittyMemory/KittyMemory/KittyInclude.hpp"
#include "thirdparty/enet/include/enet/enet.h"
#include <android/log.h>
#include <thread>

auto libName = "libgrowtopia.so";
ElfScanner elf;

typedef int32_t (*enet_host_service_t)(int32_t *arg1, int32_t *arg2, int32_t arg3);
typedef int (*get_screen_width_t)();

enet_host_service_t orig_enet_host_service = nullptr;
get_screen_width_t orig_get_screen_width = nullptr;

int32_t hooked_enet_host_service(int32_t *host, int32_t *event, int32_t timeout) {
  __android_log_print(ANDROID_LOG_INFO, "Kuro", "Enet host service called");
  return orig_enet_host_service(host, event, timeout);
}

int hooked_get_screen_width() {
  __android_log_print(ANDROID_LOG_INFO, "Kuro", "Get screen width called");
  return orig_get_screen_width();
}

void hook_function(void *functionAddress, void *hookedFunction, void **originalFunction, const char *functionName) {
  if (DobbyHook(functionAddress, hookedFunction, originalFunction) == 0) {
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "%s hooked", functionName);
  } else {
    __android_log_print(ANDROID_LOG_ERROR, "Kuro", "Failed to hook %s",
                        functionName);
  }
}

__attribute__((constructor)) void dll_main() {
  auto thread = std::thread([]() {
    do {
      sleep(1);
      elf = ElfScanner::createWithPath(libName);
    } while (!elf.isValid());

    auto base = elf.base();
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "Base address is %p",
                        (void *)base);
    auto enetHostServiceAddress = base + 0x1085258;
    auto getScreenWidth = base + 0x10A8088;

    __android_log_print(ANDROID_LOG_INFO, "Kuro",
                        "EnetHostService address is %p",
                        (void *)enetHostServiceAddress);
    __android_log_print(ANDROID_LOG_INFO, "Kuro",
                        "GetScreenWidth address is %p", (void *)getScreenWidth);

    hook_function((void *)enetHostServiceAddress,
                  (void *)hooked_enet_host_service,
                  (void **)&orig_enet_host_service, "enet_host_service");
    // hook_function((void *)getScreenWidth, (void *)hooked_get_screen_width,
    //               (void **)&orig_get_screen_width, "get_screen_width");
  });
  thread.detach();
}