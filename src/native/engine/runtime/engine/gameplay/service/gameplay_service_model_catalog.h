#ifndef BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_MODEL_CATALOG_H
#define BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_MODEL_CATALOG_H

#include "gameplay_service_models.h"

#ifdef __cplusplus
extern "C"
{
#endif

int runtime_gameplay_reinforcement_model_catalog_count(void);

const char *runtime_gameplay_reinforcement_skirmish_model(RuntimeWarReinforcementFamily family,
                                                          RuntimeWarSentienceBehaviorMode behavior_mode,
                                                          int biome_index);

const char *runtime_gameplay_reinforcement_siege_model(RuntimeWarReinforcementFamily family,
                                                       int biome_index);

const char *runtime_gameplay_reinforcement_apex_model(RuntimeWarReinforcementFamily family,
                                                      int biome_index);

const char *runtime_gameplay_reinforcement_mythic_model(RuntimeWarReinforcementFamily family,
                                                        int biome_index);

#ifdef __cplusplus
}
#endif

#endif