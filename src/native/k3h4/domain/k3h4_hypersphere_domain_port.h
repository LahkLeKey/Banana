#ifndef BANANA_NATIVE_K3H4_DOMAIN_PORT_H
#define BANANA_NATIVE_K3H4_DOMAIN_PORT_H

#include "runtime/abi/netcode/netcode_abi.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct BananaNativeK3h4DomainPort
    {
        int (*build_learning)(RuntimeNetcodeSignalInput signal_input,
                              RuntimeNetcodeLearningOutput *out_output);
        int (*build_reward)(RuntimeNetcodeSignalInput signal_input,
                            int interaction_signal,
                            RuntimeNetcodeRewardOutput *out_output);
        int (*build_link)(RuntimeNetcodeLinkSignalInput signal_input,
                          RuntimeNetcodeLinkOutput *out_output);
        int (*build_vector)(RuntimeNetcodeVectorSignalInput signal_input,
                            RuntimeNetcodeVectorOutput *out_output);
        int (*build_hypersphere)(RuntimeNetcodeVectorSignalInput signal_input,
                                 RuntimeNetcodeHypersphereOutput *out_output);
    } BananaNativeK3h4DomainPort;

#ifdef __cplusplus
}
#endif

#endif
