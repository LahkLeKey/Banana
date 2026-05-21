#ifndef BANANA_ENGINE_WFC_WORLDGEN_SERVICE_H
#define BANANA_ENGINE_WFC_WORLDGEN_SERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

    void engine_wfc_generate_heightfield(unsigned char *out_height, int width, int height,
                                         int max_layers, unsigned int seed);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_WFC_WORLDGEN_SERVICE_H */