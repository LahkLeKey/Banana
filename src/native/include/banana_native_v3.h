#ifndef BANANA_NATIVE_V3_H
#define BANANA_NATIVE_V3_H
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
#ifdef __cplusplus
}
#endif
#endif
