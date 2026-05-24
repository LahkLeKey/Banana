#include "parallel_contract.h"

static const RuntimeParallelBudget kRuntimeParallelBudgets[] = {
    {"ai.dispatch.update", "src/native/engine/ai/controller_system_dispatch.c", 16, 0.60},
    {"ai.dispatch.signal", "src/native/engine/ai/controller_system_dispatch.c", 28, 0.58},
    {"ai.dispatch.detach", "src/native/engine/ai/controller_system_dispatch.c", 41, 0.58},
    {"tools.asset.generate.layers", "src/native/engine/tools/application/banana_asset_generation_service.c", 58, 0.62},
    {"physics.dynamics.integrate", "src/native/engine/physics/dynamics.c", 54, 0.45},
    {"physics.world_collision.sweep", "src/native/engine/physics/world_collision.c", 15, 0.40},
    {"world.query.nearby", "src/native/engine/world/world_query.c", 26, 0.52},
    {"runtime.controller_sync.update", "src/native/engine/runtime/controller/controller_sync.c", 55, 0.50},
    {"runtime.controller_sync.entity_sync", "src/native/engine/runtime/controller/controller_sync.c", 75, 0.45},
    {"render.mesh_terrain.vertices", "src/native/engine/render/mesh_terrain.c", 30, 0.55},
    {"render.mesh_terrain.normals", "src/native/engine/render/mesh_terrain.c", 74, 0.55},
    {"render.renderer.instance_upload", "src/native/engine/render/renderer.c", 259, 0.64},
    {"runtime.physics_abi.sync", "src/native/engine/runtime/abi/physics_abi.c", 39, 0.48},
    {"tools.asset.cellular.step", "src/native/engine/tools/domain/banana_asset_cellular.c", 41, 0.60},
    {"tools.asset.wfc.entropy", "src/native/engine/tools/domain/banana_asset_wfc.c", 107, 0.63},
    {"tools.asset.wfc.propagate", "src/native/engine/tools/domain/banana_asset_wfc.c", 141, 0.63},
    {"runtime.player_motion.integrate", "src/native/engine/runtime/player/player_motion.c", 52, 0.47},
    {"runtime.terrain_generation.samples", "src/native/engine/runtime/terrain/terrain_generation.c", 73, 0.43},
    {"runtime.terrain_runtime.sample", "src/native/engine/runtime/terrain/terrain_runtime.c", 61, 0.44},
    {"runtime.terrain_runtime.rebuild", "src/native/engine/runtime/terrain/terrain_runtime.c", 86, 0.52},
    {"runtime.world_metrics.active_count", "src/native/engine/runtime/world/world_metrics.c", 17, 0.36},
};

const RuntimeParallelBudget *runtime_parallel_contract_entries(int *count)
{
    if (count)
        *count = (int)(sizeof(kRuntimeParallelBudgets) / sizeof(kRuntimeParallelBudgets[0]));

    return kRuntimeParallelBudgets;
}

int runtime_parallel_contract_expected_inventory_size(void)
{
    return (int)(sizeof(kRuntimeParallelBudgets) / sizeof(kRuntimeParallelBudgets[0]));
}
