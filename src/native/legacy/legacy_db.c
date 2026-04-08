#include "legacy_db.h"

#include <stdio.h>
#include <stdlib.h>

#if defined(CINTEROP_ENABLE_POSTGRES)
#include <libpq-fe.h>
#endif

static int try_forced_db_result(int* out_forced_result) {
    const char* forced = getenv("CINTEROP_FORCE_DB_RESULT");
    if (forced == 0 || forced[0] == '\0') {
        return 0;
    }

    *out_forced_result = atoi(forced);
    return 1;
}

static int should_force_payload_alloc_failure(void) {
    const char* flag = getenv("CINTEROP_FORCE_PAYLOAD_ALLOC_FAIL");
    return flag != 0 && flag[0] == '1';
}

static int should_force_payload_malloc_null(void) {
    const char* flag = getenv("CINTEROP_FORCE_PAYLOAD_MALLOC_NULL");
    return flag != 0 && flag[0] == '1';
}

static int should_force_db_bad_result(void) {
    const char* flag = getenv("CINTEROP_FORCE_DB_BAD_RESULT");
    return flag != 0 && flag[0] == '1';
}

static int should_force_db_status_mismatch(void) {
    const char* flag = getenv("CINTEROP_FORCE_DB_STATUS_MISMATCH");
    return flag != 0 && flag[0] == '1';
}

static char* build_json_payload(int purchases, int multiplier, int points) {
    int required = snprintf(0, 0,
        "{\"purchases\":%d,\"multiplier\":%d,\"points\":%d}",
        purchases,
        multiplier,
        points);
    char* payload = 0;

    if (required < 0) {
        return 0; /* GCOVR_EXCL_LINE */
    }

    if (should_force_payload_alloc_failure()) {
        return 0;
    }

    if (should_force_payload_malloc_null()) {
        payload = 0;
    } else {
        payload = (char*)malloc((size_t)required + 1U);
    }

    if (payload == 0) {
        return 0;
    }

    if (snprintf(payload, (size_t)required + 1U,
        "{\"purchases\":%d,\"multiplier\":%d,\"points\":%d}",
        purchases,
        multiplier,
        points) < 0) {
        free(payload); /* GCOVR_EXCL_LINE */
        return 0; /* GCOVR_EXCL_LINE */
    }

    return payload;
}

int legacy_db_query_points(int purchases, int multiplier, char** out_payload, int* out_row_count) {
    int forced_result = 0;

    if (out_payload == 0 || out_row_count == 0) {
        return 1;
    }

    if (try_forced_db_result(&forced_result)) {
        return forced_result;
    }

    *out_payload = 0;
    *out_row_count = 0;

#if defined(CINTEROP_ENABLE_POSTGRES)
    {
        const char* connection_string = getenv("CINTEROP_PG_CONNECTION");
        PGconn* connection = 0;
        PGresult* result = 0;
        const char* params[2];
        char purchases_buffer[32];
        char multiplier_buffer[32];
        int points = 0;

        if (connection_string == 0 || connection_string[0] == '\0') {
            return 2;
        }

        snprintf(purchases_buffer, sizeof(purchases_buffer), "%d", purchases);
        snprintf(multiplier_buffer, sizeof(multiplier_buffer), "%d", multiplier);
        params[0] = purchases_buffer;
        params[1] = multiplier_buffer;

        connection = PQconnectdb(connection_string);
        if (PQstatus(connection) != CONNECTION_OK) {
            PQfinish(connection);
            return 3;
        }

        result = PQexecParams(
            connection,
            "SELECT purchases, multiplier, (purchases * 10) + CASE WHEN purchases >= 10 THEN multiplier * 25 ELSE 0 END AS points "
            "FROM (SELECT $1::int AS purchases, $2::int AS multiplier) q",
            2,
            0,
            params,
            0,
            0,
            0);

        if (should_force_db_bad_result()) {
            PQclear(result);
            PQfinish(connection);
            return 3;
        }

        if (should_force_db_status_mismatch()) {
            PQclear(result);
            result = PQexec(connection, "SELECT 1 WHERE 1 = 0");
        }

        if (PQresultStatus(result) != PGRES_TUPLES_OK || PQntuples(result) != 1) {
            PQclear(result);
            PQfinish(connection);
            return 3;
        }

        *out_row_count = PQntuples(result);
        points = atoi(PQgetvalue(result, 0, 2));
        *out_payload = build_json_payload(purchases, multiplier, points);

        PQclear(result);
        PQfinish(connection);

        if (*out_payload == 0) {
            return 3;
        }

        return 0;
    }
#else
    {
        int points = (purchases * 10) + ((purchases >= 10) ? (multiplier * 25) : 0);
        *out_payload = build_json_payload(purchases, multiplier, points);
        *out_row_count = 1;
        if (*out_payload == 0) {
            return 3;
        }

        return 0;
    }
#endif
}
