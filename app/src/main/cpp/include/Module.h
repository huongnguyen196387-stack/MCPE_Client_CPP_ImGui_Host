#pragma once
#include <string>
#include <utility>

enum class Category { PvP, Performance, Visuals, Settings };

class Module {
public:
    Module(std::string name, std::string description, Category category)
        : name_(std::move(name)), description_(std::move(description)), category_(category) {}
    virtual ~Module() = default;

    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }
    Category category() const { return category_; }
    bool enabled() const { return enabled_; }
    void setEnabled(bool v) { if (enabled_ != v) { enabled_ = v; v ? onEnable() : onDisable(); } }

    virtual void onTick() {}
    virtual void onRenderUI() {}
    virtual void onRender3D() {}
    virtual void onEnable() {}
    virtual void onDisable() {}

protected:
    std::string name_;
    std::string description_;
    Category category_;
    bool enabled_ = false;
};
