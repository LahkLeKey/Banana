#include "banana_db.h"

#include "../core/banana_bms.h"

#include "../testing/native_test_hooks.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#endif

static char* build_json_payload(int purchases, int multiplier, int banana) {
    BananaInput input;
    BananaRipenessPrediction prediction;
    BananaStatus status;
    const char* cultivar = banana_species_name(BANANA_SPECIES_CAVENDISH);
    const char* stage_name = "UNKNOWN";
    int required = 0;
    char* payload = 0;

    input.purchases = purchases;
    input.multiplier = multiplier;
    status = banana_predict_ripeness_for_legacy_input(&input, &prediction);
    if (status != BANANA_OK) {
        return 0;
    }

    stage_name = banana_ripeness_stage_name(prediction.predicted_stage);
    required = snprintf(0, 0,
        "{\"purchases\":%d,\"multiplier\":%d,\"banana\":%d,\"cultivar\":\"%s\",\"predicted_stage\":\"%s\",\"shelf_life_hours\":%d,\"spoilage_probability\":%.3f}",
        purchases,
        multiplier,
        banana,
        cultivar,
        stage_name,
        prediction.shelf_life_hours,
        prediction.spoilage_probability);

    if (required < 0) {
        return 0; /* GCOVR_EXCL_LINE */
    }

    if (banana_test_hook_force_payload_alloc_failure()) {
        return 0;
    }

    if (banana_test_hook_force_payload_malloc_null()) {
        payload = 0;
    } else {
        payload = (char*)malloc((size_t)required + 1U);
    }

    if (payload == 0) {
        return 0;
    }

    if (snprintf(payload, (size_t)required + 1U,
        "{\"purchases\":%d,\"multiplier\":%d,\"banana\":%d,\"cultivar\":\"%s\",\"predicted_stage\":\"%s\",\"shelf_life_hours\":%d,\"spoilage_probability\":%.3f}",
        purchases,
        multiplier,
        banana,
        cultivar,
        stage_name,
        prediction.shelf_life_hours,
        prediction.spoilage_probability) < 0) {
        free(payload); /* GCOVR_EXCL_LINE */
        return 0; /* GCOVR_EXCL_LINE */
    }

    return payload;
}

int banana_db_query(int purchases, int multiplier, char** out_payload, int* out_row_count) {
    int forced_result = 0;

    if (out_payload == 0 || out_row_count == 0) {
        return 1;
    }

    if (banana_test_hook_try_forced_db_result(&forced_result)) {
        return forced_result;
    }

    *out_payload = 0;
    *out_row_count = 0;

#if defined(BANANA_ENABLE_POSTGRES)
    {
        const char* connection_string = getenv("BANANA_PG_CONNECTION");
        const char* resolved_connection_string = connection_string;
        char* connection_with_timeout = 0;
        PGconn* connection = 0;
        PGresult* result = 0;
        const char* params[2];
        char purchases_buffer[32];
        char multiplier_buffer[32];
        int banana = 0;

        if (connection_string == 0 || connection_string[0] == '\0') {
            return 2;
        }

        connection_with_timeout = create_connection_string_with_timeout(connection_string);
        if (connection_with_timeout != 0) {
            resolved_connection_string = connection_with_timeout;
        }

        snprintf(purchases_buffer, sizeof(purchases_buffer), "%d", purchases);
        snprintf(multiplier_buffer, sizeof(multiplier_buffer), "%d", multiplier);
        params[0] = purchases_buffer;
        params[1] = multiplier_buffer;

        connection = PQconnectdb(resolved_connection_string);
        if (PQstatus(connection) != CONNECTION_OK) {
            PQfinish(connection);
            free(connection_with_timeout);
            return 3;
        }

        result = PQexecParams(
            connection,
            "SELECT purchases, multiplier, (purchases * 10) + CASE WHEN purchases >= 10 THEN multiplier * 25 ELSE 0 END AS banana "
            "FROM (SELECT $1::int AS purchases, $2::int AS multiplier) q",
            2,
            0,
            params,
            0,
            0,
            0);

        if (banana_test_hook_force_db_bad_result()) {
            PQclear(result);
            PQfinish(connection);
            free(connection_with_timeout);
            return 3;
        }

        if (banana_test_hook_force_db_status_mismatch()) {
            PQclear(result);
            result = PQexec(connection, "SELECT 1 WHERE 1 = 0");
        }

        if (PQresultStatus(result) != PGRES_TUPLES_OK || PQntuples(result) != 1) {
            PQclear(result);
            PQfinish(connection);
            free(connection_with_timeout);
            return 3;
        }

        *out_row_count = PQntuples(result);
        banana = atoi(PQgetvalue(result, 0, 2));
        *out_payload = build_json_payload(purchases, multiplier, banana);

        PQclear(result);
        PQfinish(connection);
        free(connection_with_timeout);

        if (*out_payload == 0) {
            return 3;
        }

        return 0;
    }
#else
    {
        BananaInput input;
        BananaResult result;

        input.purchases = purchases;
        input.multiplier = multiplier;
        if (banana_calculate(&input, &result) != BANANA_OK) {
            return 3;
        }

        *out_payload = build_json_payload(purchases, multiplier, result.banana);
        *out_row_count = 1;
        if (*out_payload == 0) {
            return 3;
        }

        return 0;
    }
#endif
}
