// Native DAL contract tests for deterministic non-success outcome behavior.
#include "../../src/native/wrapper/banana_wrapper.h"

#include <assert.h>
#include <stdio.h>

#if defined(_WIN32)
#include <stdlib.h>
static void set_pg_connection(const char* value) {
    if (value) {
        _putenv_s("BANANA_PG_CONNECTION", value);
    } else {
        _putenv_s("BANANA_PG_CONNECTION", "");
    }
}
#else
#include <stdlib.h>
static void set_pg_connection(const char* value) {
    if (value) {
        setenv("BANANA_PG_CONNECTION", value, 1);
    } else {
        unsetenv("BANANA_PG_CONNECTION");
    }
}
#endif

int main(void) {
    int rc;
    char* json = NULL;

    // Missing configuration is always an explicit non-success category.
    set_pg_connection(NULL);
    rc = banana_db_query_banana_profile("profile-1", &json);
    assert(rc == BANANA_DB_NOT_CONFIGURED);

    // Invalid/unavailable DAL dependency path must never return synthetic success.
    set_pg_connection("host=__banana_missing__ port=1 user=banana password=banana dbname=banana connect_timeout=1");
    rc = banana_db_query_banana_profile("profile-1", &json);
    assert(rc == BANANA_DB_ERROR);

    printf("[native-unit] dal runtime contract behavior ok\n");
    return 0;
}
