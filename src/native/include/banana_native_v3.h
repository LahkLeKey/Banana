#ifndef BANANA_NATIVE_V3_H
#define BANANA_NATIVE_V3_H
#include <stdint.h>

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
#ifdef __cplusplus
}
#endif
#endif
