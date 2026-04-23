#include "banana_not_banana.h"

#include <stddef.h>

#if defined(__has_include)
#if __has_include("generated/banana_signal_tokens.h")
#include "generated/banana_signal_tokens.h"
#define BANANA_NOT_BANANA_HAS_GENERATED_VOCABULARY 1
#endif
#endif

/*
 * Banana signal vocabulary used by the polymorphic junk classifier.
 * Prefer generated vocabulary from scripts/train-not-banana-model.py when
 * available. Fallback list below preserves deterministic behavior when the
 * generated header is not staged into src/native/core/domain/generated.
 */
#ifndef BANANA_NOT_BANANA_HAS_GENERATED_VOCABULARY
static const char* const k_banana_signal_tokens[] = {
    "banana",
    "bananas",
    "plantain",
    "plantains",
    "cavendish",
    "musa",
    "ripe",
    "ripeness",
    "ripening",
    "peel",
    "peels",
    "ethylene",
    "yellow",
    "fruit",
    "harvest",
    "bunch",
    "batch"
};

static const int k_banana_signal_token_count =
    (int)(sizeof(k_banana_signal_tokens) / sizeof(k_banana_signal_tokens[0]));
#endif

#ifdef BANANA_NOT_BANANA_HAS_GENERATED_VOCABULARY
#define BANANA_SIGNAL_TOKENS k_banana_signal_tokens_generated
#define BANANA_SIGNAL_TOKEN_COUNT k_banana_signal_token_count_generated
#else
#define BANANA_SIGNAL_TOKENS k_banana_signal_tokens
#define BANANA_SIGNAL_TOKEN_COUNT k_banana_signal_token_count
#endif

/*
 * Structural polymorphic envelope tokens are intentionally ignored during
 * scoring so that payload scaffolding does not dilute true banana evidence.
 * Keep this list in sync with STRUCTURAL_STOP_WORDS in
 * scripts/train-not-banana-model.py.
 */
static const char* const k_structural_stop_tokens[] = {
    "actor",
    "actorid",
    "actorkey",
    "actors",
    "actortype",
    "entity",
    "entityid",
    "entitykey",
    "entities",
    "entitytype",
    "junk",
    "kind",
    "metadata",
    "type",
    "id",
    "key"
};

static const int k_structural_stop_token_count =
    (int)(sizeof(k_structural_stop_tokens) / sizeof(k_structural_stop_tokens[0]));

static int banana_not_banana_to_lower_ascii(int byte_value) {
    if (byte_value >= 'A' && byte_value <= 'Z') {
        return byte_value + ('a' - 'A');
    }

    return byte_value;
}

static int banana_not_banana_token_is_empty(const char* token) {
    if (token == NULL) {
        return 1;
    }

    if (token[0] == '\0') {
        return 1;
    }

    return 0;
}

static int banana_not_banana_tokens_equal_ci(const char* candidate, const char* signal) {
    int index = 0;

    if (candidate == NULL || signal == NULL) {
        return 0;
    }

    while (1) {
        int candidate_byte = (unsigned char)candidate[index];
        int signal_byte = (unsigned char)signal[index];

        int candidate_lower = banana_not_banana_to_lower_ascii(candidate_byte);
        int signal_lower = banana_not_banana_to_lower_ascii(signal_byte);

        if (candidate_lower != signal_lower) {
            return 0;
        }

        if (candidate_byte == 0) {
            return 1;
        }

        index++;
    }
}

static int banana_not_banana_token_is_signal(const char* token) {
    int signal_index = 0;

    if (banana_not_banana_token_is_empty(token)) {
        return 0;
    }

    for (signal_index = 0; signal_index < BANANA_SIGNAL_TOKEN_COUNT; signal_index++) {
        if (banana_not_banana_tokens_equal_ci(token, BANANA_SIGNAL_TOKENS[signal_index])) {
            return 1;
        }
    }

    return 0;
}

static int banana_not_banana_token_is_structural_stop_word(const char* token) {
    int token_index = 0;

    if (banana_not_banana_token_is_empty(token)) {
        return 0;
    }

    for (token_index = 0; token_index < k_structural_stop_token_count; token_index++) {
        if (banana_not_banana_tokens_equal_ci(token, k_structural_stop_tokens[token_index])) {
            return 1;
        }
    }

    return 0;
}

static double banana_not_banana_clamp_unit(double value) {
    if (value < 0.0) {
        return 0.0;
    }

    if (value > 1.0) {
        return 1.0;
    }

    return value;
}

BananaStatus banana_not_banana_classify(
    const char* const* tokens,
    int token_count,
    int actor_count,
    int entity_count,
    BananaNotBananaClassification* out_classification
) {
    int token_index = 0;
    int signal_count = 0;
    int considered_count = 0;
    double banana_probability = 0.0;
    double not_banana_probability = 0.0;
    double junk_confidence = 0.0;

    if (out_classification == NULL) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (token_count < 0 || actor_count < 0 || entity_count < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (token_count > 0 && tokens == NULL) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    for (token_index = 0; token_index < token_count; token_index++) {
        const char* token = tokens[token_index];

        if (banana_not_banana_token_is_empty(token)) {
            continue;
        }

        if (banana_not_banana_token_is_structural_stop_word(token)) {
            continue;
        }

        considered_count++;

        if (banana_not_banana_token_is_signal(token)) {
            signal_count++;
        }
    }

    out_classification->actor_count = actor_count;
    out_classification->entity_count = entity_count;
    out_classification->signal_token_count = signal_count;
    out_classification->total_token_count = considered_count;

    if (considered_count == 0) {
        if (actor_count == 0 && entity_count == 0) {
            out_classification->predicted_label = BANANA_NOT_BANANA_LABEL_INDETERMINATE;
            out_classification->banana_probability = 0.0;
            out_classification->not_banana_probability = 0.0;
            out_classification->junk_confidence = 0.0;
            return BANANA_OK;
        }

        out_classification->predicted_label = BANANA_NOT_BANANA_LABEL_NOT_BANANA;
        out_classification->banana_probability = 0.0;
        out_classification->not_banana_probability = 1.0;
        out_classification->junk_confidence = 0.9;
        return BANANA_OK;
    }

    banana_probability = banana_not_banana_clamp_unit(
        (double)signal_count / (double)considered_count);
    not_banana_probability = 1.0 - banana_probability;

    if (actor_count + entity_count > 0) {
        junk_confidence = not_banana_probability;
    } else {
        junk_confidence = not_banana_probability * 0.5;
    }

    out_classification->banana_probability = banana_probability;
    out_classification->not_banana_probability = not_banana_probability;
    out_classification->junk_confidence = banana_not_banana_clamp_unit(junk_confidence);
    out_classification->predicted_label = banana_probability >= 0.5
        ? BANANA_NOT_BANANA_LABEL_BANANA
        : BANANA_NOT_BANANA_LABEL_NOT_BANANA;

    return BANANA_OK;
}
