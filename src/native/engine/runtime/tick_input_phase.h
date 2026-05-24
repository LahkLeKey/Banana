#ifndef BANANA_ENGINE_RUNTIME_TICK_INPUT_PHASE_H
#define BANANA_ENGINE_RUNTIME_TICK_INPUT_PHASE_H

#include "../render/window.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void (*RuntimeTickInputClickFn)(void *context, float normalized_x, float normalized_y);

    int runtime_tick_input_phase_dispatch(void *context,
                                          RuntimeTickInputClickFn apply_click_input,
                                          float click_x,
                                          float click_y,
                                          int input_width,
                                          int input_height);

    void runtime_tick_input_phase_process(Window *window,
                                          void *context,
                                          RuntimeTickInputClickFn apply_click_input);

#ifdef __cplusplus
}
#endif

#endif
