#include "tick_input_phase.h"

#include "../input/input_click_policy.h"
#include "../input/input_contract.h"

int runtime_tick_input_phase_dispatch(void *context,
                                      RuntimeTickInputClickFn apply_click_input,
                                      float click_x,
                                      float click_y,
                                      int input_width,
                                      int input_height)
{
    float normalized_x = 0.f;
    float normalized_y = 0.f;

    runtime_input_contract_handle_right_click(click_x, click_y);

    if (!runtime_input_click_policy_normalize(click_x,
                                              click_y,
                                              input_width,
                                              input_height,
                                              &normalized_x,
                                              &normalized_y))
        return 0;

    runtime_input_contract_handle_right_click_normalized(normalized_x, normalized_y);
    if (apply_click_input)
        apply_click_input(context, normalized_x, normalized_y);

    return 1;
}

void runtime_tick_input_phase_process(Window *window,
                                      void *context,
                                      RuntimeTickInputClickFn apply_click_input)
{
    float click_x = 0.f;
    float click_y = 0.f;
    int input_width = 0;
    int input_height = 0;

    if (!window)
        return;

    if (!window_take_right_click(window, &click_x, &click_y))
        return;

    window_get_input_size(window, &input_width, &input_height);
    runtime_tick_input_phase_dispatch(context,
                                      apply_click_input,
                                      click_x,
                                      click_y,
                                      input_width,
                                      input_height);
}
