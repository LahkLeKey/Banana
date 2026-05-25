#ifndef BANANA_ENGINE_WORLD_H
#define BANANA_ENGINE_WORLD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "entity.h"

#define BANANA_MAX_ENTITIES 2048

    typedef struct World
    {
        Entity *entities[BANANA_MAX_ENTITIES];
        int entity_count;
        uint32_t next_entity_id;
    } World;

    /* Global singleton — the game loop works with one world at a time. */
    World *world_get(void);

    World *world_create(void);

    EntityId world_spawn_entity(World *w, EntityType type, float x, float y, float z);

    Entity *world_get_entity(World *w, EntityId id);

    void world_despawn_entity(World *w, EntityId id);

    /* Tick: update physics bodies → AI controllers → sync positions. */
    void world_tick(World *w, float dt);

    /* Spatial query: fill out_ids with entity IDs within radius of position.
     * Returns count found (up to max_results). */
    int world_query_nearby(const World *w, const float *position, float radius, EntityId *out_ids,
                           int max_results);

    void world_destroy(World *w);

    /* ── C ABI exports ────────────────────────────────────────────────────────── */
    uint32_t engine_world_spawn(int entity_type, float x, float y, float z);
    void engine_world_tick(float dt);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_WORLD_H */
