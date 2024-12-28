#include <dlfcn.h>
#include <thread>
#include <string>
#include <android/log.h>

const std::string lib_name = "libgrowtopia.so";

__unused __attribute__((constructor))
void lib_main() {
    auto thread = std::thread([]() {
        do {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } while (!dlopen(lib_name.c_str(), RTLD_NOW));
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "Library %s loaded", lib_name.c_str());
    });
    thread.detach();
}