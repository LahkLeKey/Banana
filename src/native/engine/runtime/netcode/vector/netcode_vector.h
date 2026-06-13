#ifndef BANANA_ENGINE_RUNTIME_NETCODE_VECTOR_H
#define BANANA_ENGINE_RUNTIME_NETCODE_VECTOR_H

#define RUNTIME_NETCODE_VECTOR_NODE_COUNT 4
#define RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS 16

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeNetcodeVectorInput
    {
        int call_density;
        int quest_percent;
        int player_level;
        int combo_streak;
        int branch_pressure;
        int dependency_pulse;
        int workflow_depth;
        int neural_relevance_score;
        int network_dimensions;
        int model_confidence;
        int policy_momentum;
    } RuntimeNetcodeVectorInput;

    typedef struct RuntimeNetcodeVectorOutput
    {
        int dimensions;
        float node_vectors[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
        int contract_strength[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        int kmeans_cluster_count;
        int kmeans_iteration_count;
        int kmeans_convergence_status;
        int kmeans_assignment_changes_last_iteration;
        int kmeans_assignments[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        int kmeans_member_counts[RUNTIME_NETCODE_VECTOR_NODE_COUNT];
        int kmeans_centers_q16[RUNTIME_NETCODE_VECTOR_NODE_COUNT][RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS];
    } RuntimeNetcodeVectorOutput;

    int runtime_netcode_vector_build(const RuntimeNetcodeVectorInput *input,
                                     RuntimeNetcodeVectorOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
