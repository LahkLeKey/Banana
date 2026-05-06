import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { useTelemetrySnapshot } from "../lib/telemetryStore";

function percent(part: number, total: number): number {
  if (total <= 0) return 0;
  return (part / total) * 100;
}

function p95(values: number[]): number {
  if (values.length === 0) return 0;
  const sorted = [...values].sort((a, b) => a - b);
  const idx = Math.min(sorted.length - 1, Math.ceil(0.95 * sorted.length) - 1);
  return sorted[idx] ?? 0;
}

function formatPct(value: number): string {
  return `${value.toFixed(1)}%`;
}

function ReadinessBadge({ ok, label }: { ok: boolean; label: string }) {
  return (
    <span
      className={`rounded-full border px-2 py-1 text-xs font-medium ${
        ok
          ? "border-green-500 text-green-600 dark:text-green-400"
          : "border-red-500 text-red-600 dark:text-red-400"
      }`}
    >
      {label}
    </span>
  );
}

export function TelemetryDashboardPage() {
  const snapshot = useTelemetrySnapshot();
  const runtimeEvents = snapshot.events.filter((event) => event.source === "runtime");
  const apiEvents = snapshot.events.filter((event) => event.source === "api");
  const frontendEvents = snapshot.events.filter((event) => event.source === "frontend");
  const wasmEvents = snapshot.events.filter((event) => event.source === "wasm-worker");
  const nativeEvents = snapshot.events.filter((event) => event.source === "native");

  const wasmCalls = wasmEvents.filter((event) => event.event.includes("call."));
  const wasmFallbacks = wasmEvents.filter((event) => event.status === "fallback");
  const wasmTimeouts = wasmEvents.filter(
    (event) => event.code === 1001 || (event.details?.reason ?? "") === "wasm_timeout"
  );
  const wasmInitAttempts = wasmEvents.filter((event) => event.event.includes("init.")).length;
  const wasmInitFailures = wasmEvents.filter((event) => event.event.includes("init.failed")).length;
  const wasmDurations = wasmEvents
    .map((event) => event.durationMs)
    .filter((duration): duration is number => typeof duration === "number");

  const initSuccessRate =
    wasmInitAttempts > 0 ? percent(wasmInitAttempts - wasmInitFailures, wasmInitAttempts) : 0;
  const fallbackRate = wasmCalls.length > 0 ? percent(wasmFallbacks.length, wasmCalls.length) : 0;
  const timeoutRate = wasmCalls.length > 0 ? percent(wasmTimeouts.length, wasmCalls.length) : 0;
  const wasmP95 = p95(wasmDurations);

  const readinessChecks = [
    {
      label: "Init success >= 98%",
      ok: wasmInitAttempts > 0 ? initSuccessRate >= 98 : false,
    },
    {
      label: "Fallback <= 5%",
      ok: wasmCalls.length > 0 ? fallbackRate <= 5 : false,
    },
    {
      label: "Timeout <= 2%",
      ok: wasmCalls.length > 0 ? timeoutRate <= 2 : false,
    },
  ];

  const readyCount = readinessChecks.filter((check) => check.ok).length;
  const totalChecks = readinessChecks.length;

  const nativeLayerBreakdown = {
    wrapper: nativeEvents.filter((event) => event.layer === "wrapper").length,
    core: nativeEvents.filter((event) => event.layer === "core").length,
    dal: nativeEvents.filter((event) => event.layer === "dal").length,
  };

  return (
    <div className="space-y-6" data-testid="telemetry-dashboard-page">
      <Card>
        <CardHeader>
          <CardTitle data-testid="telemetry-dashboard-title">Telemetry Dashboard</CardTitle>
          <CardDescription>
            Website observability drill-down for runtime, API, frontend, WASM workers, and native
            execution layers.
          </CardDescription>
        </CardHeader>
      </Card>

      <div
        className="grid gap-3 md:grid-cols-2 xl:grid-cols-5"
        data-testid="telemetry-summary-grid"
      >
        <Card>
          <CardHeader className="pb-2">
            <CardTitle className="text-sm">Runtime</CardTitle>
          </CardHeader>
          <CardContent className="text-sm" data-testid="runtime-summary-count">
            {runtimeEvents.length} events
          </CardContent>
        </Card>
        <Card>
          <CardHeader className="pb-2">
            <CardTitle className="text-sm">API</CardTitle>
          </CardHeader>
          <CardContent className="text-sm" data-testid="api-summary-count">
            {apiEvents.length} events
          </CardContent>
        </Card>
        <Card>
          <CardHeader className="pb-2">
            <CardTitle className="text-sm">Frontend</CardTitle>
          </CardHeader>
          <CardContent className="text-sm" data-testid="frontend-summary-count">
            {frontendEvents.length} events
          </CardContent>
        </Card>
        <Card>
          <CardHeader className="pb-2">
            <CardTitle className="text-sm">WASM Worker</CardTitle>
          </CardHeader>
          <CardContent className="text-sm" data-testid="wasm-summary-count">
            {wasmEvents.length} events
          </CardContent>
        </Card>
        <Card>
          <CardHeader className="pb-2">
            <CardTitle className="text-sm">Native</CardTitle>
          </CardHeader>
          <CardContent className="text-sm" data-testid="native-summary-count">
            {nativeEvents.length} events
          </CardContent>
        </Card>
      </div>

      <Card data-testid="wasm-worker-drilldown">
        <CardHeader>
          <CardTitle>WASM Worker Health</CardTitle>
          <CardDescription>Production execution metrics and readiness thresholds.</CardDescription>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="grid gap-3 md:grid-cols-4">
            <div className="rounded border p-3">
              <p className="text-xs text-muted-foreground">Init Success</p>
              <p className="text-lg font-semibold" data-testid="wasm-init-success-rate">
                {formatPct(initSuccessRate)}
              </p>
            </div>
            <div className="rounded border p-3">
              <p className="text-xs text-muted-foreground">Fallback Rate</p>
              <p className="text-lg font-semibold" data-testid="wasm-fallback-rate">
                {formatPct(fallbackRate)}
              </p>
            </div>
            <div className="rounded border p-3">
              <p className="text-xs text-muted-foreground">Timeout Rate</p>
              <p className="text-lg font-semibold" data-testid="wasm-timeout-rate">
                {formatPct(timeoutRate)}
              </p>
            </div>
            <div className="rounded border p-3">
              <p className="text-xs text-muted-foreground">P95 Duration</p>
              <p className="text-lg font-semibold" data-testid="wasm-p95-duration">
                {Math.round(wasmP95)} ms
              </p>
            </div>
          </div>

          <div className="flex flex-wrap gap-2" data-testid="telemetry-readiness-badges">
            {readinessChecks.map((check) => (
              <ReadinessBadge key={check.label} ok={check.ok} label={check.label} />
            ))}
            <span
              className="rounded-full border px-2 py-1 text-xs"
              data-testid="telemetry-readiness-score"
            >
              {readyCount}/{totalChecks} checks passing
            </span>
          </div>
        </CardContent>
      </Card>

      <Card data-testid="native-drilldown">
        <CardHeader>
          <CardTitle>Native Observability</CardTitle>
          <CardDescription>
            Wrapper/core/DAL status and latency distribution for cross-layer incident analysis.
          </CardDescription>
        </CardHeader>
        <CardContent>
          {nativeEvents.length === 0 ? (
            <p className="text-sm text-muted-foreground" data-testid="native-unavailable">
              Native observability data unavailable for this interval.
            </p>
          ) : (
            <div className="grid gap-3 md:grid-cols-3">
              <div className="rounded border p-3" data-testid="native-wrapper-count">
                <p className="text-xs text-muted-foreground">Wrapper Events</p>
                <p className="text-lg font-semibold">{nativeLayerBreakdown.wrapper}</p>
              </div>
              <div className="rounded border p-3" data-testid="native-core-count">
                <p className="text-xs text-muted-foreground">Core Events</p>
                <p className="text-lg font-semibold">{nativeLayerBreakdown.core}</p>
              </div>
              <div className="rounded border p-3" data-testid="native-dal-count">
                <p className="text-xs text-muted-foreground">DAL Events</p>
                <p className="text-lg font-semibold">{nativeLayerBreakdown.dal}</p>
              </div>
            </div>
          )}
        </CardContent>
      </Card>

      <p className="text-xs text-muted-foreground" data-testid="telemetry-last-updated">
        Last updated: {new Date(snapshot.updatedAt).toISOString()}
      </p>
    </div>
  );
}
