#ifndef BANANA_NATIVE_V3_H
#define BANANA_NATIVE_V3_H
#include <stdint.h>

#include "banana_native_ui_abi.h"

#if defined(_WIN32)
#define BANANA_NATIVE_V3_EXPORT __declspec(dllexport)
#else
#define BANANA_NATIVE_V3_EXPORT __attribute__((visibility("default")))
#endif
#ifdef __cplusplus
extern "C" {
#endif
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
