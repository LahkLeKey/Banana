#include "world.h"
#include "signals.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static World *s_world = NULL;

World *world_get(void) { return s_world; }

World *world_create(void) {
    World *w = calloc(1, sizeof(World));
    w->next_entity_id = 1;
    s_world = w;
    return w;
}

EntityId world_spawn_entity(World *w, EntityType type, float x, float y, float z) {
    if (w->entity_count >= BANANA_MAX_ENTITIES) return 0;
    EntityId id = w->next_entity_id++;
    w->entities[w->entity_count++] = entity_create(id, type, x, y, z);
    return id;
}

Entity *world_get_entity(World *w, EntityId id) {
    for (int i = 0; i < w->entity_count; i++)
        if (w->entities[i] && w->entities[i]->id == id)
            return w->entities[i];
    return NULL;
}

void world_despawn_entity(World *w, EntityId id) {
    for (int i = 0; i < w->entity_count; i++) {
        if (w->entities[i] && w->entities[i]->id == id) {
            entity_destroy(w->entities[i]);
            w->entities[i] = w->entities[--w->entity_count];
            return;
        }
    }
}

void world_tick(World *w, float dt) {
    (void)dt;
    /* Deliver queued signals first */
    signals_flush(w);
    /* Future: update physics bodies, AI controllers */
}

int world_query_nearby(const World *w, const float *position, float radius,
                        EntityId *out_ids, int max_results) {
    int found = 0;
    for (int i = 0; i < w->entity_count && found < max_results; i++) {
        Entity *e = w->entities[i];
        if (!e || !e->active) continue;
        float dx = e->position[0] - position[0];
        float dy = e->position[1] - position[1];
        float dz = e->position[2] - position[2];
        if (sqrtf(dx*dx + dy*dy + dz*dz) <= radius)
            out_ids[found++] = e->id;
    }
    return found;
}

void world_destroy(World *w) {
    if (!w) return;
    for (int i = 0; i < w->entity_count; i++)
        entity_destroy(w->entities[i]);
    if (s_world == w) s_world = NULL;
    free(w);
}

/* engine_world_spawn / engine_world_tick are WASM ABI entry points
 * that live in engine.c (they need the global singleton s_world). */
