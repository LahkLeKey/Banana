// Native deterministic ML tests for regression, binary metrics, and transformer.
#include "../../src/native/wrapper/banana_wrapper.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void assert_contains(const char* text, const char* needle) {
    assert(text != NULL);
    assert(needle != NULL);
    assert(strstr(text, needle) != NULL);
}

int main(void) {
    const char* transformer_payload = "{\"text\":\"ripe banana bunch banana bread\"}";
    double score = 0.0;
    char* binary_json = NULL;
    char* junk_json = NULL;
    char* transformer_json = NULL;
    char* transformer_json_second = NULL;
    int rc;

    rc = banana_predict_banana_regression_score("{\"text\":\"ripe banana peel smoothie\"}", &score);
    assert(rc == BANANA_OK);
    assert(score >= 0.0 && score <= 1.0);
    assert(score > 0.5);

    rc = banana_classify_banana_binary("{\"text\":\"fresh banana peel smoothie\"}", &binary_json);
    assert(rc == BANANA_OK);
    assert(binary_json != NULL);
    assert_contains(binary_json, "\"model\":\"binary\"");
    assert_contains(binary_json, "\"label\":\"banana\"");
    assert_contains(binary_json, "\"jaccard\":");
    assert_contains(binary_json, "\"confusion_matrix\":");
    banana_free(binary_json);

    rc = banana_classify_not_banana_junk("{\"text\":\"plastic engine oil junk waste\"}", &junk_json);
    assert(rc == BANANA_OK);
    assert(junk_json != NULL);
    assert_contains(junk_json, "\"label\":\"not_banana\"");
    banana_free(junk_json);

    rc = banana_classify_banana_transformer(transformer_payload, &transformer_json);
    assert(rc == BANANA_OK);
    assert(transformer_json != NULL);
    assert_contains(transformer_json, "\"model\":\"transformer\"");
    assert_contains(transformer_json, "\"label\":\"banana\"");

    rc = banana_classify_banana_transformer(transformer_payload, &transformer_json_second);
    assert(rc == BANANA_OK);
    assert(transformer_json_second != NULL);
    assert(strcmp(transformer_json, transformer_json_second) == 0);

    banana_free(transformer_json);
    banana_free(transformer_json_second);

    rc = banana_predict_banana_regression_score(NULL, &score);
    assert(rc == BANANA_INVALID_ARGUMENT);

    rc = banana_classify_banana_binary(NULL, &binary_json);
    assert(rc == BANANA_INVALID_ARGUMENT);

    printf("[native-unit] deterministic ML runtime models ok\n");
    return 0;
}