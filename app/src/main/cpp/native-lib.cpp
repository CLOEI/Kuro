#include <dlfcn.h>
#include <thread>
#include <android/log.h>
#include <string>
#include "vendor/KittyMemory/KittyMemory/KittyInclude.hpp"
#include "enet/enet.h"
#include "dobby.h"
#include "types.hpp"
#include "imgui.h"
#include "backends/imgui_impl_android.h"
#include "backends/imgui_impl_opengl3.h"
#include "EGL/egl.h"
#include <GLES2/gl2.h>

const std::string lib_game = "libgrowtopia.so";
ElfScanner game_lib;

bool log_host_service = false;
bool log_send_packet = true;
bool log_send_packet_raw = false;
bool init = false;

typedef int (*enet_host_service_t)(ENetHost *host, ENetEvent *event, uint32_t timeout);
typedef void (*log_to_console_t)(const char *a1, ...);
typedef __int64_t (*enet_send_packet)(__int64_t result, __uint8_t *a2, __int64_t a3);
typedef __int64_t (*enet_send_packet_raw)(__int64_t result, __int64_t a2, int a3, const void *a4, __int64_t a5, int a6);
typedef __int64_t (*base_app_draw_t)(__int64_t a1);

enet_host_service_t orig_enet_host_service = nullptr;
log_to_console_t orig_log_to_console = nullptr;
enet_send_packet orig_enet_send_packet = nullptr;
enet_send_packet_raw orig_enet_send_packet_raw = nullptr;
base_app_draw_t orig_base_app_draw = nullptr;

void render_menu() {
    if (!init) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        ImGui_ImplAndroid_Init(nullptr);
        ImGui_ImplOpenGL3_Init("#version 100");
        ImFontConfig font_cfg;
        font_cfg.SizePixels = 22.0f;
        io.Fonts->AddFontDefault(&font_cfg);
        ImGui::StyleColorsDark();
        ImGui::GetStyle().ScaleAllSizes(3.0f);
        init = true;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
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

int hooked_enet_host_service(ENetHost *host, ENetEvent *event, uint32_t timeout) {
    if (log_host_service) {
        switch (event->type) {
            case ENET_EVENT_TYPE_CONNECT:
                orig_log_to_console("ENet event type: ENET_EVENT_TYPE_CONNECT");
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                orig_log_to_console("ENet event type: ENET_EVENT_TYPE_DISCONNECT");
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                orig_log_to_console("Packet data length: %d", event->packet->dataLength);
                break;
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }
    return orig_enet_host_service(host, event, timeout);
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
    render_menu();
    return orig_base_app_draw(a1);
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

        void* enet_host_service_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0x168790C);
        void* log_to_console_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0x119AC74);
        void* enet_send_packet_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0x100B47C);
        void* enet_send_packet_raw_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0x100B2B0);
        void* base_app_draw_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(baseAddress) + 0x163D5B0);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "enet_host_service address: %p", enet_host_service_address);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "log_to_console address: %p", log_to_console_address);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "enet_send_packet address: %p", enet_send_packet_address);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "enet_send_packet_raw address: %p", enet_send_packet_raw_address);
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "BaseApp::draw address: %p", base_app_draw_address);

        hook_function(enet_host_service_address, (void*)hooked_enet_host_service, (void**)&orig_enet_host_service, "enet_host_service");
        hook_function(log_to_console_address, (void*)hooked_log_to_console, (void**)&orig_log_to_console, "log_to_console");
        hook_function(enet_send_packet_address, (void*)hooked_enet_packet_send, (void**)&orig_enet_send_packet, "enet_send_packet");
        hook_function(enet_send_packet_raw_address, (void*)hooked_enet_packet_send_raw, (void**)&orig_enet_send_packet_raw, "enet_send_packet_raw");
        hook_function(base_app_draw_address, (void*)hooked_base_app_draw, (void**)&orig_base_app_draw, "BaseApp::draw");
    });
    thread.detach();
}