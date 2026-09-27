# MCPE Client CPP ImGui Host — Android

Bản Android host độc lập dùng C++20, Dear ImGui và OpenGL ES 3. APK này hiển thị menu ImGui thật ngay khi mở ứng dụng; không cần Minecraft chạy.

## Menu
- PvP: CPS Counter, Keystrokes HUD, Armor Status, Custom FOV
- Performance: Render Culling, Chunk Update Optimizer
- Visuals: FPS HUD, Crosshair
- Settings: UI scale, opacity, reset state, ImGui demo

## Kiến trúc
`MainActivity.java` → `GLSurfaceView` → JNI → `libmcpe_client.so` → Dear ImGui/OpenGL ES 3.

Project cố ý không chứa offset, signature, byte patch, ptrace hoặc injector cho Minecraft/Bedrock. Các điểm tích hợp game-specific có thể được nối sau bằng một bridge API riêng.

## Build trên Termux
Cần Android SDK, NDK 27.2.12479018, CMake 3.22.1 và JDK 21.

Từ thư mục project:

```bash
./build-termux.sh
```

Nếu build script báo thiếu Gradle, chạy:

```bash
pkg install -y gradle
./build-termux.sh
```

CMake sẽ tải Dear ImGui `v1.92.2b` tại bước configure. Đây là release chính thức của Dear ImGui. 

APK debug sẽ nằm tại:
`app/build/outputs/apk/debug/app-debug.apk`

## Lưu ý
Môi trường hiện tại không có Android SDK/NDK nên không thể tạo ra binary APK trong phiên làm việc này; source/build script ở đây đã được chuẩn bị cho máy Termux có SDK/NDK.
