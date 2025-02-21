#include <android/log.h>

int main() {
    __android_log_print(ANDROID_LOG_INFO, "Kuro", "Kuro test");
    return 0;
}