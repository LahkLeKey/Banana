#include "input_contract.h"

#include <math.h>

static int s_click_count = 0;
static int s_target_reached_count = 0;
static int s_has_move_target = 0;

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
    return s_click_count;
}

int runtime_input_contract_get_target_reached_count(void)
{
    return s_target_reached_count;
}

int runtime_input_contract_get_has_move_target(void)
{
    return s_has_move_target;
}

int runtime_input_contract_handle_right_click(float canvas_x, float canvas_y)
{
    if (!isfinite(canvas_x) || !isfinite(canvas_y))
        return 0;

    s_click_count += 1;
    s_has_move_target = 1;
    return 1;
}

int runtime_input_contract_handle_right_click_normalized(float screen_x, float screen_y)
{
    if (!isfinite(screen_x) || !isfinite(screen_y))
        return 0;

    if (screen_x < -1.f || screen_x > 1.f || screen_y < -1.f || screen_y > 1.f)
        return 0;

    s_has_move_target = 1;
    return 1;
}

void runtime_input_contract_set_has_move_target(int has_move_target)
{
    s_has_move_target = has_move_target ? 1 : 0;
}

void runtime_input_contract_mark_target_reached(void)
{
    s_target_reached_count += 1;
    s_has_move_target = 0;
}

void runtime_input_contract_reset(void)
{
    s_click_count = 0;
    s_target_reached_count = 0;
    s_has_move_target = 0;
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
