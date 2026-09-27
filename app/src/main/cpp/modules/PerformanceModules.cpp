#include "Module.h"

class RenderCullingModule final : public Module {
public:
    RenderCullingModule() : Module("Render Culling", "Frustum + AABB visibility gate with a configurable max distance.", Category::Performance) {}
    float maxDistance = 96.0f;
};
class ChunkUpdateOptimizerModule final : public Module {
public:
    ChunkUpdateOptimizerModule() : Module("Chunk Update Optimizer", "Defers far mesh work to keep frame pacing stable.", Category::Performance) {}
    float distance = 96.0f;
};
extern "C" Module* make_culling() { return new RenderCullingModule(); }
extern "C" Module* make_chunk() { return new ChunkUpdateOptimizerModule(); }
