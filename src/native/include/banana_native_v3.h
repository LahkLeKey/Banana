#ifndef BANANA_NATIVE_V3_H
#define BANANA_NATIVE_V3_H
#include <stdint.h>

#include "banana_launch_gate_policy.h"
#include "banana_native_ui_abi.h"

#if defined(_WIN32)
#define BANANA_NATIVE_V3_EXPORT __declspec(dllexport)
#else
#define BANANA_NATIVE_V3_EXPORT __attribute__((visibility("default")))
#endif
#ifdef __cplusplus
extern "C" {
#endif

typedef struct banana_native_v3_launch_gate_decision {
	int32_t allow;
	banana_launch_gate_reason_code reason_code;
	int32_t is_remediable;
	int32_t reserved;
} banana_native_v3_launch_gate_decision;

typedef struct banana_native_v3_netcode_ledger {
	int32_t snapshots;
	int32_t inspections;
	int32_t trainings;
	int32_t routes;
	int32_t node_taps;
} banana_native_v3_netcode_ledger;

typedef struct banana_native_v3_netcode_signal_input {
	int32_t call_density;
	int32_t quest_percent;
	int32_t combo_streak;
	int32_t branch_pressure;
	int32_t workflow_depth;
} banana_native_v3_netcode_signal_input;

typedef struct banana_native_v3_netcode_learning_output {
	int32_t model_confidence;
	int32_t training_accuracy;
	int32_t policy_momentum;
	int32_t node_weights[4];
	int32_t recommended_node;
	int32_t recommended_action;
	int32_t xp_by_action[4];
} banana_native_v3_netcode_learning_output;

typedef struct banana_native_v3_netcode_reward_output {
	int32_t neural_relevance_score;
	int32_t projected_reward_xp;
	int32_t reward_tier;
} banana_native_v3_netcode_reward_output;

typedef struct banana_native_v3_netcode_link_input {
	int32_t call_density;
	int32_t quest_percent;
	int32_t player_level;
	int32_t combo_streak;
	int32_t branch_pressure;
	int32_t dependency_pulse;
	int32_t interaction_signal;
} banana_native_v3_netcode_link_input;

typedef struct banana_native_v3_netcode_link_output {
	int32_t intel;
	int32_t objectives;
	int32_t player;
	int32_t ops;
} banana_native_v3_netcode_link_output;

typedef struct banana_native_v3_netcode_vector_input {
	int32_t call_density;
	int32_t quest_percent;
	int32_t player_level;
	int32_t combo_streak;
	int32_t branch_pressure;
	int32_t dependency_pulse;
	int32_t workflow_depth;
	int32_t neural_relevance_score;
	int32_t network_dimensions;
	int32_t model_confidence;
	int32_t policy_momentum;
	int32_t assignment_family;
	int32_t spectral_mode;
	int32_t hardware_byte_order_tag;
	int32_t hardware_dtype_tag;
	int32_t hardware_alignment_bytes;
} banana_native_v3_netcode_vector_input;

typedef enum banana_native_v3_netcode_k3h4_assignment_family {
	/* score = distance_q16 / radius_q16 */
	BANANA_NATIVE_V3_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE = 0,
	/* score = distance_q16^2 - radius_q16^2 */
	BANANA_NATIVE_V3_NETCODE_K3H4_ASSIGNMENT_POWER = 1,
} banana_native_v3_netcode_k3h4_assignment_family;

typedef enum banana_native_v3_netcode_k3h4_spectral_mode {
	BANANA_NATIVE_V3_NETCODE_K3H4_SPECTRAL_DISABLED = 0,
	BANANA_NATIVE_V3_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH = 1,
} banana_native_v3_netcode_k3h4_spectral_mode;

#define BANANA_NATIVE_V3_NETCODE_K3H4_DTYPE_TAG_F32_Q16_MIXED 1
#define BANANA_NATIVE_V3_NETCODE_K3H4_ALIGNMENT_BYTES_4 4

/*
 * Public vector contract used to seed the k3h4 pipeline.
 * node_vectors remain float, while later k3h4 payload sections switch to Q16
 * fixed-point for deterministic transport and hashing.
 */
typedef struct banana_native_v3_netcode_vector_output {
	int32_t dimensions;
	float node_vectors[4][16];
	int32_t contract_strength[4];
} banana_native_v3_netcode_vector_output;

typedef struct banana_native_v3_netcode_projection_node {
	float x;
	float y;
	float z;
	int32_t coherence;
	float inradius;
	float nearest_neighbor_distance;
} banana_native_v3_netcode_projection_node;

typedef enum banana_native_v3_netcode_radius_state {
	BANANA_NATIVE_V3_NETCODE_RADIUS_OK = 0,
	BANANA_NATIVE_V3_NETCODE_RADIUS_SINGLE_CLUSTER = 1,
	BANANA_NATIVE_V3_NETCODE_RADIUS_NEAR_ZERO_CLAMPED = 2,
} banana_native_v3_netcode_radius_state;

typedef enum banana_native_v3_netcode_score_validity {
	BANANA_NATIVE_V3_NETCODE_SCORE_VALID = 0,
	BANANA_NATIVE_V3_NETCODE_SCORE_INVALID_RADIUS = 1,
} banana_native_v3_netcode_score_validity;

typedef enum banana_native_v3_netcode_spectral_state {
	BANANA_NATIVE_V3_NETCODE_SPECTRAL_OK = 0,
	BANANA_NATIVE_V3_NETCODE_SPECTRAL_RADIUS_FLOOR_APPLIED = 1,
	BANANA_NATIVE_V3_NETCODE_SPECTRAL_DISABLED = 2,
} banana_native_v3_netcode_spectral_state;

typedef enum banana_native_v3_netcode_endianness_decode_path {
	BANANA_NATIVE_V3_NETCODE_ENDIANNESS_LITTLE_ENDIAN = 0,
	BANANA_NATIVE_V3_NETCODE_ENDIANNESS_BYTE_SWAPPED = 1,
} banana_native_v3_netcode_endianness_decode_path;

/*
 * Center coordinates in signed Q16.
 * Decode with:
 *   center_real = center_q16 / 65536.0
 */
typedef struct banana_native_v3_netcode_k3h4_center {
	int32_t cluster_id;
	int32_t member_count;
	int32_t center_q16[16];
} banana_native_v3_netcode_k3h4_center;

/*
 * Radius contract in Q16.
 * nearest_neighbor_distance_q16 is the closest center-to-center distance.
 * inscribed_radius_q16 is approximately half that distance, with a floor clamp
 * applied when the geometry would collapse.
 */
typedef struct banana_native_v3_netcode_k3h4_radius {
	int32_t cluster_id;
	int32_t nearest_neighbor_distance_q16;
	int32_t inscribed_radius_q16;
	int32_t radius_state;
} banana_native_v3_netcode_k3h4_radius;

/*
 * Per-vector score against one cluster.
 * distance_to_center_q16 stores the Mahalanobis distance in Q16.
 * weighted_score_q16 is derived from the selected assignment family.
 */
typedef struct banana_native_v3_netcode_weighted_voronoi_score {
	int32_t vector_id;
	int32_t cluster_id;
	int32_t distance_to_center_q16;
	int32_t weighted_score_q16;
	int32_t score_validity;
} banana_native_v3_netcode_weighted_voronoi_score;

/*
 * Spectral proxy in Q16.
 * frequency_proxy_q16 approximates 1/radius and amplitude_proxy_q16
 * approximates member_count/vector_count.
 */
typedef struct banana_native_v3_netcode_spectral_proxy {
	int32_t cluster_id;
	int32_t frequency_proxy_q16;
	int32_t amplitude_proxy_q16;
	int32_t spectral_state;
} banana_native_v3_netcode_spectral_proxy;

/*
 * Convergence and reproducibility metadata for the exported payload.
 * deterministic_hash is computed from the Q16-bearing sections of the output.
 */
typedef struct banana_native_v3_netcode_k3h4_observability {
	int32_t convergence_status;
	int32_t iteration_count;
	int32_t assignment_changes_last_iteration;
	int32_t deterministic_hash;
	int32_t endianness_decode_path;
} banana_native_v3_netcode_k3h4_observability;

/*
 * Full exported k3h4 payload.
 * Float fields carry display-friendly geometry; the mathematical clustering
 * contract is primarily represented in the Q16 center/radius/score/proxy
 * fields plus the envelope metadata appended at the tail.
 */
typedef struct banana_native_v3_netcode_k3h4_output {
	int32_t dimensions;
	banana_native_v3_netcode_projection_node nodes[4];
	int32_t alignment;
	int32_t radial_stability;
	int32_t cluster_count;
	int32_t vector_count;
	banana_native_v3_netcode_k3h4_center centers[4];
	banana_native_v3_netcode_k3h4_radius radii[4];
	banana_native_v3_netcode_weighted_voronoi_score weighted_voronoi_scores[16];
	banana_native_v3_netcode_spectral_proxy spectral_proxy[4];
	banana_native_v3_netcode_k3h4_observability observability;
	int32_t envelope_contract_version;
	int32_t envelope_byte_order_tag;
	int32_t envelope_payload_bytes;
	int32_t envelope_payload_crc32;
	int32_t contract_status;
} banana_native_v3_netcode_k3h4_output;

#define BANANA_NATIVE_V3_NETCODE_K3H4_CONTRACT_VERSION 1
#define BANANA_NATIVE_V3_NETCODE_K3H4_BYTE_ORDER_TAG 0x01020304

#define BANANA_NATIVE_V3_K3H4_CONTRACT_VERSION BANANA_NATIVE_V3_NETCODE_K3H4_CONTRACT_VERSION
#define BANANA_NATIVE_V3_K3H4_BYTE_ORDER_TAG BANANA_NATIVE_V3_NETCODE_K3H4_BYTE_ORDER_TAG

typedef enum banana_native_v3_netcode_contract_status {
	BANANA_NATIVE_V3_NETCODE_CONTRACT_OK = 0,
	BANANA_NATIVE_V3_NETCODE_CONTRACT_UNSUPPORTED_VERSION = -2001,
	BANANA_NATIVE_V3_NETCODE_CONTRACT_INVALID_PAYLOAD = -2002,
	BANANA_NATIVE_V3_NETCODE_CONTRACT_NONFINITE_VALUE = -2003,
	BANANA_NATIVE_V3_NETCODE_CONTRACT_CRC_MISMATCH = -2004
} banana_native_v3_netcode_contract_status;

#define BANANA_NATIVE_V3_K3H4_CONTRACT_OK BANANA_NATIVE_V3_NETCODE_CONTRACT_OK
#define BANANA_NATIVE_V3_K3H4_CONTRACT_UNSUPPORTED_VERSION BANANA_NATIVE_V3_NETCODE_CONTRACT_UNSUPPORTED_VERSION
#define BANANA_NATIVE_V3_K3H4_CONTRACT_INVALID_PAYLOAD BANANA_NATIVE_V3_NETCODE_CONTRACT_INVALID_PAYLOAD
#define BANANA_NATIVE_V3_K3H4_CONTRACT_NONFINITE_VALUE BANANA_NATIVE_V3_NETCODE_CONTRACT_NONFINITE_VALUE
#define BANANA_NATIVE_V3_K3H4_CONTRACT_CRC_MISMATCH BANANA_NATIVE_V3_NETCODE_CONTRACT_CRC_MISMATCH

typedef struct banana_native_v3_netcode_contract_envelope_header {
	int32_t contract_version;
	int32_t byte_order_tag;
	int32_t payload_bytes;
	int32_t payload_crc32;
} banana_native_v3_netcode_contract_envelope_header;

typedef enum banana_native_v3_k3h4_dialogue_policy_action {
	BANANA_NATIVE_V3_K3H4_DIALOGUE_POLICY_ACTION_ALLOW = 0,
	BANANA_NATIVE_V3_K3H4_DIALOGUE_POLICY_ACTION_SAFE_REDIRECT = 1,
	BANANA_NATIVE_V3_K3H4_DIALOGUE_POLICY_ACTION_HARD_BLOCK = 2,
} banana_native_v3_k3h4_dialogue_policy_action;

typedef enum banana_native_v3_k3h4_dialogue_turn_status {
	BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK = 0,
	BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_INVALID_ARGUMENT = -4001,
	BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_EXECUTION_FAILED = -4002,
} banana_native_v3_k3h4_dialogue_turn_status;

typedef struct banana_native_v3_k3h4_dialogue_turn_input {
	char npc_id[64];
	char quest_state_id[64];
	char region_id[64];
	char intent_id[64];
	char policy_category[64];
	char policy_confidence_band[16];
	char policy_severity[16];
	int32_t prior_memory_trust_delta_q16;
	int32_t prior_memory_hostility_delta_q16;
	int32_t prior_memory_helpfulness_delta_q16;
	int32_t sequence_tick;
} banana_native_v3_k3h4_dialogue_turn_input;

typedef struct banana_native_v3_k3h4_dialogue_turn_output {
	int32_t schema_version;
	int32_t route_cluster_id;
	int32_t boundary_state;
	int32_t transition_id;
	int32_t response_policy;
	int32_t deny_reason_code;
	int32_t state_mutation_blocked;
	int32_t memory_write_blocked;
	int32_t memory_delta_applied;
	int32_t policy_action_selected;
	int32_t abi_decode_path;
	int32_t abi_contract_version;
	int32_t abi_payload_bytes;
	int32_t abi_crc32;
	int32_t abi_status;
	char response_policy_label[32];
	char npc_line_candidate[160];
	char selected_template_key[64];
} banana_native_v3_k3h4_dialogue_turn_output;

BANANA_NATIVE_V3_EXPORT int banana_native_v3_abi_version(void);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_ping(void);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_pgbouncer_available(void);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_pgbouncer_configure(const char *connection_uri,
																  const char *pool_mode,
																  int default_pool_size);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_pgbouncer_health_json(char *buffer, int buffer_len);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_world_init(uint32_t seed, int cache_size);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_world_chunk_estimate_size(int object_count);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_world_chunk_serialize(int chunk_x,
											int chunk_z,
											uint8_t *buffer,
											int buffer_len);
BANANA_NATIVE_V3_EXPORT void banana_native_v3_world_cleanup(void);
BANANA_NATIVE_V3_EXPORT void banana_native_v3_netcode_reset(void);
BANANA_NATIVE_V3_EXPORT void banana_native_v3_netcode_record_node_tap(int32_t node);
BANANA_NATIVE_V3_EXPORT void banana_native_v3_netcode_record_action(int32_t action);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_netcode_get_ledger(banana_native_v3_netcode_ledger *out_ledger);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_netcode_build_learning(const banana_native_v3_netcode_signal_input *signal_input,
										 banana_native_v3_netcode_learning_output *out_output);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_netcode_build_reward(const banana_native_v3_netcode_signal_input *signal_input,
						 int32_t interaction_signal,
						 banana_native_v3_netcode_reward_output *out_output);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_netcode_build_link(const banana_native_v3_netcode_link_input *signal_input,
						 banana_native_v3_netcode_link_output *out_output);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_netcode_build_vector(const banana_native_v3_netcode_vector_input *signal_input,
						   banana_native_v3_netcode_vector_output *out_output);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_netcode_build_k3h4(const banana_native_v3_netcode_vector_input *signal_input,
							banana_native_v3_netcode_k3h4_output *out_output);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_k3h4_run_dialogue_turn(
	const banana_native_v3_k3h4_dialogue_turn_input *turn_input,
	banana_native_v3_k3h4_dialogue_turn_output *out_output);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_launch_gate_policy_resolve(const char *mode_label,
												 banana_launch_gate_policy *out_policy);
BANANA_NATIVE_V3_EXPORT int banana_native_v3_launch_gate_decide(const char *mode_label,
											 int has_steam_identity,
											 int account_linked,
											 int account_in_good_standing,
											 int verification_fresh,
											 int verification_available,
											 banana_native_v3_launch_gate_decision *out_decision);
BANANA_NATIVE_V3_EXPORT void banana_native_v3_ui_frame_reset(banana_native_ui_frame *frame);
BANANA_NATIVE_V3_EXPORT void banana_native_v3_ui_frame_write(
	banana_native_ui_frame *frame,
	banana_native_ui_host_kind host,
	banana_native_ui_surface_kind surface,
	banana_native_ui_engine_status engine_status,
	const char *error,
	banana_native_ui_movement_source movement_source,
	float move_x,
	float move_z,
	banana_native_ui_viewport viewport,
	const char *interaction_message,
	int64_t timestamp_ms);
#ifdef __cplusplus
}
#endif
#endif
