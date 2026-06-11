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
