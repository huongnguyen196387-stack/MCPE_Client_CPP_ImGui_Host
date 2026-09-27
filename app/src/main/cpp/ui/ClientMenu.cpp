#include "ClientMenu.h"
#include <imgui.h>
#include <memory>

extern "C" Module* make_cps();
extern "C" Module* make_keys();
extern "C" Module* make_armor();
extern "C" Module* make_fov();
extern "C" Module* make_culling();
extern "C" Module* make_chunk();

namespace {
const char* categoryName(Category c) {
    switch (c) {
        case Category::PvP: return "PvP";
        case Category::Performance: return "Performance";
        case Category::Visuals: return "Visuals";
        default: return "Settings";
    }
}
void drawModule(Module* m) {
    if (!m) return;
    ImGui::PushID(m);
    bool enabled = m->enabled();
    if (ImGui::Checkbox(m->name().c_str(), &enabled)) m->setEnabled(enabled);
    ImGui::SameLine();
    ImGui::TextDisabled("%s", m->description().c_str());
    ImGui::PopID();
    ImGui::Spacing();
}
}

void ClientMenu::draw(float width, float height) {
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = uiScale_;

    ImGui::SetNextWindowPos(ImVec2(width * 0.06f, height * 0.08f), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(width * 0.88f, height * 0.84f), ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(opacity_);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
    static const char* tabs[] = {"PvP", "Performance", "Visuals", "Settings"};
    if (ImGui::Begin("MCPE CLIENT  •  Android Host", &visible_, flags)) {
        const float sidebarW = 150.0f;
        ImGui::BeginChild("Sidebar", ImVec2(sidebarW, 0), true);
        ImGui::TextUnformatted("MCPE");
        ImGui::TextDisabled("CLIENT HOST");
        ImGui::Separator();
        for (int i = 0; i < 4; ++i) {
            if (ImGui::Selectable(tabs[i], tab_ == i, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, 42))) tab_ = i;
        }
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 62);
        ImGui::Separator();
        ImGui::TextDisabled("Host mode");
        ImGui::TextWrapped("Standalone OpenGL ES 3");
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("Content", ImVec2(0, 0), false);
        ImGui::Text("%s", tabs[tab_]);
        ImGui::Separator();

        Category cat = static_cast<Category>(tab_);
        if (tab_ < 3) {
            for (Module* m : manager_.list(cat)) drawModule(m);
        } else {
            ImGui::TextWrapped("This build is a standalone visual host. No Bedrock offsets, signatures or injector are embedded.");
            ImGui::Spacing();
            ImGui::SliderFloat("UI scale", &uiScale_, 0.80f, 1.40f, "%.2fx");
            ImGui::SliderFloat("Panel opacity", &opacity_, 0.75f, 1.00f, "%.2f");
            ImGui::Checkbox("ImGui demo window", &showDemo_);
            if (ImGui::Button("Reset module state")) {
                for (const auto& p : manager_.modules()) p->setEnabled(false);
            }
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextDisabled("Architecture");
            ImGui::BulletText("C++20 + Android NDK");
            ImGui::BulletText("Dear ImGui + OpenGL ES 3");
            ImGui::BulletText("JNI / GLSurfaceView host");
            ImGui::BulletText("GameBridge integration points remain explicit");
        }
        ImGui::EndChild();
    }
    ImGui::End();

    if (showDemo_) ImGui::ShowDemoWindow(&showDemo_);
}

void populateModules(ModuleManager& manager) {
    manager.addOwned(std::unique_ptr<Module>(make_cps()));
    manager.addOwned(std::unique_ptr<Module>(make_keys()));
    manager.addOwned(std::unique_ptr<Module>(make_armor()));
    manager.addOwned(std::unique_ptr<Module>(make_fov()));
    manager.addOwned(std::unique_ptr<Module>(make_culling()));
    manager.addOwned(std::unique_ptr<Module>(make_chunk()));

    class FPSHUD final : public Module { public: FPSHUD() : Module("FPS HUD", "Shows current render FPS.", Category::Visuals) {} };
    class Crosshair final : public Module { public: Crosshair() : Module("Crosshair", "Simple centered crosshair overlay.", Category::Visuals) {} };
    manager.add<FPSHUD>();
    manager.add<Crosshair>();
}
