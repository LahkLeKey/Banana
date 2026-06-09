#ifndef BANANA_ENGINE_RUNTIME_APPLICATION_SERVICE_PORTS_H
#define BANANA_ENGINE_RUNTIME_APPLICATION_SERVICE_PORTS_H

#include "../../engine_state.h"
#include "../../../merchant/service/merchant_service.h"
#include "../../../player/player_motion.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeTerrainServicePort
    {
        float (*sample_height)(const EngineRuntimeState *state, float x, float z);
        int (*rebuild_dirty_chunks)(EngineRuntimeState *state, int max_chunks);
        void (*draw)(EngineRuntimeState *state);
        int (*set_height)(EngineRuntimeState *state, int x, int z, int elevation);
        void (*mark_region_dirty)(EngineRuntimeState *state,
                                  int min_x,
                                  int min_z,
                                  int max_x,
                                  int max_z);
    } RuntimeTerrainServicePort;

    typedef struct RuntimePlayerServicePort
    {
        void (*update_motion)(EngineRuntimeState *state,
                              float dt,
                              RuntimeTerrainSampleHeightFn sample_height);
        void (*follow_camera)(EngineRuntimeState *state);
        void (*apply_click_input)(EngineRuntimeState *state,
                                  float normalized_x,
                                  float normalized_y,
                                  RuntimeTerrainSampleHeightFn sample_height);
    } RuntimePlayerServicePort;

    typedef struct RuntimeMerchantServicePort
    {
        int (*get_price)(int npc_id,
                         const char *item_type,
                         int *inout_seeded,
                         int *out_price);

        int (*trade_buy)(int npc_id,
                         const char *item_type,
                         int quantity,
                         int *inout_seeded,
                         RuntimeMerchantResourceGateway resource_gateway);

        int (*trade_sell)(int npc_id,
                          const char *item_type,
                          int quantity,
                          int *inout_seeded,
                          RuntimeMerchantResourceGateway resource_gateway);
    } RuntimeMerchantServicePort;

    typedef struct RuntimeRenderServicePort
    {
        void (*submit_scene)(EngineRuntimeState *state);
    } RuntimeRenderServicePort;

    typedef struct RuntimeApplicationServicePorts
    {
        RuntimeTerrainServicePort terrain;
        RuntimePlayerServicePort player;
        RuntimeMerchantServicePort merchant;
        RuntimeRenderServicePort render;
    } RuntimeApplicationServicePorts;

    const RuntimeApplicationServicePorts *runtime_application_service_ports(void);
    void runtime_application_service_ports_reset(void);

#ifdef __cplusplus
}
#endif

#endif
