#include "banana_core.h"
#include "banana_status.h"
#include "domain/banana_ml_models.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int duplicate_json(const char *payload, char **out_json)
{
    size_t n;
    char *buf;
    if (!payload || !out_json)
        return BANANA_INVALID_ARGUMENT;
    n = strlen(payload) + 1;
    buf = (char *)malloc(n);
    if (!buf)
        return BANANA_INTERNAL_ERROR;
    memcpy(buf, payload, n);
    *out_json = buf;
    return BANANA_OK;
}

int banana_core_predict_ripeness(const char *input_json, char **out_json)
{
    const char *label;
    double score = 0.0;
    char payload[192];
    int written;
    int rc;

    if (!input_json || !out_json)
        return BANANA_INVALID_ARGUMENT;

    rc = banana_ml_predict_regression_score(input_json, &score);
    if (rc != BANANA_OK)
    {
        return rc;
    }

    if (score < 0.35)
    {
        label = "unripe";
    }
    else if (score > 0.72)
    {
        label = "overripe";
    }
    else
    {
        label = "ripe";
    }

    written =
        snprintf(payload, sizeof(payload),
                 "{\"label\":\"%s\",\"confidence\":%.4f,\"model\":\"regression\"}", label, score);
    if (written < 0 || written >= (int)sizeof(payload))
        return BANANA_OVERFLOW;

    return duplicate_json(payload, out_json);
}
