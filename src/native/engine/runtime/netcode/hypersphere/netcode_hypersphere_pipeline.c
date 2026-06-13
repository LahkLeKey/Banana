#include "netcode_hypersphere_pipeline.h"
#include "netcode_hypersphere_cluster_scoring.h"
#include "netcode_hypersphere_geometry.h"
#include "netcode_hypersphere_observability.h"
#include "netcode_hypersphere_pipeline_internal.h"
#include "netcode_hypersphere_pipeline_setup.h"

int runtime_netcode_hypersphere_pipeline_execute(
    const RuntimeNetcodeVectorOutput *input,
    RuntimeNetcodeHypersphereOutput *out_output)
{
    RuntimeNetcodeHyperspherePipelineContext context;

    if (runtime_netcode_hypersphere_initialize_pipeline_context(
            &context,
            input,
            out_output) != 0)
        return -1;

    runtime_netcode_hypersphere_prepare_geometry(&context);
    runtime_netcode_hypersphere_build_cluster_models(&context);
    runtime_netcode_hypersphere_build_projection_nodes(&context);
    runtime_netcode_hypersphere_build_cluster_radii_and_spectral(&context);
    runtime_netcode_hypersphere_build_weighted_voronoi_scores(&context);
    runtime_netcode_hypersphere_finalize_geometry(&context);
    runtime_netcode_hypersphere_finalize_observability(&context);

    return 0;
}
