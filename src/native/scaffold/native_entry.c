#include "banana_native_v3.h"
#include "runtime/engine/engine_aux_abi.h"
#include "runtime/engine/engine_host.h"
#include "runtime/abi/netcode/netcode_abi.h"
#include "runtime/chunk/chunk_stream_packet.h"
#include "runtime/terrain/terrain_chunks.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

static char s_pgbouncer_connection_uri[512];
static char s_pgbouncer_pool_mode[32] = "transaction";
static int s_pgbouncer_default_pool_size = 20;
static int s_world_initialized = 0;

static int32_t map_runtime_contract_status(int status)
{
	switch (status)
	{
	case RUNTIME_NETCODE_CONTRACT_OK:
		return BANANA_NATIVE_V3_NETCODE_CONTRACT_OK;
	case RUNTIME_NETCODE_CONTRACT_UNSUPPORTED_VERSION:
		return BANANA_NATIVE_V3_NETCODE_CONTRACT_UNSUPPORTED_VERSION;
	case RUNTIME_NETCODE_CONTRACT_INVALID_PAYLOAD:
		return BANANA_NATIVE_V3_NETCODE_CONTRACT_INVALID_PAYLOAD;
	case RUNTIME_NETCODE_CONTRACT_NONFINITE_VALUE:
		return BANANA_NATIVE_V3_NETCODE_CONTRACT_NONFINITE_VALUE;
	case RUNTIME_NETCODE_CONTRACT_CRC_MISMATCH:
		return BANANA_NATIVE_V3_NETCODE_CONTRACT_CRC_MISMATCH;
	default:
		return BANANA_NATIVE_V3_NETCODE_CONTRACT_INVALID_PAYLOAD;
	}
}

static int ensure_world_initialized(void)
{
	if (s_world_initialized)
	{
		return 0;
	}

	if (terrain_chunks_init(1337u, 512) != 0)
	{
		return -1;
	}

	s_world_initialized = 1;
	return 0;
}

int banana_native_v3_abi_version(void) { return 3; }
int banana_native_v3_ping(void) { return 0; }

int banana_native_v3_pgbouncer_available(void)
{
#if defined(BANANA_ENABLE_POSTGRES)
	return 1;
#else
	return 0;
#endif
}

int banana_native_v3_pgbouncer_configure(const char *connection_uri,
										 const char *pool_mode,
										 int default_pool_size)
{
#if !defined(BANANA_ENABLE_POSTGRES)
	(void)connection_uri;
	(void)pool_mode;
	(void)default_pool_size;
	return -2;
#else
	if (!connection_uri || connection_uri[0] == '\0')
		return -1;
	if (default_pool_size <= 0)
		return -1;

	memset(s_pgbouncer_connection_uri, 0, sizeof(s_pgbouncer_connection_uri));
	strncpy(s_pgbouncer_connection_uri, connection_uri, sizeof(s_pgbouncer_connection_uri) - 1);

	if (pool_mode && pool_mode[0] != '\0')
	{
		memset(s_pgbouncer_pool_mode, 0, sizeof(s_pgbouncer_pool_mode));
		strncpy(s_pgbouncer_pool_mode, pool_mode, sizeof(s_pgbouncer_pool_mode) - 1);
	}

	s_pgbouncer_default_pool_size = default_pool_size;
	return 0;
#endif
}

int banana_native_v3_pgbouncer_health_json(char *buffer, int buffer_len)
{
	int written = 0;
	if (!buffer || buffer_len <= 0)
		return -1;

#if !defined(BANANA_ENABLE_POSTGRES)
	written = snprintf(buffer, (size_t)buffer_len,
					   "{\"available\":false,\"enabled\":false,\"reason\":\"postgres_disabled\"}");
#else
	written = snprintf(
		buffer,
		(size_t)buffer_len,
		"{\"available\":true,\"enabled\":%s,\"poolMode\":\"%s\",\"defaultPoolSize\":%d}",
		s_pgbouncer_connection_uri[0] != '\0' ? "true" : "false",
		s_pgbouncer_pool_mode,
		s_pgbouncer_default_pool_size);
#endif

	if (written < 0 || written >= buffer_len)
		return -1;

	return 0;
}

int banana_native_v3_world_init(uint32_t seed, int cache_size)
{
	if (cache_size <= 0)
	{
		return -1;
	}

	if (s_world_initialized)
	{
		terrain_chunks_cleanup();
		s_world_initialized = 0;
	}

	if (terrain_chunks_init(seed, cache_size) != 0)
	{
		return -1;
	}

	s_world_initialized = 1;
	return 0;
}

int banana_native_v3_world_chunk_estimate_size(int object_count)
{
	size_t size = chunk_stream_packet_estimate_size(object_count);
	if (size > (size_t)2147483647)
	{
		return -1;
	}

	return (int)size;
}

int banana_native_v3_world_chunk_serialize(int chunk_x, int chunk_z, uint8_t *buffer, int buffer_len)
{
	if (!buffer || buffer_len <= 0)
	{
		return -1;
	}

	if (ensure_world_initialized() != 0)
	{
		return -2;
	}

	const TerrainChunk *chunk = terrain_chunks_get(chunk_x, chunk_z);
	if (!chunk)
	{
		return -3;
	}

	ChunkStreamPacket packet;
	if (chunk_stream_packet_create(chunk, chunk_x, chunk_z, &packet) != 0)
	{
		return -4;
	}

	return chunk_stream_packet_serialize(&packet, buffer, (size_t)buffer_len);
}

void banana_native_v3_world_cleanup(void)
{
	if (!s_world_initialized)
	{
		return;
	}

	terrain_chunks_cleanup();
	s_world_initialized = 0;
}

void banana_native_v3_netcode_reset(void)
{
	runtime_netcode_abi_reset();
}

void banana_native_v3_netcode_record_node_tap(int32_t node)
{
	runtime_netcode_abi_record_node_tap((RuntimeNetcodeNode)node);
}

void banana_native_v3_netcode_record_action(int32_t action)
{
	runtime_netcode_abi_record_action((RuntimeNetcodeAction)action);
}

int banana_native_v3_netcode_get_ledger(banana_native_v3_netcode_ledger *out_ledger)
{
	RuntimeNetcodeInteractionLedger ledger;

	if (!out_ledger)
	{
		return -1;
	}

	if (runtime_netcode_abi_get_ledger(&ledger) != 0)
	{
		return -1;
	}

	out_ledger->snapshots = ledger.snapshots;
	out_ledger->inspections = ledger.inspections;
	out_ledger->trainings = ledger.trainings;
	out_ledger->routes = ledger.routes;
	out_ledger->node_taps = ledger.node_taps;
	return 0;
}

int banana_native_v3_netcode_build_learning(const banana_native_v3_netcode_signal_input *signal_input,
									banana_native_v3_netcode_learning_output *out_output)
{
	RuntimeNetcodeSignalInput native_input;
	RuntimeNetcodeLearningOutput native_output;

	if (!signal_input || !out_output)
	{
		return -1;
	}

	native_input.call_density = signal_input->call_density;
	native_input.quest_percent = signal_input->quest_percent;
	native_input.combo_streak = signal_input->combo_streak;
	native_input.branch_pressure = signal_input->branch_pressure;
	native_input.workflow_depth = signal_input->workflow_depth;

	if (runtime_netcode_abi_build_learning(native_input, &native_output) != 0)
	{
		return -1;
	}

	out_output->model_confidence = native_output.model_confidence;
	out_output->training_accuracy = native_output.training_accuracy;
	out_output->policy_momentum = native_output.policy_momentum;
	out_output->node_weights[0] = native_output.node_weights[0];
	out_output->node_weights[1] = native_output.node_weights[1];
	out_output->node_weights[2] = native_output.node_weights[2];
	out_output->node_weights[3] = native_output.node_weights[3];
	out_output->recommended_node = (int32_t)native_output.recommended_node;
	out_output->recommended_action = (int32_t)native_output.recommended_action;
	out_output->xp_by_action[0] = native_output.xp_by_action[0];
	out_output->xp_by_action[1] = native_output.xp_by_action[1];
	out_output->xp_by_action[2] = native_output.xp_by_action[2];
	out_output->xp_by_action[3] = native_output.xp_by_action[3];

	return 0;
}

int banana_native_v3_netcode_build_reward(const banana_native_v3_netcode_signal_input *signal_input,
								 int32_t interaction_signal,
								 banana_native_v3_netcode_reward_output *out_output)
{
	RuntimeNetcodeSignalInput native_input;
	RuntimeNetcodeRewardOutput native_output;

	if (!signal_input || !out_output)
	{
		return -1;
	}

	native_input.call_density = signal_input->call_density;
	native_input.quest_percent = signal_input->quest_percent;
	native_input.combo_streak = signal_input->combo_streak;
	native_input.branch_pressure = signal_input->branch_pressure;
	native_input.workflow_depth = signal_input->workflow_depth;

	if (runtime_netcode_abi_build_reward(native_input, interaction_signal, &native_output) != 0)
	{
		return -1;
	}

	out_output->neural_relevance_score = native_output.neural_relevance_score;
	out_output->projected_reward_xp = native_output.projected_reward_xp;
	out_output->reward_tier = (int32_t)native_output.reward_tier;

	return 0;
}

int banana_native_v3_netcode_build_link(const banana_native_v3_netcode_link_input *signal_input,
							   banana_native_v3_netcode_link_output *out_output)
{
	RuntimeNetcodeLinkSignalInput native_input;
	RuntimeNetcodeLinkOutput native_output;

	if (!signal_input || !out_output)
	{
		return -1;
	}

	native_input.call_density = signal_input->call_density;
	native_input.quest_percent = signal_input->quest_percent;
	native_input.player_level = signal_input->player_level;
	native_input.combo_streak = signal_input->combo_streak;
	native_input.branch_pressure = signal_input->branch_pressure;
	native_input.dependency_pulse = signal_input->dependency_pulse;
	native_input.interaction_signal = signal_input->interaction_signal;

	if (runtime_netcode_abi_build_link(native_input, &native_output) != 0)
	{
		return -1;
	}

	out_output->intel = native_output.intel;
	out_output->objectives = native_output.objectives;
	out_output->player = native_output.player;
	out_output->ops = native_output.ops;

	return 0;
}

int banana_native_v3_netcode_build_vector(const banana_native_v3_netcode_vector_input *signal_input,
								 banana_native_v3_netcode_vector_output *out_output)
{
	RuntimeNetcodeVectorSignalInput native_input;
	RuntimeNetcodeVectorOutput native_output;
	int row;
	int col;

	if (!signal_input || !out_output)
	{
		return -1;
	}

	native_input.call_density = signal_input->call_density;
	native_input.quest_percent = signal_input->quest_percent;
	native_input.player_level = signal_input->player_level;
	native_input.combo_streak = signal_input->combo_streak;
	native_input.branch_pressure = signal_input->branch_pressure;
	native_input.dependency_pulse = signal_input->dependency_pulse;
	native_input.workflow_depth = signal_input->workflow_depth;
	native_input.neural_relevance_score = signal_input->neural_relevance_score;
	native_input.network_dimensions = signal_input->network_dimensions;
	native_input.model_confidence = signal_input->model_confidence;
	native_input.policy_momentum = signal_input->policy_momentum;

	if (runtime_netcode_abi_build_vector(native_input, &native_output) != 0)
	{
		return -1;
	}

	out_output->dimensions = native_output.dimensions;
	for (row = 0; row < 4; row++)
	{
		out_output->contract_strength[row] = native_output.contract_strength[row];
		for (col = 0; col < 16; col++)
		{
			out_output->node_vectors[row][col] = native_output.node_vectors[row][col];
		}
	}

	return 0;
}

int banana_native_v3_netcode_build_hypersphere(const banana_native_v3_netcode_vector_input *signal_input,
								   banana_native_v3_netcode_hypersphere_output *out_output)
{
	RuntimeNetcodeVectorSignalInput native_input;
	RuntimeNetcodeHypersphereOutput native_output;
	int index;
	int cluster;
	int dim;
	int score;

	if (!signal_input || !out_output)
	{
		return -1;
	}

	native_input.call_density = signal_input->call_density;
	native_input.quest_percent = signal_input->quest_percent;
	native_input.player_level = signal_input->player_level;
	native_input.combo_streak = signal_input->combo_streak;
	native_input.branch_pressure = signal_input->branch_pressure;
	native_input.dependency_pulse = signal_input->dependency_pulse;
	native_input.workflow_depth = signal_input->workflow_depth;
	native_input.neural_relevance_score = signal_input->neural_relevance_score;
	native_input.network_dimensions = signal_input->network_dimensions;
	native_input.model_confidence = signal_input->model_confidence;
	native_input.policy_momentum = signal_input->policy_momentum;

	if (runtime_netcode_abi_build_hypersphere(native_input, &native_output) != 0)
	{
		return -1;
	}

	out_output->dimensions = native_output.dimensions;
	out_output->alignment = native_output.alignment;
	out_output->radial_stability = native_output.radial_stability;
	out_output->cluster_count = native_output.cluster_count;
	out_output->vector_count = native_output.vector_count;
	for (index = 0; index < 4; index++)
	{
		out_output->nodes[index].x = native_output.nodes[index].x;
		out_output->nodes[index].y = native_output.nodes[index].y;
		out_output->nodes[index].z = native_output.nodes[index].z;
		out_output->nodes[index].coherence = native_output.nodes[index].coherence;
		out_output->nodes[index].inradius = native_output.nodes[index].inradius;
		out_output->nodes[index].nearest_neighbor_distance = native_output.nodes[index].nearest_neighbor_distance;
	}

	for (cluster = 0; cluster < 4; cluster++)
	{
		out_output->centers[cluster].cluster_id = native_output.centers[cluster].cluster_id;
		out_output->centers[cluster].member_count = native_output.centers[cluster].member_count;
		for (dim = 0; dim < 16; dim++)
		{
			out_output->centers[cluster].center_q16[dim] = native_output.centers[cluster].center_q16[dim];
		}

		out_output->radii[cluster].cluster_id = native_output.radii[cluster].cluster_id;
		out_output->radii[cluster].nearest_neighbor_distance_q16 = native_output.radii[cluster].nearest_neighbor_distance_q16;
		out_output->radii[cluster].inscribed_radius_q16 = native_output.radii[cluster].inscribed_radius_q16;
		out_output->radii[cluster].radius_state = native_output.radii[cluster].radius_state;

		out_output->spectral_proxy[cluster].cluster_id = native_output.spectral_proxy[cluster].cluster_id;
		out_output->spectral_proxy[cluster].frequency_proxy_q16 = native_output.spectral_proxy[cluster].frequency_proxy_q16;
		out_output->spectral_proxy[cluster].amplitude_proxy_q16 = native_output.spectral_proxy[cluster].amplitude_proxy_q16;
		out_output->spectral_proxy[cluster].spectral_state = native_output.spectral_proxy[cluster].spectral_state;
	}

	for (score = 0; score < 16; score++)
	{
		out_output->weighted_voronoi_scores[score].vector_id = native_output.weighted_voronoi_scores[score].vector_id;
		out_output->weighted_voronoi_scores[score].cluster_id = native_output.weighted_voronoi_scores[score].cluster_id;
		out_output->weighted_voronoi_scores[score].distance_to_center_q16 = native_output.weighted_voronoi_scores[score].distance_to_center_q16;
		out_output->weighted_voronoi_scores[score].weighted_score_q16 = native_output.weighted_voronoi_scores[score].weighted_score_q16;
		out_output->weighted_voronoi_scores[score].score_validity = native_output.weighted_voronoi_scores[score].score_validity;
	}

	out_output->observability.convergence_status = native_output.observability.convergence_status;
	out_output->observability.iteration_count = native_output.observability.iteration_count;
	out_output->observability.assignment_changes_last_iteration = native_output.observability.assignment_changes_last_iteration;
	out_output->observability.deterministic_hash = native_output.observability.deterministic_hash;
	out_output->observability.endianness_decode_path = native_output.observability.endianness_decode_path;
	out_output->envelope_contract_version = native_output.envelope.contract_version;
	out_output->envelope_byte_order_tag = native_output.envelope.byte_order_tag;
	out_output->envelope_payload_bytes = native_output.envelope.payload_bytes;
	out_output->envelope_payload_crc32 = native_output.envelope.payload_crc32;
	out_output->contract_status = map_runtime_contract_status(native_output.envelope.contract_status);

	if (native_output.envelope.contract_status != RUNTIME_NETCODE_CONTRACT_OK)
	{
		return native_output.envelope.contract_status;
	}

	return 0;
}

int banana_native_v3_launch_gate_policy_resolve(const char *mode_label,
										 banana_launch_gate_policy *out_policy)
{
	const char *resolved_mode_label = runtime_engine_host_launch_gate_mode_label_for(mode_label);
	return runtime_engine_aux_launch_gate_policy_resolve(resolved_mode_label, out_policy);
}

int banana_native_v3_launch_gate_decide(const char *mode_label,
								 int has_steam_identity,
								 int account_linked,
								 int account_in_good_standing,
								 int verification_fresh,
								 int verification_available,
								 banana_native_v3_launch_gate_decision *out_decision)
{
	banana_launch_gate_policy policy;
	banana_launch_gate_reason_code reason_code = BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE;
	const char *resolved_mode_label = runtime_engine_host_launch_gate_mode_label_for(mode_label);
	int allow = 0;

	if (!out_decision)
		return -1;

	if (runtime_engine_aux_launch_gate_policy_resolve(resolved_mode_label, &policy) != 0)
	{
		out_decision->allow = 0;
		out_decision->reason_code = BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE;
		out_decision->is_remediable = 0;
		out_decision->reserved = 0;
		return 0;
	}

	if (runtime_engine_aux_launch_gate_decide(&policy,
									  has_steam_identity,
									  account_linked,
									  account_in_good_standing,
									  verification_fresh,
									  verification_available,
									  &reason_code,
									  &allow) != 0)
	{
		return -1;
	}

	out_decision->allow = (int32_t)allow;
	out_decision->reason_code = reason_code;
	out_decision->is_remediable = (int32_t)runtime_engine_aux_launch_gate_reason_is_remediable(reason_code);
	out_decision->reserved = 0;
	return 0;
}

void banana_native_v3_ui_frame_reset(banana_native_ui_frame *frame)
{
	banana_native_ui_frame_reset(frame);
}

void banana_native_v3_ui_frame_write(
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
	int64_t timestamp_ms)
{
	banana_native_ui_frame_write(frame,
								 host,
								 surface,
								 engine_status,
								 error,
								 movement_source,
								 move_x,
								 move_z,
								 viewport,
								 interaction_message,
								 timestamp_ms);
}
