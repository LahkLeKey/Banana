/*
 * test_ml_binary.c -- 011 Right Brain Binary Runnable C Code.
 *
 * Validates the contracts documented in:
 *   src/native/core/domain/ml/binary/banana_ml_binary.h
 *   src/native/wrapper/domain/ml/binary/banana_wrapper_ml_binary.h
 *   src/native/wrapper/banana_wrapper.h (additive _with_threshold export)
 *
 * Coverage areas:
 *   US1 -- Pinned operating point + affine calibration anchor (positive sample)
 *   US2 -- Negative-sample coverage (oil, empty, banana-adjacent)
 *   US3 -- Configurable threshold + monotone sweep + out-of-range rejection
 *   US4 -- Locked JSON field-presence contract + null-pointer guards
 */

#include "../../src/native/wrapper/banana_wrapper.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Tiny substring helper to keep tests self-contained. */
static int contains(const char* haystack, const char* needle) {
    if (!haystack || !needle) return 0;
    return strstr(haystack, needle) != NULL;
}

/* --- US1: pinned operating point + positive calibration anchor --- */

static void test_us1_positive_anchor_label_and_confidence(void) {
    /* Anchor: clearly-banana payload. Observed banana_score ~0.944. */
    const char* payload = "{\"text\":\"ripe banana peel smoothie banana bunch banana bread\"}";
    char* json = NULL;
    int rc = banana_classify_banana_binary(payload, &json);
    assert(rc == BANANA_OK);
    assert(json != NULL);
    assert(contains(json, "\"label\":\"banana\""));
    /* Confidence must clear default operating point (0.50) with margin >= 0.85. */
    assert(contains(json, "\"confidence\":0.94"));
    banana_free(json);
}

/* --- US2: negative-sample coverage --- */

static void test_us2_oil_payload_rejected(void) {
    /* Anchor: clearly-not-banana payload. Observed banana_score ~0.093. */
    const char* payload = "{\"text\":\"plastic engine oil junk waste motor oil\"}";
    char* json = NULL;
    int rc = banana_classify_banana_binary(payload, &json);
    assert(rc == BANANA_OK);
    assert(contains(json, "\"label\":\"not_banana\""));
    banana_free(json);
}

static void test_us2_empty_text_documented(void) {
    /* Empty text on the default classifier: deterministic banana_score ~0.583,
     * label=banana. Strict junk variant must instead reject. Both behaviors are
     * documented and asserted here so future drift is caught. */
    const char* payload = "{\"text\":\"\"}";
    char* json = NULL;

    int rc = banana_classify_banana_binary(payload, &json);
    assert(rc == BANANA_OK);
    assert(contains(json, "\"label\":\"banana\""));
    banana_free(json);
    json = NULL;

    rc = banana_classify_not_banana_junk(payload, &json);
    assert(rc == BANANA_OK);
    assert(contains(json, "\"label\":\"not_banana\""));
    banana_free(json);
}

static void test_us2_banana_adjacent_rejected_by_junk_variant(void) {
    /* "Yellow plastic toy shaped like a banana" should NOT be classified as
     * banana under the strict junk-bias variant (RB-R04: bound false-positive
     * cost). Default variant accepts it (banana_score ~0.523), which is the
     * documented permissive operating point. */
    const char* payload = "{\"text\":\"yellow plastic toy shaped like a banana\"}";
    char* json = NULL;

    int rc = banana_classify_not_banana_junk(payload, &json);
    assert(rc == BANANA_OK);
    assert(contains(json, "\"label\":\"not_banana\""));
    banana_free(json);
}

/* --- US3: configurable threshold and monotone sweep --- */

static void test_us3_threshold_out_of_range_rejected(void) {
    const char* payload = "{\"text\":\"yellow ripe banana\"}";
    char* json = NULL;

    int rc = banana_classify_banana_binary_with_threshold(payload, -0.1, &json);
    assert(rc == BANANA_INVALID_ARGUMENT);
    assert(json == NULL);

    rc = banana_classify_banana_binary_with_threshold(payload, 1.5, &json);
    assert(rc == BANANA_INVALID_ARGUMENT);
    assert(json == NULL);
}

static void test_us3_monotone_threshold_sweep(void) {
    /* Payload "banana-yellow" produces banana_score ~0.523 with the default
     * operating point. Sweeping threshold from 0.10 -> 0.90 must produce
     * exactly one banana -> not_banana label transition (monotone). */
    const char* payload = "{\"text\":\"yellow plastic toy shaped like a banana\"}";
    const double thresholds[] = { 0.10, 0.30, 0.50, 0.70, 0.90 };
    int prev_was_banana = 1; /* sweep starts at banana side */
    int transitions = 0;
    size_t i;

    for (i = 0; i < sizeof(thresholds) / sizeof(thresholds[0]); ++i) {
        char* json = NULL;
        int rc = banana_classify_banana_binary_with_threshold(payload, thresholds[i], &json);
        int is_banana;
        assert(rc == BANANA_OK);
        assert(json != NULL);
        is_banana = contains(json, "\"label\":\"banana\"");
        if (i > 0 && is_banana != prev_was_banana) {
            transitions += 1;
        }
        /* Once we leave banana, we must NOT re-enter (monotone). */
        if (i > 0 && prev_was_banana == 0) {
            assert(is_banana == 0);
        }
        prev_was_banana = is_banana;
        banana_free(json);
    }
    assert(transitions == 1);
}

/* --- US4: locked JSON contract field-presence + null-pointer guards --- */

static void test_us4_json_field_presence(void) {
    const char* payload = "{\"text\":\"yellow ripe banana\"}";
    char* json = NULL;
    int rc = banana_classify_banana_binary(payload, &json);
    assert(rc == BANANA_OK);
    assert(json != NULL);

    assert(contains(json, "\"model\":\"binary\""));
    assert(contains(json, "\"label\":"));
    assert(contains(json, "\"confidence\":"));
    assert(contains(json, "\"banana_score\":"));
    assert(contains(json, "\"not_banana_score\":"));
    assert(contains(json, "\"jaccard\":"));
    assert(contains(json, "\"confusion_matrix\":{"));
    assert(contains(json, "\"tp\":"));
    assert(contains(json, "\"fp\":"));
    assert(contains(json, "\"fn\":"));
    assert(contains(json, "\"tn\":"));
    banana_free(json);
}

static void test_us4_null_input_rejected(void) {
    char* json = NULL;
    int rc = banana_classify_banana_binary(NULL, &json);
    assert(rc == BANANA_INVALID_ARGUMENT);
    assert(json == NULL);
}

static void test_us4_null_output_rejected(void) {
    int rc = banana_classify_banana_binary("{\"text\":\"banana\"}", NULL);
    assert(rc == BANANA_INVALID_ARGUMENT);
}

static void test_us4_null_threshold_input_rejected(void) {
    char* json = NULL;
    int rc = banana_classify_banana_binary_with_threshold(NULL, 0.5, &json);
    assert(rc == BANANA_INVALID_ARGUMENT);
    assert(json == NULL);

    rc = banana_classify_banana_binary_with_threshold("{\"text\":\"banana\"}", 0.5, NULL);
    assert(rc == BANANA_INVALID_ARGUMENT);
}

int main(void) {
    /* US1 */
    test_us1_positive_anchor_label_and_confidence();

    /* US2 */
    test_us2_oil_payload_rejected();
    test_us2_empty_text_documented();
    test_us2_banana_adjacent_rejected_by_junk_variant();

    /* US3 */
    test_us3_threshold_out_of_range_rejected();
    test_us3_monotone_threshold_sweep();

    /* US4 */
    test_us4_json_field_presence();
    test_us4_null_input_rejected();
    test_us4_null_output_rejected();
    test_us4_null_threshold_input_rejected();

    printf("test_ml_binary: 10/10 passed\n");
    return 0;
}
