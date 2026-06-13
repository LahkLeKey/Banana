#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_HYPERSPHERE_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_HYPERSPHERE_H

#include "../hypersphere/netcode_hypersphere.h"
#include "../vector/netcode_vector.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_netcode_k3h4_hypersphere_build(const RuntimeNetcodeVectorOutput *input,
                                               RuntimeNetcodeHypersphereOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
