#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../regression/regression_model.h"
#include "../binary/binary_model.h"
#include "../transformer/transformer_model.h"
#include "../histogram/histogram_matrix.h"

// ML-driven AI controller logic for server-side use
// This integrates left brain (regression), right brain (binary), and full brain (transformer)

int ml_controller_infer_action(const char *state_json, char *out_action, size_t out_size) {
    double regression_score = 0.0;
    int binary_label = 0;
    char transformer_result[256] = "";

    // Left brain: regression
    ml_regression_predict(state_json, &regression_score);
    ml_histogram_update("regression", regression_score);

    // Right brain: binary classification
    ml_binary_classify(state_json, &binary_label);
    ml_histogram_update("binary", (double)binary_label);

    // Full brain: transformer
    ml_transformer_infer(state_json, transformer_result, sizeof(transformer_result));
    ml_histogram_update("transformer", 1.0); // Example: count inference

    // Example: combine outputs for action selection
    snprintf(out_action, out_size,
        "{\"regression\":%.3f,\"binary\":%d,\"transformer\":%s}",
        regression_score, binary_label, transformer_result);
    return 0;
}
