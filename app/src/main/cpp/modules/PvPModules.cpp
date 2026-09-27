#include "Module.h"

class CPSCounterModule final : public Module {
public: CPSCounterModule() : Module("CPS Counter", "Displays clicks per second for the host UI.", Category::PvP) {}
};
class KeystrokesModule final : public Module {
public: KeystrokesModule() : Module("Keystrokes HUD", "Displays WASD and mouse button state.", Category::PvP) {}
};
class ArmorStatusModule final : public Module {
public: ArmorStatusModule() : Module("Armor Status", "Shows an armor status placeholder from the game bridge.", Category::PvP) {}
};
class CustomFOVModule final : public Module {
public:
    CustomFOVModule() : Module("Custom FOV", "Changes the requested FOV value in the host settings bridge.", Category::PvP) {}
    void onRenderUI() override;
    float fov = 90.0f;
};

void CustomFOVModule::onRenderUI() {}

// Factory helpers are used by ClientMenu.cpp without exposing implementation types.
extern "C" Module* make_cps() { return new CPSCounterModule(); }
extern "C" Module* make_keys() { return new KeystrokesModule(); }
extern "C" Module* make_armor() { return new ArmorStatusModule(); }
extern "C" Module* make_fov() { return new CustomFOVModule(); }
