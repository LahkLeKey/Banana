#ifndef BANANA_ENGINE_RUNTIME_ENGINE_STATE_H
#define BANANA_ENGINE_RUNTIME_ENGINE_STATE_H

#include "../../ai/controller.h"
#include "../../physics/world.h"
#include "../../render/mesh.h"
#include "../../render/renderer.h"
#include "../../render/window.h"
#include "../../world/world.h"
#include "../input/move_target/move_target_domain.h"
#include "../terrain/terrain_runtime.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BANANA_ENGINE_TERRAIN_SIZE 64
#define BANANA_ENGINE_TERRAIN_MAX_LAYERS 4
#define BANANA_ENGINE_TERRAIN_CHUNK_SIZE 8
#define BANANA_ENGINE_TERRAIN_CHUNK_COLS (BANANA_ENGINE_TERRAIN_SIZE / BANANA_ENGINE_TERRAIN_CHUNK_SIZE)
#define BANANA_ENGINE_TERRAIN_CHUNK_ROWS (BANANA_ENGINE_TERRAIN_SIZE / BANANA_ENGINE_TERRAIN_CHUNK_SIZE)
#define BANANA_ENGINE_TERRAIN_TOTAL_CHUNKS (BANANA_ENGINE_TERRAIN_CHUNK_COLS * BANANA_ENGINE_TERRAIN_CHUNK_ROWS)
#define BANANA_ENGINE_MAX_ACTIVE_CONTROLLERS 256

struct RuntimeApplicationServicePorts;

typedef struct EngineRuntimeState
{
    Window *window;
    Renderer *renderer;
    PhysicsWorld *physics;
    World *world;
    Mesh *entity_mesh;
    Mesh *pbj_pickup_mesh;
    EntityId player_id;

    int engine_initialized;
    int terrain_initialized;
    int viewport_width;
    int viewport_height;
    float camera_eye[3];
    float camera_target[3];
    int camera_valid;
    float move_input_x;
    float move_input_z;
    RuntimeMoveTargetState move_target_state;
    int merchants_seeded;
    int pbj_pickup_collected;
    const struct RuntimeApplicationServicePorts *service_ports;

    unsigned char terrain_height[BANANA_ENGINE_TERRAIN_SIZE][BANANA_ENGINE_TERRAIN_SIZE];
    RuntimeTerrainChunk terrain_chunks[BANANA_ENGINE_TERRAIN_TOTAL_CHUNKS];

    ControllerInstance *controllers[BANANA_ENGINE_MAX_ACTIVE_CONTROLLERS];
    int controller_count;
} EngineRuntimeState;

void runtime_engine_state_reset(EngineRuntimeState *state);

#ifdef __cplusplus
}
#endif

#endif
