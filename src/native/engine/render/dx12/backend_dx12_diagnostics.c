#include "backend_dx12_diagnostics.h"

#include <stdio.h>
#include <string.h>

static BananaDx12DiagnosticsProbeState s_dx12_probe_state = BANANA_DX12_DIAGNOSTICS_PROBE_UNKNOWN;
static const char *s_dx12_probe_status = "dx12-not-probed";
static char s_dx12_runtime_telemetry[256] = "dx12-runtime-inactive";

BananaDx12DiagnosticsProbeState banana_dx12_diagnostics_probe_state(void)
{
    return s_dx12_probe_state;
}

void banana_dx12_diagnostics_set_probe_state(BananaDx12DiagnosticsProbeState state,
                                             const char *status)
{
    s_dx12_probe_state = state;
    s_dx12_probe_status = status ? status : "dx12-status-unknown";
}

const char *banana_dx12_diagnostics_probe_status(void)
{
    return s_dx12_probe_status;
}

const char *banana_dx12_diagnostics_runtime_telemetry(const BananaDx12DiagnosticsSnapshot *snapshot)
{
    if (!snapshot)
    {
        return banana_dx12_diagnostics_runtime_unavailable_telemetry();
    }

    snprintf(s_dx12_runtime_telemetry,
             sizeof(s_dx12_runtime_telemetry),
             "status=%s active=%d size=%dx%d frame=%llu presented=%llu draws_frame=%u draws_total=%llu interval=%u hr=0x%08lx",
             snapshot->status ? snapshot->status : "unknown",
             snapshot->active,
             snapshot->width,
             snapshot->height,
             snapshot->frame_counter,
             snapshot->frames_presented,
             snapshot->scene_draw_calls_frame,
             snapshot->scene_draw_calls_total,
             snapshot->present_interval,
             snapshot->last_present_result);
    snprintf(s_dx12_runtime_telemetry + strlen(s_dx12_runtime_telemetry),
             sizeof(s_dx12_runtime_telemetry) - strlen(s_dx12_runtime_telemetry),
             " ui=%dx%d dirty=%d rows=%u quads=%u verts=%u",
             snapshot->ui_overlay_width,
             snapshot->ui_overlay_height,
             snapshot->ui_overlay_dirty,
             snapshot->ui_overlay_rows_last,
             snapshot->ui_overlay_quads_last,
             snapshot->ui_overlay_vertices_last);
    return s_dx12_runtime_telemetry;
}

const char *banana_dx12_diagnostics_runtime_unavailable_telemetry(void)
{
    return "status=dx12-runtime-unavailable active=0";
}