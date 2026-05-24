#include "render_submit.h"

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

            renderer_draw_command(renderer, &command);
        }
    }

    renderer_end_frame(renderer);
}