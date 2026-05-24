#include "player_motion.h"
#include "../controller_kind_domain.h"

#if !defined(BANANA_ENGINE_HAS_OPENMP)
#error "Strict parallel runtime requires OpenMP (BANANA_ENGINE_HAS_OPENMP)."
#endif

#include <math.h>
#include <stddef.h>

static float clampf_local(float v, float lo, float hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

static float vec3_len(const float *v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

static void vec3_normalize(float *v)
{
    float len = vec3_len(v);
    if (len <= 1e-6f)
        return;
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

void runtime_player_motion_apply(World *world,
                                 NativePlayerBinding **bindings,
                                 int binding_count,
                                 EntityId primary_player_id,
                                 float move_input_x,
                                 float move_input_z,
                                 const float *forward,
                                 const float *right,
                                 float speed,
                                 float dt,
                                 float island_span,
                                 RuntimeTerrainSampleHeightFn sample_height)
{
    int i = 0;

    if (!world || !bindings || binding_count <= 0 || !forward || !right || !sample_height)
        return;

#pragma omp parallel for schedule(static)
    for (i = 0; i < binding_count; i++)
    {
        NativePlayerBinding *binding = bindings[i];
        Entity *player = NULL;
        float move_x = 0.f;
        float move_z = 0.f;
        float move_dir[3] = {0.f, 0.f, 0.f};

        if (!binding)
            continue;

        player = world_get_entity(world, binding->entity_id);
        move_x = binding->pending_move_x;
        move_z = binding->pending_move_z;

        if (!player || !player->active)
            continue;
        if (runtime_controller_kind_parse_or_unknown(binding->controller_kind) != RUNTIME_CONTROLLER_KIND_HUMAN)
            continue;

        if (fabsf(move_x) <= 1e-4f && fabsf(move_z) <= 1e-4f)
        {
            if (binding->entity_id == primary_player_id)
            {
                move_x = move_input_x;
                move_z = move_input_z;
            }
        }

        if (fabsf(move_x) <= 1e-4f && fabsf(move_z) <= 1e-4f)
            continue;

        move_dir[0] = right[0] * move_x + forward[0] * move_z;
        move_dir[2] = right[2] * move_x + forward[2] * move_z;
        if (vec3_len(move_dir) > 1e-5f)
            vec3_normalize(move_dir);

        player->position[0] += move_dir[0] * speed * dt;
        player->position[2] += move_dir[2] * speed * dt;
        player->position[0] = clampf_local(player->position[0], -island_span, island_span);
        player->position[2] = clampf_local(player->position[2], -island_span, island_span);
        player->position[1] = sample_height(player->position[0], player->position[2]) + 0.55f;

        binding->pending_move_x = 0.f;
        binding->pending_move_z = 0.f;
    }
}