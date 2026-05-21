#ifndef BANANA_ENGINE_CELLULAR_WORLDGEN_SERVICE_H
#define BANANA_ENGINE_CELLULAR_WORLDGEN_SERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

    void engine_cellular_smooth_heightfield(unsigned char *heightfield, int width, int height,
                                            int max_layers, int iterations);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_CELLULAR_WORLDGEN_SERVICE_H */