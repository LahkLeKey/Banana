#include "engine.h"
#include "render/backend_dx12.h"

#include <stdio.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    const char *telemetry = NULL;

    engine_shutdown();
    if (!expect_true("engine_init", engine_init(640, 360) == 0))
        return 1;

    if (!expect_true("engine_tick", engine_tick(1.0f / 60.0f) == 0))
        return 1;

    telemetry = banana_dx12_runtime_telemetry();
    if (!expect_true("telemetry present", telemetry && telemetry[0] != '\0'))
        return 1;

    /* Frames presented can be zero in headless/fallback paths; this is a smoke call contract. */
    (void)banana_dx12_runtime_frames_presented();

    engine_shutdown();
    return 0;
}
