#include <thread>

#include "thirdparty/KittyMemory/KittyMemory/KittyInclude.hpp"
#include "thirdparty/enet/include/enet/enet.h"
#include <android/log.h>

#include "dobby.h"

auto libName = "libgrowtopia.so";
ElfScanner elf;

typedef int (*enet_host_service_t)(ENetHost *host, ENetEvent *event, enet_uint32 timeout);
typedef int (*get_screen_width_t)();

enet_host_service_t orig_enet_host_service = nullptr;
get_screen_width_t orig_get_screen_width = nullptr;

int hooked_enet_host_service(ENetHost *host, ENetEvent *event, enet_uint32 timeout) {
  __android_log_print(ANDROID_LOG_INFO, "Kuro", "Enet host service called");
  return orig_enet_host_service(host, event, timeout);
}

int hooked_get_screen_width() {
  __android_log_print(ANDROID_LOG_INFO, "Kuro", "Get screen width called");
  return orig_get_screen_width();
}

void hook_function(void* functionAddress, void* hookedFunction, void** originalFunction, const char* functionName) {
  if (DobbyHook(functionAddress, hookedFunction, originalFunction) == 0) {
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "%s hooked", functionName);
  } else {
    __android_log_print(ANDROID_LOG_ERROR, "Kuro", "Failed to hook %s", functionName);
  }
}

__attribute__((constructor)) void dll_main() {
  auto thread = std::thread([]() {
    do {
      sleep(1);
      elf = ElfScanner::createWithPath(libName);
    } while (!elf.isValid());

    auto base = elf.base();
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "Base address is %p", (void*)base);
    auto logMsgAddress = base + 0x102D838;
    auto enetHostServiceAddress = base + 0x1085258;
    auto getScreenWidth = base + 0x10A8088;
    auto positionXAddress = base + 0x1646960;
    auto positionYAddress = base + 0x1646964;
    auto invisAddress = base + 0x1646A7C;
    auto mstateAddress = base + 0x1646A7D;

    __android_log_print(ANDROID_LOG_INFO, "Kuro", "LogMsg address is %p", (void*)logMsgAddress);
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "EnetHostService address is %p", (void*)enetHostServiceAddress);
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "GetScreenWidth address is %p", (void*)getScreenWidth);
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "PositionX address is %p", (void*)positionXAddress);
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "PositionY address is %p", (void*)positionYAddress);
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "Invis address is %p", (void*)invisAddress);
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "MState address is %p", (void*)mstateAddress);

    // hook_function((void *)enetHostServiceAddress, (void*)hooked_enet_host_service, (void**)&orig_enet_host_service, "enet_host_service");
    hook_function((void *)getScreenWidth, (void*)hooked_get_screen_width, (void**)&orig_get_screen_width, "get_screen_width");
    // __android_log_print(ANDROID_LOG_INFO, "Kuro", "Screen width is %d", orig_get_screen_width());
  });
  thread.detach();
}