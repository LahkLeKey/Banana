#ifndef BANANA_ENGINE_RUNTIME_ENGINE_STATE_H
#define BANANA_ENGINE_RUNTIME_ENGINE_STATE_H

#include "../../ai/controller.h"
#include "../../physics/world.h"
#include "../../render/mesh.h"
#include "../../render/renderer.h"
#include "../../render/window.h"
#include "../../world/world.h"
#include "../../../include/banana_launch_gate_policy.h"
#include "../input/move_target/move_target_domain.h"
#include "../terrain/terrain_runtime.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BANANA_ENGINE_TERRAIN_SIZE 64
#define BANANA_ENGINE_TERRAIN_MAX_LAYERS 4
#define BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS 9
#define BANANA_ENGINE_WAR_LIFE_GRID_DIM BANANA_ENGINE_TERRAIN_MAX_LAYERS
#define BANANA_ENGINE_WAR_LIFE_CELL_COUNT (BANANA_ENGINE_WAR_LIFE_GRID_DIM * BANANA_ENGINE_WAR_LIFE_GRID_DIM)
#define BANANA_ENGINE_TERRAIN_CHUNK_SIZE 8
#define BANANA_ENGINE_TERRAIN_CHUNK_COLS (BANANA_ENGINE_TERRAIN_SIZE / BANANA_ENGINE_TERRAIN_CHUNK_SIZE)
#define BANANA_ENGINE_TERRAIN_CHUNK_ROWS (BANANA_ENGINE_TERRAIN_SIZE / BANANA_ENGINE_TERRAIN_CHUNK_SIZE)
#define BANANA_ENGINE_TERRAIN_TOTAL_CHUNKS (BANANA_ENGINE_TERRAIN_CHUNK_COLS * BANANA_ENGINE_TERRAIN_CHUNK_ROWS)
#define BANANA_ENGINE_MAX_ACTIVE_CONTROLLERS 256

typedef enum RuntimeEngineLaunchGateBlockedState
{
    RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_NONE = 0,
    RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_UNLINKED_ACCOUNT = 1,
    RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_SUSPENDED_OR_RESTRICTED = 2,
    RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_STALE_SESSION = 3,
    RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_STEAM_UNAVAILABLE = 4,
    RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_API_UNAVAILABLE = 5,
    RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_OFFLINE_UNVERIFIABLE = 6,
    RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_UNKNOWN_MODE = 7,
} RuntimeEngineLaunchGateBlockedState;

typedef enum RuntimeWarEscalationTier
{
    RUNTIME_WAR_ESCALATION_PEACEFUL = 0,
    RUNTIME_WAR_ESCALATION_SKIRMISH = 1,
    RUNTIME_WAR_ESCALATION_SIEGE = 2,
} RuntimeWarEscalationTier;

typedef enum RuntimeWarSentienceBehaviorMode
{
    RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE = 0,
    RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK = 1,
    RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP = 2,
    RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE = 3,
} RuntimeWarSentienceBehaviorMode;

#define BANANA_ENGINE_WAR_SENTIENCE_MODE_COUNT 4

struct RuntimeApplicationServicePorts;

typedef struct EngineRuntimeState
{
    Window *window;
    Renderer *renderer;
    PhysicsWorld *physics;
    World *world;
    Mesh *entity_mesh;
    Mesh *pbj_pickup_mesh;
    Mesh *banana_team_mesh;
    Mesh *bean_team_mesh;
    Mesh *landmark_mesh;
    Mesh *traversal_mesh;
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
    int launch_gate_allowed;
    banana_launch_gate_reason_code launch_gate_reason_code;
    RuntimeEngineLaunchGateBlockedState launch_gate_blocked_state;
    int war_frontier_chunks;
    int war_biome_stage_index;
    int war_terrain_expand_tick_counter;
    RuntimeWarEscalationTier war_escalation_tier;
    int war_effective_reinforcements_per_tick;
    int war_intelligence_stage;
    int war_intelligence_progress_ticks;
    unsigned int war_biome_unlock_mask;
    int war_reinforcement_spawns_total;
    int war_reinforcement_biome_hits[BANANA_ENGINE_TERRAIN_MAX_LAYERS];
    int war_reinforcement_banana_scout_hits[BANANA_ENGINE_TERRAIN_MAX_LAYERS];
    int war_reinforcement_banana_siege_hits[BANANA_ENGINE_TERRAIN_MAX_LAYERS];
    int war_reinforcement_banana_apex_hits[BANANA_ENGINE_TERRAIN_MAX_LAYERS];
    int war_reinforcement_banana_mythic_hits[BANANA_ENGINE_TERRAIN_MAX_LAYERS];
    int war_reinforcement_bean_raider_hits[BANANA_ENGINE_TERRAIN_MAX_LAYERS];
    int war_reinforcement_bean_warbrute_hits[BANANA_ENGINE_TERRAIN_MAX_LAYERS];
    int war_reinforcement_bean_apex_hits[BANANA_ENGINE_TERRAIN_MAX_LAYERS];
    int war_reinforcement_bean_mythic_hits[BANANA_ENGINE_TERRAIN_MAX_LAYERS];
    int war_reinforcement_banana_apex_stage_hits[BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS];
    int war_reinforcement_banana_mythic_stage_hits[BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS];
    int war_reinforcement_bean_apex_stage_hits[BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS];
    int war_reinforcement_bean_mythic_stage_hits[BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS];
    int war_life_tick_counter;
    int war_life_generation;
    unsigned int war_life_cells;
    int war_life_alive_cells;
    int war_life_frontline_cells;
    int war_procgen_biome_bias;
    int war_sentience_humanoid_index;
    int war_sentience_coordination_level;
    int war_sentience_empathy_level;
    RuntimeWarSentienceBehaviorMode war_sentience_behavior_banana;
    RuntimeWarSentienceBehaviorMode war_sentience_behavior_bean;
    int war_sentience_spawn_mode_hits_banana[BANANA_ENGINE_WAR_SENTIENCE_MODE_COUNT];
    int war_sentience_spawn_mode_hits_bean[BANANA_ENGINE_WAR_SENTIENCE_MODE_COUNT];
    int war_sentience_truce_variant_hits_total;
    int war_sentience_truce_variant_hits_banana;
    int war_sentience_truce_variant_hits_bean;
    int war_sentience_truce_variant_hits_stage[BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS];
    int war_sentience_truce_variant_hits_base;
    int war_sentience_truce_variant_hits_apex;
    int war_sentience_truce_variant_hits_mythic;
    int war_sentience_truce_gate_checks_total;
    int war_sentience_truce_gate_checks_granted;
    int war_sentience_truce_gate_checks_stage[BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS];
    int war_sentience_truce_gate_granted_stage[BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS];
    int war_sentience_truce_gate_block_behavior;
    int war_sentience_truce_gate_block_empathy;
    int war_sentience_truce_gate_block_coordination;
    int war_sentience_truce_gate_block_streak;
    int war_sentience_truce_gate_block_intelligence;
    int war_sentience_negotiate_streak_ticks;
    int war_sentience_negotiate_deescalation_trim_last_tick;
    int war_sentience_comeback_bonus_last_tick;

    unsigned char terrain_height[BANANA_ENGINE_TERRAIN_SIZE][BANANA_ENGINE_TERRAIN_SIZE];
    RuntimeTerrainChunk terrain_chunks[BANANA_ENGINE_TERRAIN_TOTAL_CHUNKS];

    ControllerInstance *controllers[BANANA_ENGINE_MAX_ACTIVE_CONTROLLERS];
    int controller_count;
} EngineRuntimeState;

RuntimeEngineLaunchGateBlockedState
runtime_engine_state_launch_gate_blocked_state_from_reason_code(banana_launch_gate_reason_code reason_code);

void runtime_engine_state_apply_launch_gate_decision(EngineRuntimeState *state,
                                                     int allow,
                                                     banana_launch_gate_reason_code reason_code);

void runtime_engine_state_reset(EngineRuntimeState *state);

#ifdef __cplusplus
}
#endif

#endif
