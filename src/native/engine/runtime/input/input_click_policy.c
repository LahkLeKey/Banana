#include "input_click_policy.h"

int runtime_input_click_policy_normalize(float click_x,
                                         float click_y,
                                         int input_width,
                                         int input_height,
                                         float *out_normalized_x,
                                         float *out_normalized_y)
{
    if (!out_normalized_x || !out_normalized_y)
        return 0;

    if (input_width <= 0 || input_height <= 0)
        return 0;

    *out_normalized_x = (click_x / (float)input_width) * 2.f - 1.f;
    *out_normalized_y = 1.f - (click_y / (float)input_height) * 2.f;
    return 1;
}
