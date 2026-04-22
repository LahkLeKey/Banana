#ifndef BANANA_NOT_BANANA_H
#define BANANA_NOT_BANANA_H

#include "banana_domain_status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum BananaNotBananaLabel {
    BANANA_NOT_BANANA_LABEL_NOT_BANANA = 0,
    BANANA_NOT_BANANA_LABEL_BANANA = 1,
    BANANA_NOT_BANANA_LABEL_INDETERMINATE = 2
} BananaNotBananaLabel;

typedef struct BananaNotBananaClassification {
    BananaNotBananaLabel predicted_label;
    int actor_count;
    int entity_count;
    int signal_token_count;
    int total_token_count;
    double banana_probability;
    double not_banana_probability;
    double junk_confidence;
} BananaNotBananaClassification;

/*
 * Polymorphic junk classifier.
 *
 * Inputs:
 *  - tokens: array of UTF-8/ASCII tokens already extracted from polymorphic
 *    actor/entity/junk payload fields. May be null when token_count == 0.
 *    Individual entries may be null or empty and will be ignored. Comparison
 *    is case-insensitive over the ASCII range; non-ASCII bytes pass through
 *    unchanged.
 *  - token_count: number of entries in tokens. Must be >= 0.
 *  - actor_count: number of polymorphic actors present in the request.
 *  - entity_count: number of polymorphic entities present in the request.
 *
 * Output:
 *  - out_classification: populated label, banana/not-banana probabilities,
 *    junk confidence, and counts. predicted_label is INDETERMINATE only when
 *    no tokens, no actors, and no entities are supplied.
 */
BananaStatus banana_not_banana_classify(
    const char* const* tokens,
    int token_count,
    int actor_count,
    int entity_count,
    BananaNotBananaClassification* out_classification
);

#ifdef __cplusplus
}
#endif

#endif
