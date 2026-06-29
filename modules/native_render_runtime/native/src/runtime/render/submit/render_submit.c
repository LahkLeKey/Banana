#include "render_submit.h"

#include <math.h>

static float runtime_render_snap_to_step(float value, float step)
{
    float scaled = 0.0f;
    float snapped = 0.0f;

    if (step <= 0.0f)
        return value;

    scaled = value / step;
    snapped = floorf(scaled + 0.5f) * step;
    if (fabsf(snapped) < 1e-7f)
        return 0.0f;
    return snapped;
}

void runtime_render_apply_viewport_chunk_continuity_transform(RendererDrawCommand *command)
{
    const float position_step = 1.0f / 1024.0f;
    const float scale_step = 1.0f / 2048.0f;

    if (!command)
        return;

    command->position[0] = runtime_render_snap_to_step(command->position[0], position_step);
    command->position[1] = runtime_render_snap_to_step(command->position[1], position_step);
    command->position[2] = runtime_render_snap_to_step(command->position[2], position_step);
    command->scale[0] = runtime_render_snap_to_step(command->scale[0], scale_step);
    command->scale[1] = runtime_render_snap_to_step(command->scale[1], scale_step);
    command->scale[2] = runtime_render_snap_to_step(command->scale[2], scale_step);
}

void runtime_render_submit_frame(Renderer *renderer,
                                 World *world,
                                 Mesh *entity_mesh,
                                 int terrain_initialized,
                                 RuntimeTerrainDrawFn terrain_draw,
                                 RuntimeMaterialResolver resolve_material,
                                 RuntimeMeshResolver resolve_mesh,
                                 RuntimeSceneCommandResolver resolve_command)
{
    if (!renderer)
        return;

    renderer_begin_frame(renderer);

    if (terrain_initialized && terrain_draw)
        terrain_draw();

    if (entity_mesh && world && resolve_material)
    {
        for (int j = 0; j < world->entity_count; j++)
        {
            Entity *entity = world->entities[j];
            RendererDrawCommand command;
            if (!entity || !entity->active)
                continue;

            Material material = resolve_material(entity);
            Mesh *entity_specific_mesh = resolve_mesh ? resolve_mesh(entity, entity_mesh) : entity_mesh;
            if (!entity_specific_mesh)
                continue;

            if (resolve_command)
            {
                resolve_command(entity, entity_specific_mesh, material, &command);
            }
            else
            {
                command.mesh = entity_specific_mesh;
                command.position[0] = entity->position[0];
                command.position[1] = entity->position[1];
                command.position[2] = entity->position[2];
                command.rotation[0] = 0.f;
                command.rotation[1] = 0.f;
                command.rotation[2] = 0.f;
                command.rotation[3] = 1.f;
                command.scale[0] = entity->scale[0];
                command.scale[1] = entity->scale[1];
                command.scale[2] = entity->scale[2];
                command.material = material;
            }

            runtime_render_apply_viewport_chunk_continuity_transform(&command);

            renderer_draw_command(renderer, &command);
        }
    }

    renderer_end_frame(renderer);
}