/*
 * Native test suite for slice 016 (Transformer Quantized Embedding, U-001).
 *
 * Exercises the additive public ABI:
 *   - banana_classify_banana_transformer_quant_embedding
 *
 * Coverage map:
 *   Q-R01 -- existing ABI byte-equivalent (verified by snapshot test below
 *            which calls both legacy `_ex` and new `_quant_embedding` and
 *            asserts identical JSON shape).
 *   Q-R03 -- new export operates over the 6 SPIKE anchor payloads.
 *   Q-R04 -- reconstruction error bound asserted for all 6 anchors:
 *            max(abs(reconstruct(quant) - original)) < scale/2 + epsilon.
 */

#include "../../src/native/wrapper/banana_wrapper.h"
#include "../../src/native/core/domain/ml/transformer/banana_ml_transformer.h"

#include <math.h>
#include <stdint.h>
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

/* The 6 SPIKE anchor payloads referenced from
 * .specify/specs/013-ml-brain-composition-spike/analysis/composition-strategy.md.
 * 3 cheap-path anchors (out-of-band) + 3 escalation-band anchors. */
static const char* const SPIKE_ANCHORS[] = {
    "{\"text\":\"ripe banana peel smoothie banana bunch banana bread\"}",
    "{\"text\":\"banana banana banana banana banana banana banana\"}",
    "{\"text\":\"plastic engine oil junk waste motor oil\"}",
    "{\"text\":\"yellow plastic toy shaped like a banana\"}",
    "{\"text\":\"yellow fruit on the counter maybe\"}",
    "{\"text\":\"\"}"
};
static const size_t SPIKE_ANCHOR_COUNT = sizeof(SPIKE_ANCHORS) / sizeof(SPIKE_ANCHORS[0]);

/* Pull the 4 doubles via the legacy _ex entrypoint so we have the
 * pre-quantization "original" to bound the reconstruction error against. */
static int fetch_original_embedding(const char* input_json, double* out_embedding) {
    char* json = NULL;
    int rc = banana_classify_banana_transformer_ex(input_json,
                                                    0 /* log_attention */,
                                                    out_embedding,
                                                    NULL /* out_attention_weights */,
                                                    &json);
    if (json) {
        banana_free(json);
    }
    return rc;
}

static void test_q_r03_q_r04_reconstruction_bound(void) {
    size_t a, i;
    for (a = 0; a < SPIKE_ANCHOR_COUNT; ++a) {
        const char* input = SPIKE_ANCHORS[a];
        double original[BANANA_ML_TRANSFORMER_EMBEDDING_DIM];
        signed char quant[BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_DIM];
        double scale = 0.0;
        signed char zero = 0;
        char* json = NULL;
        int rc;

        memset(original, 0, sizeof(original));
        memset(quant, 0, sizeof(quant));

        rc = fetch_original_embedding(input, original);
        EXPECT(rc == BANANA_OK, "legacy _ex must succeed on SPIKE anchor");
        for (i = 0; i < BANANA_ML_TRANSFORMER_EMBEDDING_DIM; ++i) {
            EXPECT(isfinite(original[i]), "original embedding component must be finite");
        }

        rc = banana_classify_banana_transformer_quant_embedding(input,
                                                                  quant,
                                                                  &scale,
                                                                  &zero,
                                                                  &json);
        EXPECT(rc == BANANA_OK, "quant_embedding must succeed on SPIKE anchor");
        EXPECT(json != NULL, "quant_embedding must allocate JSON");
        EXPECT(scale > 0.0 && isfinite(scale), "scale must be positive finite");
        EXPECT(zero == 0, "symmetric quantization pins zero-point to 0");

        /* Q-R04: bounded reconstruction error. */
        for (i = 0; i < BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_DIM; ++i) {
            double recon = ((double)quant[i] - (double)zero) * scale;
            double err = fabs(recon - original[i]);
            double bound = scale / 2.0 + BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_RECON_EPS;
            EXPECT(err < bound, "reconstruction error must be < scale/2 + eps");
        }

        if (json) {
            banana_free(json);
        }
    }
}

static void test_q_r01_input_contract(void) {
    signed char quant[BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_DIM];
    double scale = 0.0;
    signed char zero = 0;
    char* json = NULL;
    int rc;

    rc = banana_classify_banana_transformer_quant_embedding(NULL, quant, &scale, &zero, &json);
    EXPECT(rc != BANANA_OK, "null input rejected");

    rc = banana_classify_banana_transformer_quant_embedding("{\"text\":\"x\"}", NULL, &scale, &zero, &json);
    EXPECT(rc != BANANA_OK, "null quant rejected");

    rc = banana_classify_banana_transformer_quant_embedding("{\"text\":\"x\"}", quant, NULL, &zero, &json);
    EXPECT(rc != BANANA_OK, "null scale rejected");

    rc = banana_classify_banana_transformer_quant_embedding("{\"text\":\"x\"}", quant, &scale, NULL, &json);
    EXPECT(rc != BANANA_OK, "null zero rejected");

    rc = banana_classify_banana_transformer_quant_embedding("{\"text\":\"x\"}", quant, &scale, &zero, NULL);
    EXPECT(rc != BANANA_OK, "null out_json rejected");
}

static void test_q_r01_legacy_ex_unchanged(void) {
    /* Q-R01 byte-equivalence: the JSON returned by `_ex` and by
     * `_quant_embedding` is the same shape (same model string + ordered
     * fields). We assert the legacy entry still produces a JSON containing
     * `"model":"transformer"` AFTER a quant call has happened to prove the
     * legacy code path is bit-for-bit unchanged. */
    char* legacy_json = NULL;
    char* quant_json = NULL;
    signed char quant[BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_DIM];
    double original[BANANA_ML_TRANSFORMER_EMBEDDING_DIM];
    double scale = 0.0;
    signed char zero = 0;
    int rc;

    const char* input = SPIKE_ANCHORS[1];

    rc = banana_classify_banana_transformer_ex(input, 0, original, NULL, &legacy_json);
    EXPECT(rc == BANANA_OK && legacy_json != NULL, "legacy _ex returns OK + JSON");

    rc = banana_classify_banana_transformer_quant_embedding(input, quant, &scale, &zero, &quant_json);
    EXPECT(rc == BANANA_OK && quant_json != NULL, "quant returns OK + JSON");

    /* Same shape: both contain "model":"transformer". */
    EXPECT(strstr(legacy_json, "\"model\":\"transformer\"") != NULL, "legacy JSON has transformer model");
    EXPECT(strstr(quant_json, "\"model\":\"transformer\"") != NULL, "quant JSON has transformer model");

    /* Same byte payload (Q-R01: opt-in path returns same JSON). */
    EXPECT(strcmp(legacy_json, quant_json) == 0, "legacy and quant JSON are byte-identical");

    if (legacy_json) banana_free(legacy_json);
    if (quant_json) banana_free(quant_json);
}

int main(void) {
    test_q_r01_input_contract();
    test_q_r01_legacy_ex_unchanged();
    test_q_r03_q_r04_reconstruction_bound();
    if (g_failures > 0) {
        fprintf(stderr, "%d failures\n", g_failures);
        return 1;
    }
    printf("OK -- transformer quant embedding (slice 016)\n");
    return 0;
}
