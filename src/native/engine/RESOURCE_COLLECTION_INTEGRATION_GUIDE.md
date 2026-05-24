/**
 * @file RESOURCE_COLLECTION_INTEGRATION_GUIDE.md
 * @brief Integration guide for resource collection system
 *
 * This document explains how the three resource collection systems integrate
 * to provide playable resource gathering mechanics.
 */

# Resource Collection System Integration Guide

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Player Interaction                        │
└────────────────┬────────────────────────────────────────────┘
                 │
         Player clicks tree (INPUT_CLICK)
                 │
                 ▼
┌─────────────────────────────────────────────────────────────┐
│            Input Binding System                              │
│  (existing: routes click to registered callbacks)            │
└────────────────┬────────────────────────────────────────────┘
                 │
         Dispatch callback to collection handler
                 │
                 ▼
┌─────────────────────────────────────────────────────────────┐
│        Interactive Collection System                         │
│  • Find collectible object (object_id)                      │
│  • Check: depleted? on cooldown?                            │
│  • Register resource reward                                  │
└────────────────┬────────────────────────────────────────────┘
                 │
         Update object state, reward player
                 │
                 ▼
┌─────────────────────────────────────────────────────────────┐
│        Player Resources System                               │
│  • Increment player resource balance                         │
│  • Mark for sync payload                                     │
│  • Atomic operations for multi-player trades                │
└────────────────┬────────────────────────────────────────────┘
                 │
         Player receives wood/ore/stone
                 │
                 ▼
        Send sync to client (API layer)
```

## System Initialization Sequence

```c
// Step 1: Initialize terrain and resource systems
terrain_chunks_init(seed, cache_size);
player_resources_init();
interactive_collection_init();
interactive_object_spawner_init(max_rules);

// Step 2: Register spawn rules (what objects appear in which biomes)
BiomeSpawnRule forest_rule = {
    .biome = BIOME_FOREST,
    .object_type_id = 1,
    .resource_type = RESOURCE_WOOD,
    .reward_amount = 10,
    .max_collections = 3,
    .cooldown_ms = 500,
    .spawn_density = 0.5f  // 50% of forest tiles
};
interactive_object_spawner_register_rule(&forest_rule);

// Step 3: Register input binding callback to handle clicks
input_binding_register_callback(INPUT_CLICK, on_object_click_handler);

// Step 4: When chunk is generated, spawn objects
const TerrainChunk *chunk = terrain_chunks_get(chunk_x, chunk_z);
if (chunk) {
    interactive_object_spawner_spawn_chunk(chunk, chunk_x, chunk_z);
}
```

## Player Collection Flow

```c
// When player clicks on a tree:

int object_id = 1000;  // Unique ID assigned at spawn
const char *player_guid = "player-uuid-123";

int result = interactive_collection_collect(object_id, player_guid);

if (result == COLLECTION_OK) {
    // Get updated resources for sync
    int resources[RESOURCE_COUNT];
    player_resources_get_snapshot(player_guid, resources);
    // Send to client: {wood: resources[0], ore: resources[1], stone: resources[2]}
}
else if (result == COLLECTION_OBJECT_DEPLETED) {
    // Tree is exhausted - need respawn or different tree
}
else if (result == COLLECTION_OBJECT_COOLDOWN) {
    // Too soon - cooldown still active
}
```

## Data Flow for Multiplayer Sync

```
Server State                          Client State
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Terrain Chunks                        Chunk Cache
(generated server-side)      ──────→  (received from API)
  64x64 heightfield
  64x64 biome map

Interactive Objects                  Actor Mesh Registry
(spawned via spawner)        ──────→  (selected via LOD)
  object_id 1000-2000
  tile positions

Player Resources                      UI Display
(track inventory)            ──────→  (show wood/ore/stone)
  player_guid → [10, 5, 2]
```

## Integration with Input Binding System

```c
// In your click handler callback:

void on_object_click_handler(ClickEventData *click) {
    int object_id = click->object_id;
    const char *player_guid = click->player_guid;
    
    // Attempt collection
    int result = interactive_collection_collect(object_id, player_guid);
    
    if (result == COLLECTION_OK) {
        // Success - get updated inventory
        int resources[RESOURCE_COUNT];
        player_resources_get_snapshot(player_guid, resources);
        
        // TODO: Prepare sync packet for API layer
        // Send: {event: "resource_collected", 
        //        resources: {wood, ore, stone},
        //        object_id: 1000}
    }
}

// Register with input binding
input_binding_register_callback(INPUT_CLICK, 
    (InputCallback)on_object_click_handler);
```

## Resource Respawning

Objects can be respawned after depletion (e.g., after time passes):

```c
// After some time, respawn object
interactive_collection_respawn(object_id);

// Object is now collectible again with full hit count
```

## Performance Considerations

| Aspect | Value | Notes |
|--------|-------|-------|
| Max Players | 256 | Global registry limit |
| Max Collectible Objects | 1024 | System-wide capacity |
| Max Spawned Objects | 4096 | Per-chunk scalable |
| Typical Forest Chunk | ~800 objects | At 50% density, 64x64 tiles |
| Memory per Player | ~16 bytes | 3 ints + tick timestamp |
| Memory per Object | ~32 bytes | ID + resource info + cooldown |

## Testing Spawned Objects

```c
// Verify spawning works:
int count = interactive_object_spawner_count();
printf("Spawned objects: %d\n", count);

// Verify collection:
TerrainChunk chunk = terrain_chunks_get(0, 0);
interactive_object_spawner_spawn_chunk(&chunk, 0, 0);

int result = interactive_collection_collect(1000, "test_player");
assert(result == COLLECTION_OK);

int balance = player_resources_get_balance("test_player", RESOURCE_WOOD);
printf("Player wood: %d\n", balance);  // Should be > 0
```

## Next: API Bridge

When connecting to TypeScript API layer:
1. Implement route: `GET /api/world/resources` → returns snapshot
2. Implement route: `POST /api/world/objects/:id/collect` → triggers collection
3. Implement WebSocket event: `world:chunk-generated` → spawn objects
4. Subscribe to player position updates → center chunk generation

## Related Files

- **Implementation**: `src/native/engine/runtime/player/player_resources.{h,c}`
- **Implementation**: `src/native/engine/runtime/interaction/collection/interactive_collection.{h,c}`
- **Implementation**: `src/native/engine/runtime/interaction/spawner/interactive_object_spawner.{h,c}`
- **Tests**: `tests/native/player_resources_test.c` (28 tests)
- **Tests**: `tests/native/runtime/interaction/collection/interactive_collection_test.c` (26 tests)
- **Tests**: `tests/native/runtime/interaction/spawner/interactive_object_spawner_test.c` (14 tests)
- **Existing Systems**: `src/native/engine/runtime/input/binding/input_binding.{h,c}`
- **Existing Systems**: `src/native/engine/runtime/terrain/terrain_chunks.{h,c}`

