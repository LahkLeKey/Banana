#include "banana_native_v3.h"

#include <stdio.h>
#include <string.h>

static char s_pgbouncer_connection_uri[512];
static char s_pgbouncer_pool_mode[32] = "transaction";
static int s_pgbouncer_default_pool_size = 20;

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
