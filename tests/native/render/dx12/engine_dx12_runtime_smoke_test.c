#include "engine.h"
#include "render/backend_dx12.h"

#include <stdio.h>
#include <string.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    const char *status = banana_dx12_backend_status();
    const char *telemetry = NULL;

    if (!expect_true("backend status available before init", status && status[0] != '\0'))
        return 1;

    engine_shutdown();
    if (!expect_true("engine_init", engine_init(640, 360) == 0))
        return 1;

    if (!expect_true("engine_tick", engine_tick(1.0f / 60.0f) == 0))
        return 1;

    telemetry = banana_dx12_runtime_telemetry();
    if (!expect_true("telemetry present", telemetry && telemetry[0] != '\0'))
        return 1;
    if (!expect_true("telemetry includes status field", strstr(telemetry, "status=") != NULL))
        return 1;
    if (!expect_true("telemetry includes active field", strstr(telemetry, "active=") != NULL))
        return 1;

    /* Frames presented can be zero in headless/fallback paths; this is a smoke call contract. */
    (void)banana_dx12_runtime_frames_presented();
    (void)banana_dx12_runtime_scene_draw_calls();

    engine_shutdown();
    return 0;
}
