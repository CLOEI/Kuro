#include <dlfcn.h>
#include <thread>
#include <android/log.h>
#include <string>
#include "vendor/KittyMemory/KittyMemory/KittyInclude.hpp"
#include "enet/enet.h"
#include "dobby.h"
#include "types.hpp"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "EGL/egl.h"
#include <GLES3/gl3.h>
#include <arm_neon.h>
#include "packet.hpp"

const std::string lib_game = "libgrowtopia.so";
ElfScanner game_lib;

bool log_host_service = true;
bool log_send_packet = true;
bool log_send_packet_raw = false;
bool init = false;

typedef int (*enet_host_service_t)(ENetHost *host, ENetEvent *event, enet_uint32 timeout);
typedef void (*log_to_console_t)(const char *a1, ...);
typedef __int64_t (*enet_send_packet)(__int64_t result, __uint8_t *a2, __int64_t a3);
typedef __int64_t (*enet_send_packet_raw)(__int64_t result, __int64_t a2, int a3, const void *a4, __int64_t a5, int a6);
typedef __int64_t (*base_app_draw_t)(__int64_t a1);
typedef __int64_t (*get_screen_width_t)();
typedef __int64_t (*get_screen_height_t)();
typedef __int64_t (*native_on_touch_t)(float a1, float a2, __int64_t a3, __int64_t a4, unsigned int a5, int a6);
typedef void (*touch_at_world_coordinate_t)(__int64_t a1, float32x2_t *a2, char a3);
typedef __int64_t (*world_tilemap_collide_t)(__int64_t a1, unsigned int a2, char a3, float a4, float a5, float a6, float a7, float a8, float a9);
typedef float32x2_t (*world_to_screen_t)(float32x2_t *a1, float32x2_t *a2, float32x2_t *a3);
typedef int (*enet_peer_send_t)(ENetPeer *peer, enet_uint8 channelID, ENetPacket *packet);

enet_host_service_t orig_enet_host_service = nullptr;
log_to_console_t orig_log_to_console = nullptr;
enet_send_packet orig_enet_send_packet = nullptr;
enet_send_packet_raw orig_enet_send_packet_raw = nullptr;
base_app_draw_t orig_base_app_draw = nullptr;
get_screen_width_t orig_get_screen_width = nullptr;
get_screen_height_t orig_get_screen_height = nullptr;
native_on_touch_t orig_native_on_touch = nullptr;
touch_at_world_coordinate_t orig_touch_at_world_coordinate = nullptr;
world_tilemap_collide_t orig_collide = nullptr;
world_to_screen_t orig_world_to_screen = nullptr;
enet_peer_send_t orig_enet_peer_send = nullptr;

ENetPeer* peer = nullptr;

void render_menu() {
    if (!init) {
        auto screen_width = orig_get_screen_width();
        auto screen_height = orig_get_screen_height();
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "Screen width|height: %d|%d", screen_width, screen_height);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)screen_width, (float)screen_height);
        io.IniFilename = nullptr;
        ImGui_ImplOpenGL3_Init();
        ImFontConfig font_cfg;
        font_cfg.SizePixels = 32.0f;
        io.Fonts->AddFontDefault(&font_cfg);
        ImGui::StyleColorsDark();
        ImGui::GetStyle().ScaleAllSizes(io.DisplaySize.x / 360.0f);
        init = true;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Kuro", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Checkbox("Log enet_host_service", &log_host_service);
    ImGui::Checkbox("Log enet_send_packet", &log_send_packet);
    ImGui::Checkbox("Log enet_send_packet_raw", &log_send_packet_raw);
    ImGui::End();

    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int hooked_enet_host_service(ENetHost *host, ENetEvent *event, enet_uint32 timeout) {
    bool should_process_packet = true;
    switch (event->type) {
        case ENET_EVENT_TYPE_CONNECT:
            __android_log_print(ANDROID_LOG_INFO, "Kuro", "ENet event type: ENET_EVENT_TYPE_CONNECT");
            peer = event->peer;
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            __android_log_print(ANDROID_LOG_INFO, "Kuro", "ENet event type: ENET_EVENT_TYPE_DISCONNECT");
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            if (event->packet->dataLength < 4) {
                break;
            }
            Packet::handle(event->packet->data, &should_process_packet, log_host_service,
                           event->peer, orig_enet_peer_send);
            break;
        case ENET_EVENT_TYPE_NONE:
            break;
    }
    if (should_process_packet) {
        return orig_enet_host_service(host, event, timeout);
    } else {
        return 0;
    }
}

void hooked_log_to_console(const char *a1, ...) {
    va_list args;
    va_start(args, a1);
    orig_log_to_console(a1, args);
    va_end(args);
}

__int64_t hooked_enet_packet_send(__int64_t result, __uint8_t *a2, __int64_t a3) {
    if (log_send_packet) {
        const uint8_t *data;
        size_t data_size;
        if ((*a2 & 1) != 0) {
            data = *reinterpret_cast<const uint8_t* const*>(a2 + 16);
            data_size = *reinterpret_cast<const uint64_t*>(a2 + 8);
        } else {
            data = a2 + 1;
            data_size = *a2 >> 1;
        }

        auto string_data = std::string(reinterpret_cast<const char*>(data), data_size);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "enet_packet_send: %s", string_data.c_str());
        orig_log_to_console("enet_packet_send: %s", string_data.c_str());
    }
    return orig_enet_send_packet(result, a2, a3);
}

__int64_t hooked_enet_packet_send_raw(__int64_t result, __int64_t a2, int a3, const void *a4, __int64_t a5, int a6) {
    if (log_send_packet_raw) {
        orig_log_to_console("enet_packet_send_raw");
    }
    return orig_enet_send_packet_raw(result, a2, a3, a4, a5, a6);
}

__int64_t hooked_base_app_draw(__int64_t a1) {
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "BaseApp::draw");
    render_menu();
    return orig_base_app_draw(a1);
}

__int64_t hooked_get_screen_width() {
    return orig_get_screen_width();
}

__int64_t hooked_get_screen_height() {
    return orig_get_screen_height();
}

int hooked_enet_peer_send(ENetPeer *peer, enet_uint8 channelID, ENetPacket *packet) {
    return orig_enet_peer_send(peer, channelID, packet);
}

__int64_t hooked_native_on_touch(float x, float y, __int64_t a3, __int64_t a4, unsigned int type, int a6) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(x, y);
    switch (type) {
        case 0:
            io.MouseDown[0] = true;
            break;
        case 1:
            io.MouseDown[0] = false;
            break;
        case 2:
            io.MouseDown[0] = true;
            break;
        default:
            break;
    }
    return orig_native_on_touch(x, y, a3, a4, type, a6);
}

void hooked_touch_at_world_coordinate(__int64_t a1, float32x2_t *a2, char a3) {
    float x = vget_lane_f32(*a2, 0);
    float y = vget_lane_f32(*a2, 1);
    orig_touch_at_world_coordinate(a1, a2, a3);
}

__int64_t hooked_collide(__int64_t a1, unsigned int a2, char a3, float a4, float a5, float a6, float a7, float a8, float a9) {
    return 140736318787792;
}

float32x2_t hooked_world_to_screen(float32x2_t *a1, float32x2_t *a2, float32x2_t *a3) {
    auto result = orig_world_to_screen(a1, a2, a3);
    return result;
}

void hook_function(void* functionAddress, void* hookedFunction, void** originalFunction, const char* functionName) {
    if (DobbyHook(functionAddress, (dobby_dummy_func_t)hookedFunction, (dobby_dummy_func_t*)originalFunction) == 0) {
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "%s hooked", functionName);
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "Kuro", "Failed to hook %s", functionName);
    }
}

__unused __attribute__((constructor))
void lib_main() {
    auto thread = std::thread([]() {
        do {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            game_lib = ElfScanner::createWithPath(lib_game);
        } while (!game_lib.isValid());
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "Library %s loaded", lib_game.c_str());

        std::string processName = KittyMemory::getProcessName();
        void *baseAddress = (void*)game_lib.base();
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "Process Name: %s", processName.c_str());
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "Base Address: %p", baseAddress);

        void* enet_host_service_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0x166E030);
        void* enet_send_packet_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0xFEEEC4);
        void* enet_send_packet_raw_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0xFEECF8);
        void* base_app_draw_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0x1623CD4);
        void* get_screen_width_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0x169ADC0);
        void* get_screen_height_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0x169ADCC);
        void* world_tilemap_collide_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0x106328C);
        void* world_to_screen_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0xCDF730);
        void* enet_peer_send_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0x166C2F8);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "enet_host_service address: %p", enet_host_service_address);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "enet_send_packet address: %p", enet_send_packet_address);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "enet_send_packet_raw address: %p", enet_send_packet_raw_address);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "BaseApp::draw address: %p", base_app_draw_address);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "get_screen_width address: %p", get_screen_width_address);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "get_screen_height address: %p", get_screen_height_address);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "net_moving_collide address: %p", world_tilemap_collide_address);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "world_to_screen address: %p", world_to_screen_address);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "enet_peer_send address: %p", enet_peer_send_address);

        hook_function(enet_host_service_address, (void*)hooked_enet_host_service, (void**)&orig_enet_host_service, "enet_host_service");
//        hook_function(enet_send_packet_address, (void*)hooked_enet_packet_send, (void**)&orig_enet_send_packet, "enet_send_packet");
//        hook_function(enet_send_packet_raw_address, (void*)hooked_enet_packet_send_raw, (void**)&orig_enet_send_packet_raw, "enet_send_packet_raw");
//        hook_function(base_app_draw_address, (void*)hooked_base_app_draw, (void**)&orig_base_app_draw, "BaseApp::draw");
//        hook_function(get_screen_width_address, (void*)hooked_get_screen_width, (void**)&orig_get_screen_width, "get_screen_width");
//        hook_function(get_screen_height_address, (void*)hooked_get_screen_height, (void**)&orig_get_screen_height, "get_screen_height");
        hook_function(world_tilemap_collide_address, (void*)hooked_collide, (void**)&orig_collide, "net_moving_collide");
//        hook_function(world_to_screen_address, (void*)hooked_world_to_screen, (void**)&orig_world_to_screen, "world_to_screen");
        hook_function(enet_peer_send_address, (void*)hooked_enet_peer_send, (void**)&orig_enet_peer_send, "enet_peer_send");
    });
    thread.detach();
}