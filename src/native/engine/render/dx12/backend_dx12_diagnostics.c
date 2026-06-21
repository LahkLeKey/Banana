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
    const char *unavailable_telemetry = banana_dx12_diagnostics_runtime_unavailable_telemetry();
    BananaDx12DiagnosticsSnapshot fallback_snapshot;
    const BananaDx12DiagnosticsSnapshot *effective_snapshot = NULL;

    memset(&fallback_snapshot, 0, sizeof(fallback_snapshot));
    fallback_snapshot.status = "dx12-runtime-unavailable";

    effective_snapshot = snapshot ? snapshot : &fallback_snapshot;

    snprintf(s_dx12_runtime_telemetry,
             sizeof(s_dx12_runtime_telemetry),
             "status=%s active=%d size=%dx%d frame=%llu presented=%llu draws_frame=%u draws_total=%llu interval=%u hr=0x%08lx",
             effective_snapshot->status ? effective_snapshot->status : "unknown",
             effective_snapshot->active,
             effective_snapshot->width,
             effective_snapshot->height,
             effective_snapshot->frame_counter,
             effective_snapshot->frames_presented, effective_snapshot->scene_draw_calls_frame,
             effective_snapshot->scene_draw_calls_total, effective_snapshot->present_interval, effective_snapshot->last_present_result);
    snprintf(s_dx12_runtime_telemetry + strlen(s_dx12_runtime_telemetry),
             sizeof(s_dx12_runtime_telemetry) - strlen(s_dx12_runtime_telemetry),
             " ui=%dx%d dirty=%d rows=%u quads=%u verts=%u",
             effective_snapshot->ui_overlay_width,
             effective_snapshot->ui_overlay_height, effective_snapshot->ui_overlay_dirty,
             effective_snapshot->ui_overlay_rows_last, effective_snapshot->ui_overlay_quads_last,
             effective_snapshot->ui_overlay_vertices_last);
    return snapshot ? s_dx12_runtime_telemetry : unavailable_telemetry;
}

const char *banana_dx12_diagnostics_runtime_unavailable_telemetry(void)
{
    return "status=dx12-runtime-unavailable active=0";
}