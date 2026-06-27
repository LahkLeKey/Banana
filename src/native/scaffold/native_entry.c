#include "banana_native_v3.h"
#include "k3h4/k3h4_dialogue_abi_envelope.h"
#include "k3h4/k3h4_dialogue_cluster_router.h"
#include "k3h4/k3h4_dialogue_generative_surface_layer.h"
#include "k3h4/k3h4_dialogue_memory_delta_store.h"
#include "k3h4/k3h4_dialogue_spectral_transition_graph.h"
#include "k3h4/k3h4_dialogue_template_layer.h"
#include "k3h4/k3h4_metrics_orchestration_layer.h"
#include "k3h4/k3h4_native_orchestrator.h"
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
static BananaK3h4DialogueMemoryStore s_dialogue_memory_store;
static int s_dialogue_memory_initialized = 0;

static void copy_text(char *target, size_t target_size, const char *source)
{
	if (!target || target_size == 0)
		return;

	if (!source)
	{
		target[0] = '\0';
		return;
	}

	strncpy(target, source, target_size - 1);
	target[target_size - 1] = '\0';
}

static uint32_t hash_text_seed(uint32_t seed, const char *text)
{
	uint32_t hash = seed;
	const unsigned char *cursor = (const unsigned char *)text;

	if (!cursor)
		return hash;

	while (*cursor)
	{
		hash ^= (uint32_t)(*cursor);
		hash *= 16777619u;
		cursor += 1;
	}

	return hash;
}

static uint32_t hash_text(const char *text)
{
	return hash_text_seed(2166136261u, text);
}

static int text_contains(const char *text, const char *token)
{
	if (!text || !token || token[0] == '\0')
		return 0;
	return strstr(text, token) != NULL;
}

static BananaK3h4DialogueIntent parse_dialogue_intent(const char *intent_id)
{
	if (!intent_id)
		return BANANA_K3H4_DIALOGUE_INTENT_UNKNOWN;

	if (strcmp(intent_id, "ASK_CASTLE_ENTRY") == 0)
		return BANANA_K3H4_DIALOGUE_INTENT_ASK_CASTLE_ENTRY;
	if (strcmp(intent_id, "ASK_DIRECTIONS") == 0)
		return BANANA_K3H4_DIALOGUE_INTENT_ASK_DIRECTIONS;
	if (strcmp(intent_id, "INSULT") == 0)
		return BANANA_K3H4_DIALOGUE_INTENT_INSULT;
	if (strcmp(intent_id, "REQUEST_HELP") == 0)
		return BANANA_K3H4_DIALOGUE_INTENT_REQUEST_HELP;

	return BANANA_K3H4_DIALOGUE_INTENT_UNKNOWN;
}

static int policy_category_is_hard_block(const char *policy_category)
{
	if (!policy_category)
		return 0;

	return strcmp(policy_category, "minor_safety") == 0 ||
		   strcmp(policy_category, "self_harm") == 0 ||
		   strcmp(policy_category, "real_world_violence") == 0 ||
		   strcmp(policy_category, "hate_or_slur_severe") == 0;
}

static banana_native_v3_k3h4_dialogue_policy_action decide_policy_action(
	const banana_native_v3_k3h4_dialogue_turn_input *turn_input)
{
	int is_high_confidence;
	int is_boundary_confidence;

	if (!turn_input)
		return BANANA_NATIVE_V3_K3H4_DIALOGUE_POLICY_ACTION_ALLOW;

	is_high_confidence = strcmp(turn_input->policy_confidence_band, "high") == 0;
	is_boundary_confidence = strcmp(turn_input->policy_confidence_band, "boundary") == 0 ||
					 strcmp(turn_input->policy_confidence_band, "medium") == 0;

	if (policy_category_is_hard_block(turn_input->policy_category) && is_high_confidence)
		return BANANA_NATIVE_V3_K3H4_DIALOGUE_POLICY_ACTION_HARD_BLOCK;

	if (policy_category_is_hard_block(turn_input->policy_category) && is_boundary_confidence)
		return BANANA_NATIVE_V3_K3H4_DIALOGUE_POLICY_ACTION_SAFE_REDIRECT;

	if (strcmp(turn_input->policy_category, "jailbreak_or_prompt_attack") == 0)
		return BANANA_NATIVE_V3_K3H4_DIALOGUE_POLICY_ACTION_SAFE_REDIRECT;

	return BANANA_NATIVE_V3_K3H4_DIALOGUE_POLICY_ACTION_ALLOW;
}

static uint32_t proposed_fact_mask_for_intent(BananaK3h4DialogueIntent intent)
{
	switch (intent)
	{
	case BANANA_K3H4_DIALOGUE_INTENT_ASK_CASTLE_ENTRY:
		return BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_ENTRY_WRIT_STATUS;
	case BANANA_K3H4_DIALOGUE_INTENT_ASK_DIRECTIONS:
		return BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_PUBLIC_GATE_DIRECTIONS;
	case BANANA_K3H4_DIALOGUE_INTENT_INSULT:
		return BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_ESCALATION_WARNING;
	case BANANA_K3H4_DIALOGUE_INTENT_REQUEST_HELP:
		return BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_HELP_GUIDANCE;
	case BANANA_K3H4_DIALOGUE_INTENT_UNKNOWN:
	default:
		return BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_PUBLIC_GATE_DIRECTIONS;
	}
}

static BananaK3h4DialogueSpectralState requested_state_for_route(
	const BananaK3h4DialogueRouteOutput *route_output)
{
	if (!route_output)
		return BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ENTRY_GATE;

	switch (route_output->response_contract.response_mode)
	{
	case BANANA_K3H4_DIALOGUE_RESPONSE_DENY:
		return BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_DENY;
	case BANANA_K3H4_DIALOGUE_RESPONSE_REDIRECT:
		return BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_REDIRECT;
	case BANANA_K3H4_DIALOGUE_RESPONSE_DEESCALATE:
		return BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_DEESCALATE;
	case BANANA_K3H4_DIALOGUE_RESPONSE_ASSIST:
		return BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ASSIST;
	case BANANA_K3H4_DIALOGUE_RESPONSE_NEUTRAL:
	default:
		return BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ENTRY_GATE;
	}
}

static void ensure_dialogue_memory_store(void)
{
	BananaK3h4DialogueMemoryPolicy policy;

	if (s_dialogue_memory_initialized)
		return;

	policy.ttl_ticks = 16;
	policy.delta_min = -8192;
	policy.delta_max = 8192;
	banana_native_k3h4_dialogue_memory_init(&s_dialogue_memory_store, policy);
	s_dialogue_memory_initialized = 1;
}

static void select_fallback_line(const banana_native_v3_k3h4_dialogue_turn_input *turn_input,
				 banana_native_v3_k3h4_dialogue_turn_output *out_output)
{
	uint32_t hash;
	int slot;

	hash = hash_text_seed(hash_text(turn_input->policy_category), turn_input->npc_id);
	slot = (int)(hash % 3u);

	if (slot == 0)
	{
		copy_text(out_output->npc_line_candidate,
			  sizeof(out_output->npc_line_candidate),
			  "I cannot help with that. We can discuss safe alternatives inside the gate protocol.");
	}
	else if (slot == 1)
	{
		copy_text(out_output->npc_line_candidate,
			  sizeof(out_output->npc_line_candidate),
			  "That request is blocked. I can offer lawful guidance for the South Gate instead.");
	}
	else
	{
		copy_text(out_output->npc_line_candidate,
			  sizeof(out_output->npc_line_candidate),
			  "I will not continue on that path. Ask for directions or entry requirements instead.");
	}
}

static void select_allow_line(const BananaK3h4DialogueGeneratorOutput *generator_output,
			      banana_native_v3_k3h4_dialogue_turn_output *out_output)
{
	if (text_contains(generator_output->selected_template_key, "assist"))
	{
		copy_text(out_output->npc_line_candidate,
			  sizeof(out_output->npc_line_candidate),
			  "Bring your writ to the registrar and I can guide you through entry.");
		return;
	}

	if (text_contains(generator_output->selected_template_key, "deescalate"))
	{
		copy_text(out_output->npc_line_candidate,
			  sizeof(out_output->npc_line_candidate),
			  "Stand down and speak plainly. We can settle this at the checkpoint.");
		return;
	}

	if (text_contains(generator_output->selected_template_key, "redirect"))
	{
		copy_text(out_output->npc_line_candidate,
			  sizeof(out_output->npc_line_candidate),
			  "Use the public gate road and report to the watch post for next steps.");
		return;
	}

	copy_text(out_output->npc_line_candidate,
		  sizeof(out_output->npc_line_candidate),
		  "State your business at South Gate and I will review your entry status.");
}

static int32_t map_runtime_contract_status(int status)
{
	switch (status)
	{
	case RUNTIME_K3H4_CONTRACT_OK:
		return BANANA_NATIVE_V3_NETCODE_CONTRACT_OK;
	case RUNTIME_K3H4_CONTRACT_UNSUPPORTED_VERSION:
		return BANANA_NATIVE_V3_NETCODE_CONTRACT_UNSUPPORTED_VERSION;
	case RUNTIME_K3H4_CONTRACT_INVALID_PAYLOAD:
		return BANANA_NATIVE_V3_NETCODE_CONTRACT_INVALID_PAYLOAD;
	case RUNTIME_K3H4_CONTRACT_NONFINITE_VALUE:
		return BANANA_NATIVE_V3_NETCODE_CONTRACT_NONFINITE_VALUE;
	case RUNTIME_K3H4_CONTRACT_CRC_MISMATCH:
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
	runtime_k3h4_abi_reset();
}

void banana_native_v3_netcode_record_node_tap(int32_t node)
{
	runtime_k3h4_abi_record_node_tap((RuntimeNetcodeNode)node);
}

void banana_native_v3_netcode_record_action(int32_t action)
{
	runtime_k3h4_abi_record_action((RuntimeNetcodeAction)action);
}

int banana_native_v3_netcode_get_ledger(banana_native_v3_netcode_ledger *out_ledger)
{
	RuntimeNetcodeInteractionLedger ledger;

	if (!out_ledger)
	{
		return -1;
	}

	if (runtime_k3h4_abi_get_ledger(&ledger) != 0)
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
	RuntimeK3h4SignalInput native_input;
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

	if (banana_native_k3h4_layer_build_learning(native_input, &native_output) != 0)
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
	RuntimeK3h4SignalInput native_input;
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

	if (banana_native_k3h4_layer_build_reward(native_input, interaction_signal, &native_output) != 0)
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
	RuntimeK3h4LinkSignalInput native_input;
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

	if (banana_native_k3h4_layer_build_link(native_input, &native_output) != 0)
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
	RuntimeK3h4VectorSignalInput native_input;
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
	native_input.assignment_family = signal_input->assignment_family;
	native_input.spectral_mode = signal_input->spectral_mode;
	native_input.hardware_byte_order_tag = signal_input->hardware_byte_order_tag;
	native_input.hardware_dtype_tag = signal_input->hardware_dtype_tag;
	native_input.hardware_alignment_bytes = signal_input->hardware_alignment_bytes;

	if (banana_native_k3h4_layer_build_vector(native_input, &native_output) != 0)
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

int banana_native_v3_netcode_build_k3h4(const banana_native_v3_netcode_vector_input *signal_input,
								   banana_native_v3_netcode_k3h4_output *out_output)
{
	RuntimeK3h4VectorSignalInput native_input;
	RuntimeNetcodeK3h4Output native_output;
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
	native_input.assignment_family = signal_input->assignment_family;
	native_input.spectral_mode = signal_input->spectral_mode;
	native_input.hardware_byte_order_tag = signal_input->hardware_byte_order_tag;
	native_input.hardware_dtype_tag = signal_input->hardware_dtype_tag;
	native_input.hardware_alignment_bytes = signal_input->hardware_alignment_bytes;

	if (banana_native_k3h4_layer_build_k3h4(native_input, &native_output) != 0)
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

	if (native_output.envelope.contract_status != RUNTIME_K3H4_CONTRACT_OK)
	{
		return native_output.envelope.contract_status;
	}

	return 0;
}

int banana_native_v3_k3h4_run_dialogue_turn(
	const banana_native_v3_k3h4_dialogue_turn_input *turn_input,
	banana_native_v3_k3h4_dialogue_turn_output *out_output)
{
	BananaK3h4DialogueRouteInput route_input;
	BananaK3h4DialogueRouteOutput route_output;
	BananaK3h4DialogueTemplateInput template_input;
	BananaK3h4DialogueTemplateOutput template_output;
	BananaK3h4DialogueGeneratorInput generator_input;
	BananaK3h4DialogueGeneratorOutput generator_output;
	BananaK3h4DialogueSpectralTransitionInput spectral_input;
	BananaK3h4DialogueSpectralTransitionOutput spectral_output;
	banana_native_v3_k3h4_dialogue_policy_action policy_action;
	BananaK3h4DialogueAbiEnvelope abi_envelope;
	uint8_t abi_buffer[256];
	size_t abi_written = 0;
	uint32_t npc_hash;
	int status;

	if (!turn_input || !out_output)
		return BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_INVALID_ARGUMENT;

	memset(out_output, 0, sizeof(*out_output));
	out_output->schema_version = 1;
	out_output->abi_status = BANANA_K3H4_DIALOGUE_ABI_INVALID_ARGUMENT;

	route_input.intent = parse_dialogue_intent(turn_input->intent_id);
	route_input.has_entry_writ = text_contains(turn_input->quest_state_id, "granted") ? 1 : 0;
	route_input.mentions_secret_tunnel = text_contains(turn_input->intent_id, "SECRET_TUNNEL") ? 1 : 0;
	route_input.ambiguity_basis_points =
		strcmp(turn_input->policy_confidence_band, "low") == 0 ? 900 :
		strcmp(turn_input->policy_confidence_band, "boundary") == 0 ? 1300 : 200;

	status = banana_native_k3h4_route_dialogue_cluster(&route_input, &route_output);
	if (status != 0)
		return BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_EXECUTION_FAILED;

	template_input.route_output = route_output;
	template_input.proposed_fact_mask = proposed_fact_mask_for_intent(route_input.intent);
	status = banana_native_k3h4_resolve_dialogue_template(&template_input, &template_output);
	if (status != 0)
		return BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_EXECUTION_FAILED;

	generator_input.template_output = template_output;
	generator_input.selected_fact_mask = template_input.proposed_fact_mask;
	generator_input.canon_violation_mask = 0u;
	status = banana_native_k3h4_assemble_dialogue_generator_contract(&generator_input, &generator_output);
	if (status != 0 && status != -2)
		return BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_EXECUTION_FAILED;

	spectral_input.current_state = BANANA_K3H4_DIALOGUE_SPECTRAL_STATE_ENTRY_GATE;
	spectral_input.requested_state = requested_state_for_route(&route_output);
	spectral_input.route_output = route_output;
	banana_native_k3h4_resolve_spectral_transition(&spectral_input, &spectral_output);

	status = banana_native_k3h4_dialogue_abi_encode(&spectral_output, abi_buffer, sizeof(abi_buffer), &abi_written);
	if (status != BANANA_K3H4_DIALOGUE_ABI_OK)
		return BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_EXECUTION_FAILED;

	status = banana_native_k3h4_dialogue_abi_decode(abi_buffer, abi_written, &abi_envelope);
	if (status != BANANA_K3H4_DIALOGUE_ABI_OK)
		return BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_EXECUTION_FAILED;

	policy_action = decide_policy_action(turn_input);

	out_output->route_cluster_id = route_output.cluster_id;
	out_output->boundary_state = route_output.boundary_state;
	out_output->transition_id = route_output.transition_id;
	out_output->response_policy = route_output.response_contract.response_mode;
	out_output->deny_reason_code = generator_output.deny_code != 0 ?
		generator_output.deny_code : spectral_output.deny_reason_code;
	out_output->policy_action_selected = policy_action;
	out_output->abi_decode_path = abi_envelope.telemetry.endianness_decode_path;
	out_output->abi_contract_version = (int32_t)abi_envelope.contract_version;
	out_output->abi_payload_bytes = abi_envelope.telemetry.payload_bytes;
	out_output->abi_crc32 = (int32_t)abi_envelope.crc32;
	out_output->abi_status = BANANA_K3H4_DIALOGUE_ABI_OK;
	copy_text(out_output->selected_template_key,
		  sizeof(out_output->selected_template_key),
		  generator_output.selected_template_key);

	if (policy_action == BANANA_NATIVE_V3_K3H4_DIALOGUE_POLICY_ACTION_HARD_BLOCK)
	{
		out_output->state_mutation_blocked = 1;
		out_output->memory_write_blocked = 1;
		out_output->memory_delta_applied = 0;
		out_output->deny_reason_code = (int32_t)(9100 + (hash_text(turn_input->policy_category) % 100u));
		copy_text(out_output->response_policy_label,
			  sizeof(out_output->response_policy_label),
			  "hard_block");
		select_fallback_line(turn_input, out_output);
		return BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK;
	}

	if (policy_action == BANANA_NATIVE_V3_K3H4_DIALOGUE_POLICY_ACTION_SAFE_REDIRECT)
	{
		out_output->state_mutation_blocked = 1;
		out_output->memory_write_blocked = 1;
		out_output->memory_delta_applied = 0;
		copy_text(out_output->response_policy_label,
			  sizeof(out_output->response_policy_label),
			  "safe_redirect");
		select_fallback_line(turn_input, out_output);
		return BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK;
	}

	out_output->state_mutation_blocked = 0;
	out_output->memory_write_blocked = 0;
	copy_text(out_output->response_policy_label,
		  sizeof(out_output->response_policy_label),
		  generator_output.fail_closed ? "fail_closed" : "allow");
	select_allow_line(&generator_output, out_output);

	ensure_dialogue_memory_store();
	npc_hash = hash_text(turn_input->npc_id);
	status = banana_native_k3h4_dialogue_memory_upsert_delta(
		&s_dialogue_memory_store,
		npc_hash == 0 ? 1u : npc_hash,
		BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_TRUST_DELTA,
		turn_input->prior_memory_trust_delta_q16,
		(uint64_t)(turn_input->sequence_tick < 0 ? 0 : turn_input->sequence_tick));
	out_output->memory_delta_applied = (status == 0) ? 1 : 0;

	return BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK;
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
