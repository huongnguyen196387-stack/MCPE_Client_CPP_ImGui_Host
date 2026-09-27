#include "ModuleManager.h"

void ModuleManager::tick() { for (auto& m : modules_) if (m->enabled()) m->onTick(); }
void ModuleManager::renderUI(Category category) {
    for (auto& m : modules_) if (m->category() == category) m->onRenderUI();
}
void ModuleManager::render3D() { for (auto& m : modules_) if (m->enabled()) m->onRender3D(); }
std::vector<Module*>& ModuleManager::list(Category category) {
    scratch_.clear();
    for (auto& m : modules_) if (m->category() == category) scratch_.push_back(m.get());
    return scratch_;
}
