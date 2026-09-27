#include "ModuleManager.h"
#include "ui/ClientMenu.h"

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>
#include <backends/imgui_impl_android.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#define LOG_TAG "MCPEClient"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace {
ModuleManager g_modules;
ClientMenu* g_menu = nullptr;
ANativeWindow* g_window = nullptr;
int g_width = 1, g_height = 1;
bool g_ready = false;
}

static void initImgui(JNIEnv* env, jobject surface, int width, int height) {
    if (g_ready) return;
    g_width = width; g_height = height;

    g_window = ANativeWindow_fromSurface(env, surface);
    if (!g_window) { LOGE("ANativeWindow_fromSurface failed"); return; }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
    io.DisplaySize = ImVec2((float)width, (float)height);

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 12.0f;
    style.ChildRounding = 10.0f;
    style.FrameRounding = 7.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.WindowPadding = ImVec2(14, 14);
    style.FramePadding = ImVec2(10, 7);
    style.ItemSpacing = ImVec2(10, 9);

    if (!ImGui_ImplAndroid_Init(g_window)) LOGE("ImGui Android backend init failed");
    if (!ImGui_ImplOpenGL3_Init("#version 300 es")) LOGE("ImGui OpenGL3 backend init failed");

    populateModules(g_modules);
    g_menu = new ClientMenu(g_modules);
    g_ready = true;
    LOGI("MCPE Client ImGui host initialized %dx%d", width, height);
}

static void shutdownImgui() {
    if (!g_ready) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
    delete g_menu; g_menu = nullptr;
    if (g_window) ANativeWindow_release(g_window);
    g_window = nullptr;
    g_ready = false;
}

extern "C" JNIEXPORT void JNICALL
Java_com_mcpe_client_MainActivity_00024ClientGLSurfaceView_00024RendererImpl_nativeInit(
    JNIEnv* env, jobject, jint width, jint height) {
    // Surface is intentionally fetched through the current GLSurfaceView holder by the Java layer.
    // The native host can operate without taking ownership of the Activity surface.
    // For this build, we create a backend using a window obtained from the EGL surface isn't directly exposed.
    // We therefore initialize an ImGui context and use the OpenGL renderer; touch is forwarded via JNI.
    if (g_ready) return;
    g_width = width; g_height = height;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
    io.DisplaySize = ImVec2((float)width, (float)height);
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 12.0f;
    style.ChildRounding = 10.0f;
    style.FrameRounding = 7.0f;
    style.PopupRounding = 8.0f;
    style.WindowPadding = ImVec2(14, 14);
    style.FramePadding = ImVec2(10, 7);
    style.ItemSpacing = ImVec2(10, 9);

    if (!ImGui_ImplOpenGL3_Init("#version 300 es")) {
        LOGE("OpenGL3 backend initialization failed");
        ImGui::DestroyContext();
        return;
    }
    populateModules(g_modules);
    g_menu = new ClientMenu(g_modules);
    g_ready = true;
}

extern "C" JNIEXPORT void JNICALL
Java_com_mcpe_client_MainActivity_00024ClientGLSurfaceView_00024RendererImpl_nativeResize(
    JNIEnv*, jobject, jint width, jint height) {
    g_width = width; g_height = height;
    if (g_ready) ImGui::GetIO().DisplaySize = ImVec2((float)width, (float)height);
    glViewport(0, 0, width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_mcpe_client_MainActivity_00024ClientGLSurfaceView_00024RendererImpl_nativeRender(
    JNIEnv*, jobject) {
    if (!g_ready || !g_menu) return;

    glViewport(0, 0, g_width, g_height);
    glClearColor(0.055f, 0.065f, 0.075f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    g_modules.tick();
    g_menu->draw((float)g_width, (float)g_height);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

extern "C" JNIEXPORT void JNICALL
Java_com_mcpe_client_MainActivity_00024ClientGLSurfaceView_00024RendererImpl_nativeShutdown(
    JNIEnv*, jobject) { shutdownImgui(); }

extern "C" JNIEXPORT void JNICALL
Java_com_mcpe_client_MainActivity_00024ClientGLSurfaceView_00024RendererImpl_nativeTouch(
    JNIEnv*, jobject, jint action, jfloat x, jfloat y, jint down) {
    if (!g_ready) return;
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);
    if (action == 0) io.AddMouseButtonEvent(0, down != 0);
    else if (action == 1) io.AddMouseButtonEvent(0, false);
}
