#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_PIPELINE_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_PIPELINE_H

#include "netcode_k3h4_metrics.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * Executes the default geometry/scoring pipeline over vector output.
     *
     * The resulting payload combines:
     *   1. normalized-node geometry and coherence summaries
     *   2. cluster radii derived from center distances
     *   3. weighted Voronoi scores in Q16 contract space
     *
     * Default scoring mode is resolved by setup and is equivalent to either:
     *   multiplicative: score_q16 ~= distance_q16 / radius_q16
     *   power:          score_q16 = distance_q16^2 - radius_q16^2
     */
    int runtime_netcode_k3h4_pipeline_execute(
        const RuntimeNetcodeVectorOutput *input,
        RuntimeNetcodeK3h4Output *out_output);

    /*
     * Executes the same stage graph but with explicit runtime knobs.
     *
     * assignment_family controls the weighted-score equation.
     * spectral_mode toggles whether radius-derived affinity proxies are emitted.
     */
    int runtime_netcode_k3h4_pipeline_execute_with_config(
        const RuntimeNetcodeVectorOutput *input,
        RuntimeNetcodeK3h4Output *out_output,
        int assignment_family,
        int spectral_mode);

#ifdef __cplusplus
}
#endif

#endif
