#include "banana_native_v3.h"
#include "runtime/engine/engine_aux_abi.h"
#include "runtime/engine/engine_host.h"
#include "runtime/chunk/chunk_stream_packet.h"
#include "runtime/terrain/terrain_chunks.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

static char s_pgbouncer_connection_uri[512];
static char s_pgbouncer_pool_mode[32] = "transaction";
static int s_pgbouncer_default_pool_size = 20;
static int s_world_initialized = 0;

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
