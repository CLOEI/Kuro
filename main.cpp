#include <android/log.h>
#include <thread>
#include "dobby.h"
#include "KittyInclude.hpp"
#include "definition.hpp"

auto libName = "libgrowtopia.so";
ElfScanner elf;

enet_host_service_t orig_enet_host_service = nullptr;
get_screen_width_t orig_get_screen_width = nullptr;
get_screen_height_t orig_get_screen_height = nullptr;
send_packet_t orig_send_packet = nullptr;
send_packet_raw_t orig_send_packet_raw = nullptr;

int hooked_enet_host_service(ENetHost *host, ENetEvent *event, enet_uint32 timeout) {
  switch (event->type)
  {
  case ENET_EVENT_TYPE_CONNECT:
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "Connected to server");
    break;
  case ENET_EVENT_TYPE_RECEIVE:
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "Received data from server");
    break;
  case ENET_EVENT_TYPE_DISCONNECT:
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "Disconnected from server");
    break;
  default:
    break;
  }
  return orig_enet_host_service(host, event, timeout);
}

int hooked_get_screen_width() {
  return orig_get_screen_width();
}

int hooked_get_screen_height() {
  return orig_get_screen_height();
}

int hooked_send_packet(int type, uint8_t *data, int peer) {
  __android_log_print(ANDROID_LOG_INFO, "Kuro", "send_packet called");
  return orig_send_packet(type, data, peer);
}

int hooked_send_packet_raw(int type, int data, int packet_length, void *a4, int *peer, int flag) {
  __android_log_print(ANDROID_LOG_INFO, "Kuro", "send_packet_raw called");
  return orig_send_packet_raw(type, data, packet_length, a4, peer, flag);
}

void hook_function(void *functionAddress, void *hookedFunction, void **originalFunction, const char *functionName) {
  if (DobbyHook(functionAddress, hookedFunction, originalFunction) == 0) {
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "%s hooked", functionName);
  } else {
    __android_log_print(ANDROID_LOG_ERROR, "Kuro", "Failed to hook %s", functionName);
  }
}

__attribute__((constructor)) void lib_main() {
  std::thread([]() {
    do {
      sleep(1);
      elf = ElfScanner::createWithPath(libName);
    } while (!elf.isValid());

    auto base = elf.base();
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "Base address is %p",
                        (void *)base);
    auto enet_host_service = base + 0x1085259;
    auto get_screen_width = base + 0x10A8089;
    auto get_screen_height = base + 0x10A8095;
    auto send_packet = base + 0xB2B325;
    auto send_packet_raw = base + 0xB2B209;

    __android_log_print(ANDROID_LOG_INFO, "Kuro",
                        "EnetHostService address is %p",
                        (void *)enet_host_service);
    __android_log_print(ANDROID_LOG_INFO, "Kuro",
                        "GetScreenWidth address is %p", (void *)get_screen_width);
    __android_log_print(ANDROID_LOG_INFO, "Kuro",
                        "GetScreenHeight address is %p", (void *)get_screen_height);
    __android_log_print(ANDROID_LOG_INFO, "Kuro",
                        "SendPacket address is %p", (void *)send_packet);
    __android_log_print(ANDROID_LOG_INFO, "Kuro",
                        "SendPacketRaw address is %p", (void *)send_packet_raw);

    hook_function((void *)enet_host_service,
                  (void *)hooked_enet_host_service,
                  (void **)&orig_enet_host_service, "enet_host_service");

    hook_function((void *)get_screen_width,
                  (void *)hooked_get_screen_width,
                  (void **)&orig_get_screen_width, "get_screen_width");
    
    hook_function((void *)get_screen_height,
                  (void *)hooked_get_screen_height,
                  (void **)&orig_get_screen_height, "get_screen_height");
    
    hook_function((void *)send_packet,
                  (void *)hooked_send_packet,
                  (void **)&orig_send_packet, "send_packet");

    hook_function((void *)send_packet_raw,
                  (void *)hooked_send_packet_raw,
                  (void **)&orig_send_packet_raw, "send_packet_raw");
  }).detach();
}