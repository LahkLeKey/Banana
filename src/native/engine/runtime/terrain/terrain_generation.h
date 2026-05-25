#ifndef BANANA_ENGINE_RUNTIME_TERRAIN_GENERATION_H
#define BANANA_ENGINE_RUNTIME_TERRAIN_GENERATION_H

#ifdef __cplusplus
extern "C"
{
#endif

#define RUNTIME_TERRAIN_GENERATION_CONTRACT_V1 "v1"

    typedef enum RuntimeTerrainGenerationStatus
    {
        RUNTIME_TERRAIN_GENERATION_STATUS_OK = 0,
        RUNTIME_TERRAIN_GENERATION_STATUS_TIMEOUT = -11,
        RUNTIME_TERRAIN_GENERATION_STATUS_DEPENDENCY_UNAVAILABLE = -12,
        RUNTIME_TERRAIN_GENERATION_STATUS_INVALID_AREA_IDENTITY = -21,
        RUNTIME_TERRAIN_GENERATION_STATUS_CONTRACT_VERSION_UNSUPPORTED = -22,
    } RuntimeTerrainGenerationStatus;

    unsigned int runtime_terrain_derive_world_seed(const char *world_id,
                                                   const char *lane_id,
                                                   unsigned int partition_epoch);

    int runtime_terrain_generate_island(unsigned char *height_grid,
                                        int terrain_size,
                                        int terrain_max_layers);

    unsigned int runtime_terrain_generation_input_fingerprint(
        const char *world_id,
        const char *lane_id,
        unsigned int partition_epoch,
        int chunk_x,
        int chunk_z,
        const char *generation_contract_version,
        unsigned int retry_attempt);

    int runtime_terrain_generate_unexplored_contract(
        unsigned char *height_grid,
        int terrain_size,
        int terrain_max_layers,
        const char *world_id,
        const char *lane_id,
        unsigned int partition_epoch,
        int chunk_x,
        int chunk_z,
        const char *generation_contract_version,
        unsigned int retry_attempt,
        int inject_failure_code,
        unsigned int *out_generation_input_fingerprint);

#ifdef __cplusplus
}
#endif

#endif