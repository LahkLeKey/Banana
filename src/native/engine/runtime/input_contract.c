#include "input_contract.h"

#include <math.h>

static float clampf_local(float v, float lo, float hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

int runtime_input_contract_get_click_count(void)
{
    return 0;
}

int runtime_input_contract_get_target_reached_count(void)
{
    return 0;
}

int runtime_input_contract_get_has_move_target(void)
{
    return 0;
}

int runtime_input_contract_handle_right_click(float canvas_x, float canvas_y)
{
    (void)canvas_x;
    (void)canvas_y;
    return 0;
}

int runtime_input_contract_handle_right_click_normalized(float screen_x, float screen_y)
{
    (void)screen_x;
    (void)screen_y;
    return 0;
}

void runtime_input_contract_sanitize_move_input(float input_x,
                                                float input_z,
                                                float *out_move_x,
                                                float *out_move_z)
{
    if (!out_move_x || !out_move_z)
        return;

    if (!isfinite(input_x))
        input_x = 0.f;
    if (!isfinite(input_z))
        input_z = 0.f;

    *out_move_x = clampf_local(input_x, -1.f, 1.f);
    *out_move_z = clampf_local(input_z, -1.f, 1.f);
}
