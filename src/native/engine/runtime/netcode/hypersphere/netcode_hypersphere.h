#ifndef BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_H
#define BANANA_ENGINE_RUNTIME_NETCODE_HYPERSPHERE_H

#include "../vector/netcode_vector.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeNetcodeProjectionNode
    {
        float x;
        float y;
        float z;
        int coherence;
        float inradius;
        float nearest_neighbor_distance;
    } RuntimeNetcodeProjectionNode;

    typedef struct RuntimeNetcodeHypersphereOutput
    {
        int dimensions;
        RuntimeNetcodeProjectionNode nodes[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        int alignment;
        int radial_stability;
    } RuntimeNetcodeHypersphereOutput;

    int runtime_netcode_hypersphere_build(const RuntimeNetcodeVectorOutput *input,
                                          RuntimeNetcodeHypersphereOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
