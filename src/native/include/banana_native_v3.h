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
} banana_native_v3_netcode_vector_input;

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
} banana_native_v3_netcode_spectral_state;

typedef enum banana_native_v3_netcode_endianness_decode_path {
	BANANA_NATIVE_V3_NETCODE_ENDIANNESS_LITTLE_ENDIAN = 0,
	BANANA_NATIVE_V3_NETCODE_ENDIANNESS_BYTE_SWAPPED = 1,
} banana_native_v3_netcode_endianness_decode_path;

typedef struct banana_native_v3_netcode_k3h4_center {
	int32_t cluster_id;
	int32_t member_count;
	int32_t center_q16[16];
} banana_native_v3_netcode_k3h4_center;

typedef struct banana_native_v3_netcode_k3h4_radius {
	int32_t cluster_id;
	int32_t nearest_neighbor_distance_q16;
	int32_t inscribed_radius_q16;
	int32_t radius_state;
} banana_native_v3_netcode_k3h4_radius;

typedef struct banana_native_v3_netcode_weighted_voronoi_score {
	int32_t vector_id;
	int32_t cluster_id;
	int32_t distance_to_center_q16;
	int32_t weighted_score_q16;
	int32_t score_validity;
} banana_native_v3_netcode_weighted_voronoi_score;

typedef struct banana_native_v3_netcode_spectral_proxy {
	int32_t cluster_id;
	int32_t frequency_proxy_q16;
	int32_t amplitude_proxy_q16;
	int32_t spectral_state;
} banana_native_v3_netcode_spectral_proxy;

typedef struct banana_native_v3_netcode_k3h4_observability {
	int32_t convergence_status;
	int32_t iteration_count;
	int32_t assignment_changes_last_iteration;
	int32_t deterministic_hash;
	int32_t endianness_decode_path;
} banana_native_v3_netcode_k3h4_observability;

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
