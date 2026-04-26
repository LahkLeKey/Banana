#include "banana_dal.h"
#include "banana_status.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(BANANA_DAL_LIBPQ)
#  include <libpq-fe.h>
#endif

int banana_dal_query_profile(const char* profile_id, char** out_json) {
    if (!profile_id || !out_json) return BANANA_INVALID_ARGUMENT;

    const char* conn = getenv("BANANA_PG_CONNECTION");
    if (!conn || conn[0] == '\0') {
        /* Loud gate (spec 006 FR-006). Never silent skip. */
        return BANANA_DB_NOT_CONFIGURED;
    }

#if defined(BANANA_DAL_LIBPQ)
    PGconn* c = PQconnectdb(conn);
    if (PQstatus(c) != CONNECTION_OK) { PQfinish(c); return BANANA_DB_ERROR; }
    const char* params[1] = { profile_id };
    PGresult* r = PQexecParams(c,
        "select coalesce(json_agg(t), '[]'::json)::text from banana_profile t where t.id = $1",
        1, NULL, params, NULL, NULL, 0);
    if (PQresultStatus(r) != PGRES_TUPLES_OK) { PQclear(r); PQfinish(c); return BANANA_DB_ERROR; }
    if (PQntuples(r) == 0) { PQclear(r); PQfinish(c); return BANANA_NOT_FOUND; }
    const char* val = PQgetvalue(r, 0, 0);
    size_t n = strlen(val) + 1;
    char* buf = (char*)malloc(n);
    if (!buf) { PQclear(r); PQfinish(c); return BANANA_INTERNAL_ERROR; }
    memcpy(buf, val, n);
    *out_json = buf;
    PQclear(r); PQfinish(c);
    return BANANA_OK;
#else
    /* Dependency unavailable is an explicit non-success outcome.
     * Do not emit synthetic success payloads when DAL cannot execute. */
    (void)profile_id;
    return BANANA_DB_ERROR;
#endif
}
