#include "runtime/engine_lifecycle.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TERRAIN_SIZE 18
#define TERRAIN_LAYERS 4
#define TERRAIN_CHUNK_SIZE 6
#define TERRAIN_CHUNK_COLS (TERRAIN_SIZE / TERRAIN_CHUNK_SIZE)
#define TERRAIN_CHUNK_ROWS (TERRAIN_SIZE / TERRAIN_CHUNK_SIZE)
#define TERRAIN_TOTAL_CHUNKS (TERRAIN_CHUNK_COLS * TERRAIN_CHUNK_ROWS)

static int s_destroyed_controllers = 0;

static float sample_height_fn(float x, float z)
{
    return (x * 0.05f) + (z * 0.03f);
}

static void controller_destroy_fn(ControllerInstance *self)
{
    if (!self)
        return;

    if (self->state)
        free(self->state);
    s_destroyed_controllers += 1;
}

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    unsigned char height_grid[TERRAIN_SIZE * TERRAIN_SIZE];
    RuntimeTerrainChunk chunks[TERRAIN_TOTAL_CHUNKS];
    World *world = world_create();
    EntityId primary_player = 0;
    ControllerInstance *controllers[4] = {0};
    int controller_count = 0;

    memset(height_grid, 0, sizeof(height_grid));
    memset(chunks, 0, sizeof(chunks));
    runtime_player_registry_reset();

    if (!expect_true("world created", world != NULL))
        return 1;

    if (!expect_true("terrain build",
                     runtime_engine_lifecycle_build_terrain(height_grid,
                                                            TERRAIN_SIZE,
                                                            TERRAIN_LAYERS,
                                                            TERRAIN_CHUNK_SIZE,
                                                            chunks,
                                                            TERRAIN_CHUNK_COLS,
                                                            TERRAIN_CHUNK_ROWS) == 1))
        return 1;

    primary_player = world_spawn_entity(world, ENTITY_TYPE_PLAYER, 0.f, 0.f, 0.f);
    if (!expect_true("bootstrap player",
                     runtime_engine_lifecycle_bootstrap_primary_player(world,
                                                                       primary_player,
                                                                       sample_height_fn) == 1))
        return 1;

    {
        Entity *player = world_get_entity(world, primary_player);
        if (!expect_true("player exists", player != NULL))
            return 1;
        if (!expect_true("player guid", strcmp(player->player_guid, "native-default-player") == 0))
            return 1;
        if (!expect_true("player kind", strcmp(player->controller_kind, "human") == 0))
            return 1;
    }

    if (!expect_true("default registry added", runtime_player_registry_count() == 1))
        return 1;

    if (!expect_true("spawn actors", runtime_engine_lifecycle_spawn_default_actors(world, sample_height_fn, NULL) == 9))
        return 1;
    if (!expect_true("entity count after actors", world->entity_count == 10))
        return 1;

    controllers[0] = (ControllerInstance *)calloc(1, sizeof(ControllerInstance));
    controllers[1] = (ControllerInstance *)calloc(1, sizeof(ControllerInstance));
    if (!expect_true("controllers allocated", controllers[0] != NULL && controllers[1] != NULL))
        return 1;

    controllers[0]->state = malloc(8);
    controllers[1]->state = malloc(8);
    controllers[0]->destroy = controller_destroy_fn;
    controllers[1]->destroy = controller_destroy_fn;
    controller_count = 2;

    runtime_engine_lifecycle_destroy_controllers(controllers, 4, &controller_count);
    if (!expect_true("controller count reset", controller_count == 0))
        return 1;
    if (!expect_true("controller slots nulled", controllers[0] == NULL && controllers[1] == NULL))
        return 1;
    if (!expect_true("controllers destroyed", s_destroyed_controllers == 2))
        return 1;

    runtime_engine_lifecycle_reset_terrain_chunks(chunks, TERRAIN_TOTAL_CHUNKS);
    for (int i = 0; i < TERRAIN_TOTAL_CHUNKS; i++)
    {
        if (!expect_true("chunk dirty reset", chunks[i].dirty == 0u))
            return 1;
        if (!expect_true("chunk mesh reset", chunks[i].mesh == NULL))
            return 1;
    }

    world_destroy(world);
    runtime_player_registry_reset();
    return 0;
}
