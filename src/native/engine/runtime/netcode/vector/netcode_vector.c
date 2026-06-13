#include "netcode_vector.h"
#include "netcode_kmeans.h"

#include <math.h>

static float clamp_unit(float value)
{
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static int clamp_percent(int value)
{
    if (value < 0) return 0;
    if (value > 100) return 100;
    return value;
}

static float safe_value(const float *arr, int count, int index)
{
    if (index < 0 || index >= count) return 0.0f;
    return arr[index];
}

static void expand_vector(const float *base,
                          int base_len,
                          int dimensions,
                          const float *harmonic,
                          int harmonic_len,
                          float *out)
{
    int i;
    for (i = 0; i < dimensions; i++)
    {
        if (i < base_len)
        {
            out[i] = clamp_unit(base[i]);
        }
        else
        {
            float a = safe_value(harmonic, harmonic_len, i % harmonic_len);
            float b = safe_value(harmonic, harmonic_len, (i + 3) % harmonic_len);
            float c = safe_value(harmonic, harmonic_len, (i + 5) % harmonic_len);
            out[i] = clamp_unit(a * 0.52f + b * 0.32f + c * 0.16f);
        }
    }
}

static int strength_from_vector(const float *vector, int dimensions)
{
    int i;
    float sum = 0.0f;
    if (dimensions <= 0) return 0;
    for (i = 0; i < dimensions; i++) sum += vector[i];
    return clamp_percent((int)lroundf((sum / (float)dimensions) * 100.0f));
}

int runtime_netcode_vector_build(const RuntimeNetcodeVectorInput *input,
                                 RuntimeNetcodeVectorOutput *out_output)
{
    RuntimeNetcodeKmeansResult kmeans_result;
    float density;
    float quest;
    float player;
    float pressure;
    float pulse;
    float depth;
    float relevance;
    float stability;
    float confidence;
    float momentum;
    float harmonic[10];
    float intel_base[7];
    float objectives_base[7];
    float player_base[7];
    float ops_base[7];
    int dimensions;

    if (!input || !out_output) return -1;

    dimensions = input->network_dimensions;
    if (dimensions < 6) dimensions = 6;
    if (dimensions > RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS)
        dimensions = RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS;

    density = clamp_unit((float)input->call_density / 100.0f);
    quest = clamp_unit((float)input->quest_percent / 100.0f);
    player = clamp_unit(((float)input->player_level * 15.0f +
                         (float)(input->combo_streak < 1 ? 1 : input->combo_streak) * 8.0f) /
                        100.0f);
    pressure = clamp_unit((float)input->branch_pressure / 100.0f);
    pulse = clamp_unit((float)input->dependency_pulse / 100.0f);
    depth = clamp_unit((float)input->workflow_depth / 3.0f);
    relevance = clamp_unit((float)input->neural_relevance_score / 100.0f);
    stability = clamp_unit(1.0f - pressure);
    confidence = clamp_unit((float)input->model_confidence / 100.0f);
    momentum = clamp_unit((float)input->policy_momentum / 100.0f);

    harmonic[0] = density;
    harmonic[1] = quest;
    harmonic[2] = player;
    harmonic[3] = pressure;
    harmonic[4] = pulse;
    harmonic[5] = depth;
    harmonic[6] = relevance;
    harmonic[7] = stability;
    harmonic[8] = confidence;
    harmonic[9] = momentum;

    intel_base[0] = density;
    intel_base[1] = relevance;
    intel_base[2] = stability;
    intel_base[3] = (density + pulse) / 2.0f;
    intel_base[4] = depth * 0.72f;
    intel_base[5] = 1.0f - quest * 0.4f;
    intel_base[6] = confidence;

    objectives_base[0] = quest;
    objectives_base[1] = relevance;
    objectives_base[2] = stability;
    objectives_base[3] = depth;
    objectives_base[4] = (quest + density) / 2.0f;
    objectives_base[5] = 1.0f - pressure * 0.5f;
    objectives_base[6] = momentum;

    player_base[0] = player;
    player_base[1] = relevance;
    player_base[2] = (player + stability) / 2.0f;
    player_base[3] = depth * 0.58f;
    player_base[4] = pulse > 0.2f ? pulse : 0.2f;
    player_base[5] = density > 0.2f ? density : 0.2f;
    player_base[6] = confidence;

    ops_base[0] = pulse;
    ops_base[1] = pressure;
    ops_base[2] = depth;
    ops_base[3] = (density + quest) / 2.0f;
    ops_base[4] = relevance;
    ops_base[5] = (1.0f - stability * 0.7f) > 0.2f ? (1.0f - stability * 0.7f) : 0.2f;
    ops_base[6] = momentum;

    out_output->dimensions = dimensions;
    expand_vector(intel_base, 7, dimensions, harmonic, 10, out_output->node_vectors[0]);
    expand_vector(objectives_base, 7, dimensions, harmonic, 10, out_output->node_vectors[1]);
    expand_vector(player_base, 7, dimensions, harmonic, 10, out_output->node_vectors[2]);
    expand_vector(ops_base, 7, dimensions, harmonic, 10, out_output->node_vectors[3]);

    out_output->contract_strength[0] =
        strength_from_vector(out_output->node_vectors[0], dimensions);
    out_output->contract_strength[1] =
        strength_from_vector(out_output->node_vectors[1], dimensions);
    out_output->contract_strength[2] =
        strength_from_vector(out_output->node_vectors[2], dimensions);
    out_output->contract_strength[3] =
        strength_from_vector(out_output->node_vectors[3], dimensions);

    if (runtime_netcode_kmeans_compute(out_output->node_vectors,
                                       RUNTIME_NETCODE_VECTOR_NODE_COUNT,
                                       dimensions,
                                       RUNTIME_NETCODE_VECTOR_NODE_COUNT,
                                       32,
                                       8,
                                       &kmeans_result) != 0)
    {
        return -1;
    }

    out_output->kmeans_cluster_count = kmeans_result.cluster_count;
    out_output->kmeans_iteration_count = kmeans_result.iteration_count;
    out_output->kmeans_convergence_status = kmeans_result.convergence_status;
    out_output->kmeans_assignment_changes_last_iteration =
        kmeans_result.assignment_changes_last_iteration;

    for (int i = 0; i < RUNTIME_NETCODE_VECTOR_NODE_COUNT; i++)
    {
        out_output->kmeans_assignments[i] = kmeans_result.assignments[i];
        out_output->kmeans_member_counts[i] = kmeans_result.member_counts[i];
        for (int d = 0; d < dimensions; d++)
        {
            out_output->kmeans_centers_q16[i][d] = kmeans_result.centers_q16[i][d];
        }
    }

    return 0;
}
