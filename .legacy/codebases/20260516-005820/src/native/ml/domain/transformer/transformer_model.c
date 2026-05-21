#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Full brain: transformer model
int ml_transformer_infer(const char *input_json, char *out_json, size_t out_size) {
    // TODO: Implement transformer inference
    snprintf(out_json, out_size, "{\"result\":\"full-brain\"}");
    return 0;
}
