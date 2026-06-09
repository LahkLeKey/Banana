#include "application_service_render_model_cache.h"

#include "../../gameplay_model_profile.h"

#include <string.h>

typedef struct RuntimeGameplayModelMeshCacheEntry
{
    char model_id[96];
    Mesh *mesh;
} RuntimeGameplayModelMeshCacheEntry;

#define RUNTIME_GAMEPLAY_MODEL_MESH_CACHE_CAPACITY 32

static RuntimeGameplayModelMeshCacheEntry s_model_mesh_cache[RUNTIME_GAMEPLAY_MODEL_MESH_CACHE_CAPACITY];
static int s_model_mesh_cache_count = 0;

Mesh *runtime_application_render_model_mesh_for_gameplay_model_id(const char *model_id)
{
    int index = 0;
    float radius_scale = 0.0f;
    float length_scale = 0.0f;
    float curve_scale = 0.0f;
    float tip_taper = 0.0f;
    int quality = 0;

    if (!model_id || model_id[0] == '\0')
        return NULL;

    for (index = 0; index < s_model_mesh_cache_count; index++)
    {
        if (strcmp(s_model_mesh_cache[index].model_id, model_id) == 0)
            return s_model_mesh_cache[index].mesh;
    }

    if (s_model_mesh_cache_count >= RUNTIME_GAMEPLAY_MODEL_MESH_CACHE_CAPACITY)
        return NULL;

    if (!runtime_gameplay_model_vector_profile_for_model_id(model_id,
                                                            &radius_scale,
                                                            &length_scale,
                                                            &curve_scale,
                                                            &tip_taper,
                                                            &quality))
    {
        return NULL;
    }

    s_model_mesh_cache[s_model_mesh_cache_count].mesh =
        mesh_create_banana_vector(radius_scale,
                                  length_scale,
                                  curve_scale,
                                  tip_taper,
                                  quality);
    if (!s_model_mesh_cache[s_model_mesh_cache_count].mesh)
        return NULL;

    strncpy(s_model_mesh_cache[s_model_mesh_cache_count].model_id,
            model_id,
            sizeof(s_model_mesh_cache[s_model_mesh_cache_count].model_id) - 1);
    s_model_mesh_cache[s_model_mesh_cache_count]
        .model_id[sizeof(s_model_mesh_cache[s_model_mesh_cache_count].model_id) - 1] = '\0';

    s_model_mesh_cache_count += 1;
    return s_model_mesh_cache[s_model_mesh_cache_count - 1].mesh;
}

void runtime_application_render_model_cache_reset(void)
{
    int index = 0;

    for (index = 0; index < s_model_mesh_cache_count; index++)
    {
        if (s_model_mesh_cache[index].mesh)
        {
            mesh_destroy(s_model_mesh_cache[index].mesh);
            s_model_mesh_cache[index].mesh = NULL;
        }
        s_model_mesh_cache[index].model_id[0] = '\0';
    }

    s_model_mesh_cache_count = 0;
}