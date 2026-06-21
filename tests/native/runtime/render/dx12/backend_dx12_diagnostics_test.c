#include "render/dx12/backend_dx12_diagnostics.h"
#include "../../support/test_support.h"

#include <string.h>

static void test_dx12_probe_state_and_status_defaults(void **state)
{
    (void)state;

    banana_dx12_diagnostics_set_probe_state(BANANA_DX12_DIAGNOSTICS_PROBE_AVAILABLE, "dx12-ok");
    BANANA_TEST_ASSERT_INT_EQ((int)banana_dx12_diagnostics_probe_state(),
                              (int)BANANA_DX12_DIAGNOSTICS_PROBE_AVAILABLE,
                              "probe_state must return the most recent state");
    BANANA_TEST_ASSERT_TRUE(strcmp(banana_dx12_diagnostics_probe_status(), "dx12-ok") == 0,
                            "probe_status must return explicit status values");

    banana_dx12_diagnostics_set_probe_state(BANANA_DX12_DIAGNOSTICS_PROBE_UNAVAILABLE, NULL);
    BANANA_TEST_ASSERT_TRUE(strcmp(banana_dx12_diagnostics_probe_status(), "dx12-status-unknown") == 0,
                            "probe_status must fall back when status string is null");
}

static void test_dx12_runtime_telemetry_formats_snapshot_fields(void **state)
{
    (void)state;
    BananaDx12DiagnosticsSnapshot snapshot;
    const char *telemetry = NULL;
    const char *unavailable = banana_dx12_diagnostics_runtime_unavailable_telemetry();

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.status = "running";
    snapshot.active = 1;
    snapshot.width = 1920;
    snapshot.height = 1080;
    snapshot.frame_counter = 40;
    snapshot.frames_presented = 39;
    snapshot.scene_draw_calls_frame = 77;
    snapshot.scene_draw_calls_total = 1234;
    snapshot.present_interval = 1;
    snapshot.last_present_result = 0;
    snapshot.ui_overlay_width = 800;
    snapshot.ui_overlay_height = 600;
    snapshot.ui_overlay_dirty = 1;
    snapshot.ui_overlay_rows_last = 9;
    snapshot.ui_overlay_quads_last = 18;
    snapshot.ui_overlay_vertices_last = 72;

    telemetry = banana_dx12_diagnostics_runtime_telemetry(&snapshot);
    BANANA_TEST_ASSERT_TRUE(strstr(telemetry, "draws_frame=77") != NULL,
                            "runtime telemetry must include per-frame draw calls");
    BANANA_TEST_ASSERT_TRUE(strstr(telemetry, "ui=800x600") != NULL,
                            "runtime telemetry must include UI overlay dimensions");
    BANANA_TEST_ASSERT_TRUE(strstr(telemetry, "dirty=1") != NULL,
                            "runtime telemetry must include UI dirty flag");
    BANANA_TEST_ASSERT_TRUE(strstr(telemetry, "rows=9") != NULL,
                            "runtime telemetry must include UI row count");

    telemetry = banana_dx12_diagnostics_runtime_telemetry(NULL);
    BANANA_TEST_ASSERT_TRUE(strcmp(telemetry, unavailable) == 0,
                            "null snapshot telemetry must return unavailable summary");
}

static void test_dx12_runtime_telemetry_falls_back_for_null_status(void **state)
{
    (void)state;
    BananaDx12DiagnosticsSnapshot snapshot;
    const char *telemetry = NULL;

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.status = NULL;
    snapshot.active = 0;
    snapshot.width = 320;
    snapshot.height = 200;
    snapshot.present_interval = 2;

    telemetry = banana_dx12_diagnostics_runtime_telemetry(&snapshot);
    BANANA_TEST_ASSERT_TRUE(strstr(telemetry, "status=unknown") != NULL,
                            "runtime telemetry must fall back to an unknown status label");
    BANANA_TEST_ASSERT_TRUE(strstr(telemetry, "size=320x200") != NULL,
                            "runtime telemetry must still include snapshot dimensions");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_dx12_probe_state_and_status_defaults),
    BANANA_TEST_CASE(test_dx12_runtime_telemetry_formats_snapshot_fields),
    BANANA_TEST_CASE(test_dx12_runtime_telemetry_falls_back_for_null_status)
)
