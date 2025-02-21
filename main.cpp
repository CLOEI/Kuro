#include <android/log.h>

__unused __attribute__((constructor))
void dll_main() {
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "Kuro test");
}