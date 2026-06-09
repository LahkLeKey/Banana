#ifndef BANANA_ENGINE_RUNTIME_ENGINE_APPLICATION_SERVICE_RENDER_MODEL_CACHE_H
#define BANANA_ENGINE_RUNTIME_ENGINE_APPLICATION_SERVICE_RENDER_MODEL_CACHE_H

#include "../../render/mesh.h"

#ifdef __cplusplus
extern "C"
{
#endif

Mesh *runtime_application_render_model_mesh_for_gameplay_model_id(const char *model_id);
void runtime_application_render_model_cache_reset(void);

#ifdef __cplusplus
}
#endif

#endif