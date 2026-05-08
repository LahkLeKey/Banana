/**
 * viewportTelemetry.ts — Structured telemetry emission for viewport lifecycle events.
 *
 * Spec 261 T003/T009: Runtime telemetry event schema. All events are fire-and-forget
 * POSTs to /api/telemetry/frame. Preserves compatibility with existing game-state
 * telemetry endpoint used by prior runtime flows.
 */

const TELEMETRY_ENDPOINT = "/api/telemetry/frame";

export type TelemetryEventKind =
  | "viewport_start"
  | "first_frame"
  | "frame_interval"
  | "overlay_open"
  | "overlay_close"
  | "degraded_entry"
  | "recovery_attempt"
  | "recovery_success"
  | "recovery_failure"
  | "stopped";

export interface RuntimeTelemetryEvent {
  kind: TelemetryEventKind;
  ts: number;
  session_id: string;
  payload: Record<string, number | string | boolean>;
}

interface EmitOptions {
  kind: TelemetryEventKind;
  sessionId: string;
  payload: Record<string, number | string | boolean>;
}

export function emitTelemetry({ kind, sessionId, payload }: EmitOptions): void {
  const event: RuntimeTelemetryEvent = {
    kind,
    ts: Date.now(),
    session_id: sessionId,
    payload,
  };

  void fetch(TELEMETRY_ENDPOINT, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(event),
    keepalive: true,
  }).catch(() => {
    /* telemetry is non-blocking */
  });
}
