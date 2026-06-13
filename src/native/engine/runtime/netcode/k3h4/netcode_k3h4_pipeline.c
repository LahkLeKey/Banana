#include "netcode_k3h4_pipeline.h"
#include "netcode_k3h4_cluster_scoring.h"
#include "netcode_k3h4_geometry.h"
#include "netcode_k3h4_observability.h"
#include "netcode_k3h4_pipeline_internal.h"
#include "netcode_k3h4_pipeline_setup.h"

int runtime_netcode_k3h4_pipeline_execute(
    const RuntimeNetcodeVectorOutput *input,
    RuntimeNetcodeK3h4Output *out_output)
{
    RuntimeNetcodeK3h4PipelineContext context;

    if (runtime_netcode_k3h4_initialize_pipeline_context(
            &context,
            input,
            out_output) != 0)
        return -1;

    runtime_netcode_k3h4_prepare_geometry(&context);
    runtime_netcode_k3h4_build_cluster_models(&context);
    runtime_netcode_k3h4_build_projection_nodes(&context);
    runtime_netcode_k3h4_build_cluster_radii_and_spectral(&context);
    runtime_netcode_k3h4_build_weighted_voronoi_scores(&context);
    runtime_netcode_k3h4_finalize_geometry(&context);
    runtime_netcode_k3h4_finalize_observability(&context);

    return 0;
}
