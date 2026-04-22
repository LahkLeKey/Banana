#include "../banana_wrapper_internal.h"

#include "../../core/domain/banana_not_banana.h"

_Static_assert(
    CINTEROP_BANANA_NOT_BANANA_LABEL_NOT_BANANA == BANANA_NOT_BANANA_LABEL_NOT_BANANA,
    "Wrapper ABI NOT_BANANA label ordinal must match core not-banana label ordinal.");
_Static_assert(
    CINTEROP_BANANA_NOT_BANANA_LABEL_BANANA == BANANA_NOT_BANANA_LABEL_BANANA,
    "Wrapper ABI BANANA label ordinal must match core not-banana label ordinal.");
_Static_assert(
    CINTEROP_BANANA_NOT_BANANA_LABEL_INDETERMINATE == BANANA_NOT_BANANA_LABEL_INDETERMINATE,
    "Wrapper ABI INDETERMINATE label ordinal must match core not-banana label ordinal.");

int banana_classify_not_banana_junk(
    const char* const* tokens,
    int token_count,
    int actor_count,
    int entity_count,
    CInteropBananaNotBananaClassification* out_classification
) {
    BananaNotBananaClassification classification;
    BananaStatus status = BANANA_OK;

    if (out_classification == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    status = banana_not_banana_classify(
        tokens,
        token_count,
        actor_count,
        entity_count,
        &classification
    );

    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    out_classification->predicted_label = (CInteropBananaNotBananaLabel)classification.predicted_label;
    out_classification->actor_count = classification.actor_count;
    out_classification->entity_count = classification.entity_count;
    out_classification->signal_token_count = classification.signal_token_count;
    out_classification->total_token_count = classification.total_token_count;
    out_classification->banana_probability = classification.banana_probability;
    out_classification->not_banana_probability = classification.not_banana_probability;
    out_classification->junk_confidence = classification.junk_confidence;

    return BANANA_STATUS_OK;
}
