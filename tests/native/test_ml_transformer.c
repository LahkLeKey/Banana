/*
 * Native test suite for slice 012 (Full Brain transformer runnable C code).
 *
 * Exercises the public ABI:
 *   - banana_classify_banana_transformer
 *   - banana_classify_banana_transformer_ex
 *
 * Coverage map:
 *   US1 -- input contract enforcement (FB-R05).
 *   US2 -- 5-call byte-identical idempotence (FB-R02).
 *   US3 -- embedding fingerprint validity helper (FB-R04).
 *   US4 -- opt-in attention-weights logging (FB-R03).
 */

#include "../../src/native/wrapper/banana_wrapper.h"
#include "../../src/native/core/domain/ml/transformer/banana_ml_transformer.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_failures = 0;

#define EXPECT(cond, msg)                                                                  \
    do {                                                                                    \
        if (!(cond)) {                                                                      \
            ++g_failures;                                                                   \
            fprintf(stderr, "[FAIL] %s:%d: %s -- (%s)\n", __FILE__, __LINE__, msg, #cond); \
        }                                                                                   \
    } while (0)

static int contains(const char* haystack, const char* needle) {
    if (!haystack || !needle) {
        return 0;
    }
    return strstr(haystack, needle) != NULL;
}

/*
 * Validates that an embedding vector satisfies the documented FB-R04 shape:
 *   - non-NULL
 *   - dim > 0 and equal to the expected `dim`
 *   - all components finite (no NaN/Inf)
 *   - L2 norm <= max_norm
 * Returns 1 on success, 0 on failure (so it can be used inside both
 * positive and negative cases without triggering EXPECT failures itself).
 */
static int assert_embedding_valid(const double* vec, size_t dim, double max_norm) {
    double sum_sq = 0.0;
    size_t i;

    if (!vec || dim == 0) {
        return 0;
    }
    for (i = 0; i < dim; ++i) {
        if (!isfinite(vec[i])) {
            return 0;
        }
        sum_sq += vec[i] * vec[i];
    }
    if (!(sqrt(sum_sq) <= max_norm)) {
        return 0;
    }
    return 1;
}

/* -------------------------- US1: input contract -------------------------- */

static void test_us1_input_contract(void) {
    char* json = NULL;
    int rc;

    /* Null input JSON -> rejected. */
    json = NULL;
    rc = banana_classify_banana_transformer(NULL, &json);
    EXPECT(rc != 0, "null input must be rejected by transformer ABI");
    EXPECT(json == NULL, "null input must not allocate output");

    /* Null output pointer -> rejected. */
    rc = banana_classify_banana_transformer("{\"text\":\"banana\"}", NULL);
    EXPECT(rc != 0, "null output pointer must be rejected by transformer ABI");

    /* Diagnostics variant guards too. */
    rc = banana_classify_banana_transformer_ex(NULL, 0, NULL, NULL, &json);
    EXPECT(rc != 0, "null input must be rejected by transformer_ex ABI");
    rc = banana_classify_banana_transformer_ex("{\"text\":\"banana\"}", 0, NULL, NULL, NULL);
    EXPECT(rc != 0, "null output pointer must be rejected by transformer_ex ABI");

    /* Well-formed payload produces the locked JSON contract block. */
    json = NULL;
    rc = banana_classify_banana_transformer("{\"text\":\"ripe banana peel smoothie banana bunch banana bread\"}", &json);
    EXPECT(rc == 0, "well-formed payload must succeed");
    EXPECT(json != NULL, "well-formed payload must allocate JSON output");
    if (json) {
        EXPECT(contains(json, "\"model\":\"transformer\""), "JSON must declare model=transformer");
        EXPECT(contains(json, "\"label\":\"banana\""), "clearly-banana payload must label=banana");
        EXPECT(contains(json, "\"banana_score\":"), "JSON must contain banana_score");
        EXPECT(contains(json, "\"not_banana_score\":"), "JSON must contain not_banana_score");
        EXPECT(contains(json, "\"jaccard\":"), "JSON must contain jaccard");
        EXPECT(contains(json, "\"confusion_matrix\":"), "JSON must contain confusion_matrix");
        banana_free(json);
    }
}

/* -------------------------- US2: idempotence ----------------------------- */

static void test_us2_idempotence(void) {
    const char* payloads[] = {
        "{\"text\":\"ripe banana peel smoothie banana bunch banana bread\"}",
        "{\"text\":\"plastic engine oil junk waste motor oil\"}",
        "{\"text\":\"\"}"
    };
    size_t p;

    for (p = 0; p < sizeof(payloads) / sizeof(payloads[0]); ++p) {
        char* outputs[5] = {0};
        int i;
        int all_match = 1;

        for (i = 0; i < 5; ++i) {
            int rc = banana_classify_banana_transformer(payloads[p], &outputs[i]);
            EXPECT(rc == 0, "transformer call must succeed for idempotence sweep");
            EXPECT(outputs[i] != NULL, "transformer call must allocate output");
        }
        for (i = 1; i < 5; ++i) {
            if (!outputs[0] || !outputs[i] || strcmp(outputs[0], outputs[i]) != 0) {
                all_match = 0;
                break;
            }
        }
        EXPECT(all_match, "5 repeated calls must produce byte-identical JSON");
        for (i = 0; i < 5; ++i) {
            if (outputs[i]) {
                banana_free(outputs[i]);
            }
        }
    }
}

/* -------------------------- US3: embedding helper ------------------------ */

static void test_us3_embedding_helper(void) {
    double embedding[BANANA_ML_TRANSFORMER_EMBEDDING_DIM];
    char* json = NULL;
    int rc;

    /* Helper failure modes. */
    EXPECT(assert_embedding_valid(NULL, BANANA_ML_TRANSFORMER_EMBEDDING_DIM, BANANA_ML_TRANSFORMER_EMBEDDING_MAX_L2) == 0,
           "helper must reject NULL vector");
    EXPECT(assert_embedding_valid(embedding, 0, BANANA_ML_TRANSFORMER_EMBEDDING_MAX_L2) == 0,
           "helper must reject zero dim");
    {
        double bad[BANANA_ML_TRANSFORMER_EMBEDDING_DIM];
        bad[0] = (double)NAN;
        bad[1] = 0.0; bad[2] = 0.0; bad[3] = 0.0;
        EXPECT(assert_embedding_valid(bad, BANANA_ML_TRANSFORMER_EMBEDDING_DIM,
                                       BANANA_ML_TRANSFORMER_EMBEDDING_MAX_L2) == 0,
               "helper must reject NaN component");
    }

    /* Apply helper to embedding emitted via the diagnostics ABI. */
    embedding[0] = embedding[1] = embedding[2] = embedding[3] = 0.0;
    rc = banana_classify_banana_transformer_ex(
        "{\"text\":\"ripe banana peel smoothie banana bunch banana bread\"}",
        0, embedding, NULL, &json);
    EXPECT(rc == 0, "transformer_ex must succeed for embedding capture");
    EXPECT(json != NULL, "transformer_ex must allocate JSON");
    EXPECT(assert_embedding_valid(embedding, BANANA_ML_TRANSFORMER_EMBEDDING_DIM,
                                   BANANA_ML_TRANSFORMER_EMBEDDING_MAX_L2),
           "embedding fingerprint must be finite and within max L2 bound");
    /* Probability slot (index 3) must be in [0,1]. */
    EXPECT(embedding[3] >= 0.0 && embedding[3] <= 1.0,
           "embedding probability slot must be in [0,1]");
    if (json) {
        banana_free(json);
        json = NULL;
    }
}

/* -------------------------- US4: attention toggle ------------------------ */

static void test_us4_attention_toggle(void) {
    const char* payload = "{\"text\":\"yellow fruit on the counter maybe\"}";
    double attention_on[BANANA_ML_TRANSFORMER_ATTENTION_DIM];
    double attention_off[BANANA_ML_TRANSFORMER_ATTENTION_DIM];
    char* json_baseline = NULL;
    char* json_off = NULL;
    char* json_on = NULL;
    int rc;
    size_t i;
    double sum;

    /* Baseline (legacy ABI). */
    rc = banana_classify_banana_transformer(payload, &json_baseline);
    EXPECT(rc == 0, "baseline call must succeed");

    /* Diagnostics ABI with toggle OFF and a buffer present -- buffer must
     * be left untouched (sentinel value preserved) and JSON must match
     * the baseline byte-for-byte (FB-R03 default-off equivalence). */
    for (i = 0; i < BANANA_ML_TRANSFORMER_ATTENTION_DIM; ++i) {
        attention_off[i] = -42.0;
    }
    rc = banana_classify_banana_transformer_ex(payload, 0, NULL, attention_off, &json_off);
    EXPECT(rc == 0, "diagnostics-off call must succeed");
    for (i = 0; i < BANANA_ML_TRANSFORMER_ATTENTION_DIM; ++i) {
        EXPECT(attention_off[i] == -42.0, "attention buffer must be untouched when toggle off");
    }
    EXPECT(json_baseline && json_off && strcmp(json_baseline, json_off) == 0,
           "diagnostics-off JSON must be byte-identical to baseline");

    /* Diagnostics ABI with toggle ON -- buffer populated with finite
     * weights summing to ~1.0. */
    for (i = 0; i < BANANA_ML_TRANSFORMER_ATTENTION_DIM; ++i) {
        attention_on[i] = -1.0;
    }
    rc = banana_classify_banana_transformer_ex(payload, 1, NULL, attention_on, &json_on);
    EXPECT(rc == 0, "diagnostics-on call must succeed");
    sum = 0.0;
    for (i = 0; i < BANANA_ML_TRANSFORMER_ATTENTION_DIM; ++i) {
        EXPECT(isfinite(attention_on[i]), "attention weight must be finite");
        EXPECT(attention_on[i] >= 0.0 && attention_on[i] <= 1.0,
               "attention weight must be in [0,1]");
        sum += attention_on[i];
    }
    EXPECT(fabs(sum - 1.0) < 1e-6, "attention weights must sum to ~1.0");
    /* JSON output remains the locked contract regardless of attention toggle. */
    EXPECT(json_on && strcmp(json_baseline, json_on) == 0,
           "diagnostics-on JSON must remain byte-identical to baseline");

    if (json_baseline) banana_free(json_baseline);
    if (json_off) banana_free(json_off);
    if (json_on) banana_free(json_on);
}

/* ------------------- Anchor sanity (US1 + integration) -------------------- */

static void test_calibration_anchors(void) {
    char* json = NULL;
    int rc;

    rc = banana_classify_banana_transformer("{\"text\":\"plastic engine oil junk waste motor oil\"}", &json);
    EXPECT(rc == 0, "clearly-not-banana call must succeed");
    EXPECT(json && contains(json, "\"label\":\"not_banana\""),
           "clearly-not-banana payload must label=not_banana");
    if (json) { banana_free(json); json = NULL; }

    /* Banana-yellow decoy: transformer rejects it where binary is permissive. */
    rc = banana_classify_banana_transformer("{\"text\":\"yellow plastic toy shaped like a banana\"}", &json);
    EXPECT(rc == 0, "decoy call must succeed");
    EXPECT(json && contains(json, "\"label\":\"not_banana\""),
           "yellow-plastic-banana decoy must label=not_banana via transformer");
    if (json) { banana_free(json); json = NULL; }
}

int main(void) {
    test_us1_input_contract();
    test_us2_idempotence();
    test_us3_embedding_helper();
    test_us4_attention_toggle();
    test_calibration_anchors();

    if (g_failures != 0) {
        fprintf(stderr, "test_ml_transformer: %d failure(s)\n", g_failures);
        return 1;
    }
    printf("test_ml_transformer: all checks passed\n");
    return 0;
}
