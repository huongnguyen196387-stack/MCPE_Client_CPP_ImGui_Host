#pragma once
#include "ModuleManager.h"

class ClientMenu {
public:
    explicit ClientMenu(ModuleManager& manager) : manager_(manager) {}
    void draw(float width, float height);
    bool visible() const { return visible_; }
private:
    ModuleManager& manager_;
    bool visible_ = true;
    int tab_ = 0;
    bool showDemo_ = false;
    float uiScale_ = 1.0f;
    float opacity_ = 0.96f;
};

void populateModules(ModuleManager& manager);
