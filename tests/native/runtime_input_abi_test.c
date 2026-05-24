#include "runtime/engine_state.h"
#include "runtime/input/input_abi.h"
#include "runtime/input/input_contract.h"

#include <math.h>
#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    EngineRuntimeState state = {0};
    RuntimeInputCanvasPoint canvas;
    RuntimeScreenNormalizedPoint normalized;

    runtime_input_contract_reset();

    state.viewport_width = 200;
    state.viewport_height = 100;

    canvas.x = 100.f;
    canvas.y = 50.f;

    if (!expect_int("canvas click accepted",
                    runtime_input_abi_handle_right_click(&state, canvas, NULL),
                    1))
        return 1;

    if (!expect_int("click count incremented", runtime_input_abi_get_click_count(), 1))
        return 1;

    if (!expect_int("target flagged after canvas click",
                    runtime_input_abi_get_has_move_target(),
                    1))
        return 1;

    runtime_input_abi_set_move_input(&state, 0.35f, 0.f);
    if (!expect_int("move input clears target", runtime_input_abi_get_has_move_target(), 0))
        return 1;

    normalized.x = 0.f;
    normalized.y = 0.f;
    if (!expect_int("normalized click accepted",
                    runtime_input_abi_handle_right_click_normalized(&state, normalized, NULL),
                    1))
        return 1;

    if (!expect_int("normalized click keeps click count",
                    runtime_input_abi_get_click_count(),
                    1))
        return 1;

    if (!expect_int("normalized click sets target", runtime_input_abi_get_has_move_target(), 1))
        return 1;

    normalized.x = 2.5f;
    normalized.y = 0.f;
    if (!expect_int("out of range normalized rejected",
                    runtime_input_abi_handle_right_click_normalized(&state, normalized, NULL),
                    0))
        return 1;

    normalized.x = 0.f;
    normalized.y = 0.f;
    if (!expect_int("canvas convert valid",
                    runtime_input_abi_canvas_to_normalized(canvas,
                                                           (RuntimeViewportSize){200, 100},
                                                           &normalized),
                    0))
        return 1;

    if (!expect_int("canvas convert invalid viewport",
                    runtime_input_abi_canvas_to_normalized(canvas,
                                                           (RuntimeViewportSize){0, 100},
                                                           &normalized),
                    -1))
        return 1;

    if (!expect_int("canvas convert null output",
                    runtime_input_abi_canvas_to_normalized(canvas,
                                                           (RuntimeViewportSize){200, 100},
                                                           NULL),
                    -1))
        return 1;

    if (!expect_int("canvas convert negative viewport",
                    runtime_input_abi_canvas_to_normalized(canvas,
                                                           (RuntimeViewportSize){200, -1},
                                                           &normalized),
                    -1))
        return 1;

    runtime_input_contract_handle_right_click(10.f, 10.f);
    if (!expect_int("zero input keeps target flag",
                    runtime_input_abi_get_has_move_target(),
                    1))
        return 1;
    runtime_input_abi_set_move_input(&state, 0.f, 0.f);
    if (!expect_int("zero move does not clear target",
                    runtime_input_abi_get_has_move_target(),
                    1))
        return 1;

    runtime_input_abi_set_move_input(&state, NAN, INFINITY);
    if (!expect_int("nan input sanitized no-clear",
                    runtime_input_abi_get_has_move_target(),
                    1))
        return 1;

    if (!expect_int("null state still records click",
                    runtime_input_abi_handle_right_click(NULL, canvas, NULL),
                    1))
        return 1;

    if (!expect_int("click count after null-state click", runtime_input_abi_get_click_count(), 3))
        return 1;

    runtime_input_contract_reset();
    if (!expect_int("reset click count", runtime_input_abi_get_click_count(), 0))
        return 1;

    return 0;
}
