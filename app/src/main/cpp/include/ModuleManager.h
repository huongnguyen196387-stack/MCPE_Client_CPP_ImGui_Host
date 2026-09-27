#pragma once
#include "Module.h"
#include <memory>
#include <vector>
#include <type_traits>
#include <utility>

class ModuleManager {
public:
    void addOwned(std::unique_ptr<Module> p) { modules_.push_back(std::move(p)); }

    template<class T, class... Args>
    T* add(Args&&... args) {
        auto p = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = p.get();
        modules_.push_back(std::move(p));
        return raw;
    }

    void tick();
    void renderUI(Category category);
    void render3D();
    std::vector<Module*>& list(Category category);
    const std::vector<std::unique_ptr<Module>>& modules() const { return modules_; }

private:
    std::vector<std::unique_ptr<Module>> modules_;
    std::vector<Module*> scratch_;
};
