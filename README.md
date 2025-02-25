<div align="center">
<h3 align="center">Kuro</h3>
<p align="center">
Growtopia android internal hack
</p>
</div>

### Building & Requirement

- Android NDK

```bash
git submodule update --init
cmake .. -DCMAKE_TOOLCHAIN_FILE=$ndk/build/cmake/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a -DANDROID_PLATFORM=android-24
```
