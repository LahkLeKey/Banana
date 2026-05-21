#pragma once
#include <stddef.h>

// ML-driven AI controller service API
int ml_controller_infer_action(const char *state_json, char *out_action, size_t out_size);
