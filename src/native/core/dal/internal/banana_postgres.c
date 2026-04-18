#include "banana_postgres.h"

#include "../../testing/native_test_hooks.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BANANA_DOMAIN_AGGREGATE_TABLE "banana_domain_aggregates"

#if defined(BANANA_ENABLE_POSTGRES)
#include <libpq-fe.h>

static char* create_connection_string_with_timeout(const char* connection_string) {
    static const char timeout_key[] = "connect_timeout=";
    static const char timeout_suffix[] = " connect_timeout=5";
    size_t connection_length = 0U;
    size_t suffix_length = sizeof(timeout_suffix) - 1U;
    char* with_timeout = 0;

    if (connection_string == 0 || connection_string[0] == '\0') {
        return 0;
    }

    if (strstr(connection_string, timeout_key) != 0) {
        return 0;
    }

    connection_length = strlen(connection_string);
    with_timeout = (char*)malloc(connection_length + suffix_length + 1U);
    if (with_timeout == 0) {
        return 0;
    }

    if (snprintf(with_timeout, connection_length + suffix_length + 1U, "%s%s", connection_string, timeout_suffix) < 0) {
        free(with_timeout);
        return 0;
    }

    return with_timeout;
}

static void close_connection(PGconn* connection, char* connection_with_timeout) {
    if (connection != 0) {
        PQfinish(connection);
    }

    free(connection_with_timeout);
}

static int open_connection(PGconn** out_connection, char** out_connection_with_timeout) {
    const char* connection_string = getenv("BANANA_PG_CONNECTION");
    const char* resolved_connection_string = connection_string;
    char* connection_with_timeout = 0;
    PGconn* connection = 0;

    if (out_connection == 0 || out_connection_with_timeout == 0) {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    *out_connection = 0;
    *out_connection_with_timeout = 0;

    if (connection_string == 0 || connection_string[0] == '\0') {
        return BANANA_DB_NOT_CONFIGURED;
    }

    connection_with_timeout = create_connection_string_with_timeout(connection_string);
    if (connection_with_timeout != 0) {
        resolved_connection_string = connection_with_timeout;
    }

    connection = PQconnectdb(resolved_connection_string);
    if (connection == 0 || PQstatus(connection) != CONNECTION_OK) {
        close_connection(connection, connection_with_timeout);
        return BANANA_DB_ERROR;
    }

    *out_connection = connection;
    *out_connection_with_timeout = connection_with_timeout;
    return BANANA_DB_OK;
}

static int validate_command_result(PGconn* connection, PGresult** result) {
    if (result == 0 || *result == 0) {
        return BANANA_DB_ERROR;
    }

    if (banana_test_hook_force_db_bad_result()) {
        PQclear(*result);
        *result = 0;
        return BANANA_DB_ERROR;
    }

    if (banana_test_hook_force_db_status_mismatch()) {
        PQclear(*result);
        *result = PQexec(connection, "SELECT 1 WHERE 1 = 0");
        if (*result == 0) {
            return BANANA_DB_ERROR;
        }
    }

    if (PQresultStatus(*result) != PGRES_COMMAND_OK) {
        return BANANA_DB_ERROR;
    }

    return BANANA_DB_OK;
}

static int validate_tuple_result(PGconn* connection, PGresult** result) {
    if (result == 0 || *result == 0) {
        return BANANA_DB_ERROR;
    }

    if (banana_test_hook_force_db_bad_result()) {
        PQclear(*result);
        *result = 0;
        return BANANA_DB_ERROR;
    }

    if (banana_test_hook_force_db_status_mismatch()) {
        PQclear(*result);
        *result = PQexec(connection, "SELECT 1 WHERE 1 = 0");
        if (*result == 0) {
            return BANANA_DB_ERROR;
        }
    }

    if (PQresultStatus(*result) != PGRES_TUPLES_OK) {
        return BANANA_DB_ERROR;
    }

    return BANANA_DB_OK;
}

static int ensure_snapshot_table(PGconn* connection) {
    PGresult* result = PQexec(
        connection,
        "CREATE TABLE IF NOT EXISTS " BANANA_DOMAIN_AGGREGATE_TABLE " ("
        "aggregate_type TEXT NOT NULL, "
        "aggregate_id TEXT NOT NULL, "
        "snapshot BYTEA NOT NULL, "
        "snapshot_size INTEGER NOT NULL, "
        "updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(), "
        "PRIMARY KEY (aggregate_type, aggregate_id))");
    int status = validate_command_result(connection, &result);

    if (result != 0) {
        PQclear(result);
    }

    return status;
}
#endif

int banana_postgres_query_banana_profile(int purchases, int multiplier, int* out_banana) {
    if (out_banana == 0) {
        return BANANA_DB_INVALID_ARGUMENT;
    }

#if defined(BANANA_ENABLE_POSTGRES)
    {
        PGconn* connection = 0;
        char* connection_with_timeout = 0;
        PGresult* result = 0;
        const char* params[2];
        char purchases_buffer[32];
        char multiplier_buffer[32];
        int status = open_connection(&connection, &connection_with_timeout);

        if (status != BANANA_DB_OK) {
            return status;
        }

        snprintf(purchases_buffer, sizeof(purchases_buffer), "%d", purchases);
        snprintf(multiplier_buffer, sizeof(multiplier_buffer), "%d", multiplier);
        params[0] = purchases_buffer;
        params[1] = multiplier_buffer;

        result = PQexecParams(
            connection,
            "WITH banana_input AS (SELECT $1::int AS purchases, $2::int AS multiplier), "
            "banana_yield AS ("
            "SELECT purchases, multiplier, purchases * 10 AS base_banana, "
            "CASE WHEN purchases >= 10 THEN multiplier * 25 ELSE 0 END AS bonus_banana "
            "FROM banana_input) "
            "SELECT purchases, multiplier, (base_banana + bonus_banana) AS banana FROM banana_yield",
            2,
            0,
            params,
            0,
            0,
            0);
        status = validate_tuple_result(connection, &result);
        if (status != BANANA_DB_OK || PQntuples(result) != 1) {
            if (result != 0) {
                PQclear(result);
            }

            close_connection(connection, connection_with_timeout);
            return BANANA_DB_ERROR;
        }

        *out_banana = atoi(PQgetvalue(result, 0, 2));
        PQclear(result);
        close_connection(connection, connection_with_timeout);
        return BANANA_DB_OK;
    }
#else
    (void)purchases;
    (void)multiplier;
    (void)out_banana;
    return BANANA_DB_NOT_CONFIGURED;
#endif
}

int banana_postgres_upsert_snapshot(
    const char* aggregate_type,
    const char* aggregate_id,
    const void* snapshot,
    size_t snapshot_size
) {
    int forced_result = 0;

    if (aggregate_type == 0
        || aggregate_type[0] == '\0'
        || aggregate_id == 0
        || aggregate_id[0] == '\0'
        || snapshot == 0
        || snapshot_size == 0U) {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    if (banana_test_hook_try_forced_db_result(&forced_result)) {
        return forced_result;
    }

#if defined(BANANA_ENABLE_POSTGRES)
    {
        PGconn* connection = 0;
        char* connection_with_timeout = 0;
        int status = open_connection(&connection, &connection_with_timeout);

        if (status != BANANA_DB_OK) {
            return status;
        }

        status = ensure_snapshot_table(connection);
        if (status != BANANA_DB_OK) {
            close_connection(connection, connection_with_timeout);
            return status;
        }

        {
            unsigned char* escaped_snapshot = 0;
            size_t escaped_length = 0U;
            PGresult* result = 0;
            const char* params[4];
            char snapshot_size_buffer[32];

            escaped_snapshot = PQescapeByteaConn(
                connection,
                (const unsigned char*)snapshot,
                snapshot_size,
                &escaped_length);
            (void)escaped_length;
            if (escaped_snapshot == 0) {
                close_connection(connection, connection_with_timeout);
                return BANANA_DB_ERROR;
            }

            snprintf(
                snapshot_size_buffer,
                sizeof(snapshot_size_buffer),
                "%llu",
                (unsigned long long)snapshot_size);
            params[0] = aggregate_type;
            params[1] = aggregate_id;
            params[2] = (const char*)escaped_snapshot;
            params[3] = snapshot_size_buffer;

            result = PQexecParams(
                connection,
                "INSERT INTO " BANANA_DOMAIN_AGGREGATE_TABLE " (aggregate_type, aggregate_id, snapshot, snapshot_size) "
                "VALUES ($1, $2, $3::bytea, $4::int) "
                "ON CONFLICT (aggregate_type, aggregate_id) DO UPDATE SET "
                "snapshot = EXCLUDED.snapshot, "
                "snapshot_size = EXCLUDED.snapshot_size, "
                "updated_at = NOW()",
                4,
                0,
                params,
                0,
                0,
                0);
            status = validate_command_result(connection, &result);

            PQfreemem(escaped_snapshot);
            if (result != 0) {
                PQclear(result);
            }

            close_connection(connection, connection_with_timeout);
            return status;
        }
    }
#else
    (void)aggregate_type;
    (void)aggregate_id;
    (void)snapshot;
    (void)snapshot_size;
    return BANANA_DB_NOT_CONFIGURED;
#endif
}

int banana_postgres_get_snapshot(
    const char* aggregate_type,
    const char* aggregate_id,
    void* out_snapshot,
    size_t snapshot_size
) {
    int forced_result = 0;

    if (aggregate_type == 0
        || aggregate_type[0] == '\0'
        || aggregate_id == 0
        || aggregate_id[0] == '\0'
        || out_snapshot == 0
        || snapshot_size == 0U) {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    if (banana_test_hook_try_forced_db_result(&forced_result)) {
        return forced_result;
    }

#if defined(BANANA_ENABLE_POSTGRES)
    {
        PGconn* connection = 0;
        char* connection_with_timeout = 0;
        int status = open_connection(&connection, &connection_with_timeout);

        if (status != BANANA_DB_OK) {
            return status;
        }

        status = ensure_snapshot_table(connection);
        if (status != BANANA_DB_OK) {
            close_connection(connection, connection_with_timeout);
            return status;
        }

        {
            PGresult* result = 0;
            const char* params[2];
            unsigned char* decoded_snapshot = 0;
            size_t decoded_length = 0U;
            unsigned long long stored_snapshot_size = 0ULL;

            params[0] = aggregate_type;
            params[1] = aggregate_id;
            result = PQexecParams(
                connection,
                "SELECT snapshot, snapshot_size FROM " BANANA_DOMAIN_AGGREGATE_TABLE " "
                "WHERE aggregate_type = $1 AND aggregate_id = $2",
                2,
                0,
                params,
                0,
                0,
                0);
            status = validate_tuple_result(connection, &result);
            if (status != BANANA_DB_OK) {
                if (result != 0) {
                    PQclear(result);
                }

                close_connection(connection, connection_with_timeout);
                return status;
            }

            if (PQntuples(result) == 0) {
                PQclear(result);
                close_connection(connection, connection_with_timeout);
                return BANANA_DB_NOT_FOUND;
            }

            if (PQntuples(result) != 1) {
                PQclear(result);
                close_connection(connection, connection_with_timeout);
                return BANANA_DB_ERROR;
            }

            stored_snapshot_size = strtoull(PQgetvalue(result, 0, 1), 0, 10);
            decoded_snapshot = PQunescapeBytea((const unsigned char*)PQgetvalue(result, 0, 0), &decoded_length);
            if (decoded_snapshot == 0
                || stored_snapshot_size != (unsigned long long)snapshot_size
                || decoded_length != snapshot_size) {
                if (decoded_snapshot != 0) {
                    PQfreemem(decoded_snapshot);
                }

                PQclear(result);
                close_connection(connection, connection_with_timeout);
                return BANANA_DB_ERROR;
            }

            memcpy(out_snapshot, decoded_snapshot, snapshot_size);
            PQfreemem(decoded_snapshot);
            PQclear(result);
            close_connection(connection, connection_with_timeout);
            return BANANA_DB_OK;
        }
    }
#else
    (void)aggregate_type;
    (void)aggregate_id;
    (void)out_snapshot;
    (void)snapshot_size;
    return BANANA_DB_NOT_CONFIGURED;
#endif
}

int banana_postgres_clear_snapshots(const char* aggregate_type) {
    int forced_result = 0;

    if (aggregate_type == 0 || aggregate_type[0] == '\0') {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    if (banana_test_hook_try_forced_db_result(&forced_result)) {
        return forced_result;
    }

#if defined(BANANA_ENABLE_POSTGRES)
    {
        PGconn* connection = 0;
        char* connection_with_timeout = 0;
        int status = open_connection(&connection, &connection_with_timeout);

        if (status != BANANA_DB_OK) {
            return status;
        }

        status = ensure_snapshot_table(connection);
        if (status != BANANA_DB_OK) {
            close_connection(connection, connection_with_timeout);
            return status;
        }

        {
            PGresult* result = 0;
            const char* params[1];

            params[0] = aggregate_type;
            result = PQexecParams(
                connection,
                "DELETE FROM " BANANA_DOMAIN_AGGREGATE_TABLE " WHERE aggregate_type = $1",
                1,
                0,
                params,
                0,
                0,
                0);
            status = validate_command_result(connection, &result);

            if (result != 0) {
                PQclear(result);
            }

            close_connection(connection, connection_with_timeout);
            return status;
        }
    }
#else
    (void)aggregate_type;
    return BANANA_DB_NOT_CONFIGURED;
#endif
}