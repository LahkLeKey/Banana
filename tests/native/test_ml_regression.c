/*
 * test_ml_regression.c -- 010 Left Brain Regression Runnable C Code.
 *
 * Validates the contracts documented in:
 *   src/native/core/domain/ml/regression/banana_ml_regression.h
 *   src/native/wrapper/domain/ml/regression/banana_wrapper_ml_regression.h
 *
 * Coverage areas:
 *   US1 -- Locked input/output contract (null guard, in-range output)
 *   US2 -- Boundary behavior and clamping via wrapper ABI
 *   US3 -- Calibration anchors (clearly-banana, ambiguous, clearly-not-banana)
 *
 * Notes:
 *   - The core function banana_ml_regression_predict is exercised indirectly
 *     through the wrapper ABI banana_predict_banana_regression_score, since the
 *     core symbol is not exported from the banana_native shared library.
 *   - The NaN/Inf rejection guard inside banana_ml_regression.c cannot be
 *     reached through the JSON wrapper (the JSON -> feature vector builder
 *     always produces finite ratios). The guard is a defensive contract for any
 *     future caller that bypasses the wrapper.
 */

#include "../../src/native/wrapper/banana_wrapper.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

/* --- US1: locked input/output contract --- */

static void test_us1_null_input_rejected(void) {
    double score = -1.0;
    int rc = banana_predict_banana_regression_score(NULL, &score);
    assert(rc == BANANA_INVALID_ARGUMENT);
    assert(score == -1.0);
}

static void test_us1_null_output_rejected(void) {
    int rc = banana_predict_banana_regression_score("{\"text\":\"banana\"}", NULL);
    assert(rc == BANANA_INVALID_ARGUMENT);
}

static void test_us1_output_is_finite_and_in_range(void) {
    const char* payload = "{\"text\":\"yellow ripe banana ripe banana\"}";
    double score = -1.0;
    int rc = banana_predict_banana_regression_score(payload, &score);
    assert(rc == BANANA_OK);
    assert(isfinite(score));
    assert(score >= 0.0 && score <= 1.0);
}

/* --- US2: boundary behavior and clamping via wrapper ABI --- */

static void test_us2_empty_text_in_range(void) {
    const char* payload = "{\"text\":\"\"}";
    double score = -1.0;
    int rc = banana_predict_banana_regression_score(payload, &score);
    assert(rc == BANANA_OK);
    assert(score >= 0.0 && score <= 1.0);
}

static void test_us2_extreme_banana_in_range(void) {
    const char* payload =
        "{\"text\":\"banana banana banana banana banana banana banana "
        "banana banana banana banana banana banana banana banana\"}";
    double score = -1.0;
    int rc = banana_predict_banana_regression_score(payload, &score);
    assert(rc == BANANA_OK);
    assert(score >= 0.0 && score <= 1.0);
}

static void test_us2_extreme_not_banana_in_range(void) {
    const char* payload =
        "{\"text\":\"engine oil engine oil engine oil engine oil engine oil "
        "engine oil engine oil engine oil engine oil engine oil\"}";
    double score = -1.0;
    int rc = banana_predict_banana_regression_score(payload, &score);
    assert(rc == BANANA_OK);
    assert(score >= 0.0 && score <= 1.0);
}

static void test_us2_non_json_text_in_range(void) {
    /* Wrapper layer accepts free-form text; ensure the regression scorer still
     * produces an in-range, finite score (US2 clamp contract). */
    double score = -1.0;
    int rc = banana_predict_banana_regression_score("not json", &score);
    assert(rc == BANANA_OK);
    assert(isfinite(score));
    assert(score >= 0.0 && score <= 1.0);
}

/* --- US3: calibration anchors via wrapper ABI --- */

static void test_us3_calibration_clearly_banana(void) {
    /* Anchor: clearly-banana payload. Observed score ~0.90. Pin > 0.65. */
    const char* payload = "{\"text\":\"ripe banana peel smoothie banana bunch banana bread\"}";
    double score = -1.0;
    int rc = banana_predict_banana_regression_score(payload, &score);
    assert(rc == BANANA_OK);
    assert(score > 0.65);
    assert(score <= 1.0);
}

static void test_us3_calibration_ambiguous(void) {
    /* Anchor: empty-text ambiguous payload. Observed score ~0.59. Pin in (0.30, 0.70). */
    const char* payload = "{\"text\":\"\"}";
    double score = -1.0;
    int rc = banana_predict_banana_regression_score(payload, &score);
    assert(rc == BANANA_OK);
    assert(score > 0.30);
    assert(score < 0.70);
}

static void test_us3_calibration_clearly_not_banana(void) {
    /* Anchor: clearly-not-banana payload. Observed score ~0.19. Pin < 0.35. */
    const char* payload = "{\"text\":\"plastic engine oil junk waste motor oil\"}";
    double score = -1.0;
    int rc = banana_predict_banana_regression_score(payload, &score);
    assert(rc == BANANA_OK);
    assert(score < 0.35);
    assert(score >= 0.0);
}

int main(void) {
    test_us1_null_input_rejected();
    test_us1_null_output_rejected();
    test_us1_output_is_finite_and_in_range();

    test_us2_empty_text_in_range();
    test_us2_extreme_banana_in_range();
    test_us2_extreme_not_banana_in_range();
    test_us2_non_json_text_in_range();

    test_us3_calibration_clearly_banana();
    test_us3_calibration_ambiguous();
    test_us3_calibration_clearly_not_banana();

    printf("test_ml_regression: 10/10 passed\n");
    return 0;
}
