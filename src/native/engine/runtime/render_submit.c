#include "render_submit.h"

void runtime_render_submit_frame(Renderer *renderer,
                                 World *world,
                                 Mesh *entity_mesh,
                                 int terrain_initialized,
                                 RuntimeTerrainDrawFn terrain_draw,
                                 RuntimeMaterialResolver resolve_material,
                                 RuntimeMeshResolver resolve_mesh)
{
    if (!renderer)
        return;

    renderer_begin_frame(renderer);

    if (terrain_initialized && terrain_draw)
        terrain_draw();

    if (entity_mesh && world && resolve_material)
    {
        float identity[4] = {0.f, 0.f, 0.f, 1.f};

        for (int j = 0; j < world->entity_count; j++)
        {
            Entity *entity = world->entities[j];
            if (!entity || !entity->active)
                continue;

            Material material = resolve_material(entity);
            Mesh *entity_specific_mesh = resolve_mesh ? resolve_mesh(entity, entity_mesh) : entity_mesh;
            if (!entity_specific_mesh)
                continue;

            renderer_draw_mesh(renderer,
                               entity_specific_mesh,
                               entity->position,
                               identity,
                               entity->scale,
                               &material);
        }
    }

    renderer_end_frame(renderer);
}