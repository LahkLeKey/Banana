import { useEffect, useState } from "react";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import {
    fetchApiHealth,
    fetchApiRoot,
    fetchTelemetryEvents,
    fetchTelemetryConfig,
    ingestTelemetryEvent,
    resolveApiBaseResolution,
} from "../lib/api";
import { recordTelemetryEvent, useTelemetrySnapshot } from "../lib/telemetryStore";
import type { TelemetryEvent } from "../lib/telemetryStore";

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
            className={`rounded-full border px-2 py-1 text-xs font-medium ${ok
                ? "border-green-500 text-green-600 dark:text-green-400"
                : "border-red-500 text-red-600 dark:text-red-400"
                }`}
        >
            {label}
        </span>
    );
}

type HydrationState = {
    status: "idle" | "loading" | "ok" | "error";
    message: string;
};

export function TelemetryDashboardPage({ autoHydrate = true }: { autoHydrate?: boolean }) {
    const snapshot = useTelemetrySnapshot();
    const [remoteEvents, setRemoteEvents] = useState<TelemetryEvent[] | null>(null);
  const [remoteUpdatedAt, setRemoteUpdatedAt] = useState<number | null>(null);
    const [hydration, setHydration] = useState<HydrationState>({
        status: "idle",
        message: "Waiting for API telemetry probe.",
    });

    useEffect(() => {
        if (!autoHydrate) {
            return;
        }

        const resolution = resolveApiBaseResolution();
        if (resolution.error || resolution.baseUrl.trim().length === 0) {
            setHydration({
                status: "error",
                message: resolution.error ?? "Missing API base URL for telemetry hydration.",
            });
            return;
        }

        let cancelled = false;
        let inFlight = false;

        const hydrateFromApi = async () => {
          if (cancelled || inFlight) {
                return;
            }
          inFlight = true;

            setHydration({
                status: "loading",
                message: `Probing ${resolution.baseUrl} for runtime telemetry inputs...`,
            });

            const emitTelemetry = (event: TelemetryEvent) => {
                recordTelemetryEvent(event);
                void ingestTelemetryEvent(resolution.baseUrl, {
                    source: event.source,
                    event: event.event,
                    timestamp: event.timestamp,
                    status: event.status,
                    durationMs: event.durationMs,
                    code: event.code,
                    channel: event.channel,
                    variant: event.variant,
                    layer: event.layer,
                    details: event.details,
                  }).catch(() => {
                    // Best-effort client telemetry ingest; dashboard still renders local/remote snapshots.
                  });
            };

            emitTelemetry({
                source: "frontend",
                event: "telemetry.dashboard.hydration.started",
                timestamp: Date.now(),
                status: "info",
                details: {
                    source: resolution.source,
                    base_url: resolution.baseUrl,
                },
            });

            const [rootResult, healthResult, configResult] = await Promise.allSettled([
                fetchApiRoot(resolution.baseUrl),
                fetchApiHealth(resolution.baseUrl),
                fetchTelemetryConfig(resolution.baseUrl),
            ]);

            if (cancelled) {
                return;
            }

            if (rootResult.status === "fulfilled") {
                emitTelemetry({
                    source: "runtime",
                    event: "runtime.service.root.ok",
                    timestamp: Date.now(),
                    status: "ok",
                    details: {
                        service: rootResult.value.service ?? "unknown",
                        status: rootResult.value.status ?? "unknown",
                    },
                });
            } else {
                emitTelemetry({
                    source: "runtime",
                    event: "runtime.service.root.error",
                    timestamp: Date.now(),
                    status: "error",
                    details: {
                        message: rootResult.reason instanceof Error
                            ? rootResult.reason.message
                            : String(rootResult.reason),
                    },
                });
            }

            if (healthResult.status === "fulfilled") {
                emitTelemetry({
                    source: "api",
                    event: "api.health.check.ok",
                    timestamp: Date.now(),
                    status: "ok",
                    details: {
                        status: healthResult.value.status,
                    },
                });
            } else {
                emitTelemetry({
                    source: "api",
                    event: "api.health.check.error",
                    timestamp: Date.now(),
                    status: "error",
                    details: {
                        message: healthResult.reason instanceof Error
                            ? healthResult.reason.message
                            : String(healthResult.reason),
                    },
                });
            }

            if (configResult.status === "fulfilled") {
                emitTelemetry({
                    source: "api",
                    event: "api.telemetry.config.loaded",
                    timestamp: Date.now(),
                    status: "ok",
                    details: {
                        sample_rate: configResult.value.sample_rate,
                        unit: configResult.value.unit,
                    },
                });
            } else {
                emitTelemetry({
                    source: "api",
                    event: "api.telemetry.config.error",
                    timestamp: Date.now(),
                    status: "error",
                    details: {
                        message: configResult.reason instanceof Error
                            ? configResult.reason.message
                            : String(configResult.reason),
                    },
                });
            }

            const eventsResult = await fetchTelemetryEvents(resolution.baseUrl, 500).catch(() => null);
            if (eventsResult) {
                const hydratedEvents: TelemetryEvent[] = eventsResult.events.map((event) => ({
                    source: event.source as TelemetryEvent["source"],
                    event: event.event,
                    timestamp: event.timestamp,
                    status: event.status as TelemetryEvent["status"],
                    durationMs: event.durationMs,
                    code: event.code,
                    channel: event.channel,
                    variant: event.variant as TelemetryEvent["variant"],
                    layer: event.layer as TelemetryEvent["layer"],
                    details: event.details,
                }));
                setRemoteEvents(hydratedEvents);
                  setRemoteUpdatedAt(Date.now());
            }

            const successCount = [rootResult, healthResult, configResult].filter((r) => r.status === "fulfilled").length;
            if (successCount > 0) {
                setHydration({
                    status: "ok",
                    message: `API telemetry probe succeeded (${successCount}/3 checks) via ${resolution.source}. Stored feed active.`,
                });
            } else {
                setHydration({
                    status: "error",
                    message: "API telemetry probe failed for all checks. Inspect runtime/API health and base URL.",
                });
            }

                inFlight = false;
        };

        void hydrateFromApi();
        const interval = setInterval(() => {
            void hydrateFromApi();
        }, 30000);

        return () => {
            cancelled = true;
            clearInterval(interval);
        };
    }, [autoHydrate]);

    const telemetryEvents = remoteEvents && remoteEvents.length > 0 ? remoteEvents : snapshot.events;
    const usingRemoteEvents = !!remoteEvents && remoteEvents.length > 0;
    const runtimeEvents = telemetryEvents.filter((event) => event.source === "runtime");
    const apiEvents = telemetryEvents.filter((event) => event.source === "api");
    const frontendEvents = telemetryEvents.filter((event) => event.source === "frontend");
    const wasmEvents = telemetryEvents.filter((event) => event.source === "wasm-worker");
    const nativeEvents = telemetryEvents.filter((event) => event.source === "native");

    const wasmCallStarts = wasmEvents.filter((event) => event.event === "wasm.worker.call.started");
    const wasmAttemptIds = new Set(
        wasmCallStarts
            .map((event) => event.details?.id)
            .filter((id): id is string => typeof id === "string" && id.length > 0)
    );
    const wasmFallbackIds = new Set(
        wasmEvents
            .filter((event) => event.event === "wasm.worker.fallback.triggered")
            .map((event) => event.details?.id)
            .filter((id): id is string => typeof id === "string" && id.length > 0)
    );
    const wasmTimeoutIds = new Set(
        wasmEvents
            .filter(
                (event) =>
                    event.code === 1001 ||
                    (event.details?.reason ?? "") === "wasm_timeout" ||
                    (event.event === "wasm.worker.call.error" && (event.details?.message ?? "") === "execution_timeout")
            )
            .map((event) => event.details?.id)
            .filter((id): id is string => typeof id === "string" && id.length > 0)
    );
    const wasmInitAttempts = wasmEvents.filter((event) => event.event.includes("init.")).length;
    const wasmInitFailures = wasmEvents.filter((event) => event.event.includes("init.failed")).length;
    const wasmDurations = wasmEvents
        .filter((event) => event.event === "wasm.worker.call.completed")
        .map((event) => event.durationMs)
        .filter((duration): duration is number => typeof duration === "number");

    const wasmAttemptCount = wasmAttemptIds.size;

    const initSuccessRate =
        wasmInitAttempts > 0 ? percent(wasmInitAttempts - wasmInitFailures, wasmInitAttempts) : 0;
    const fallbackRate = wasmAttemptCount > 0 ? percent(wasmFallbackIds.size, wasmAttemptCount) : 0;
    const timeoutRate = wasmAttemptCount > 0 ? percent(wasmTimeoutIds.size, wasmAttemptCount) : 0;
    const wasmP95 = p95(wasmDurations);

    const readinessChecks = [
        {
            label: "Init success >= 98%",
            ok: wasmInitAttempts > 0 ? initSuccessRate >= 98 : false,
        },
        {
            label: "Fallback <= 5%",
            ok: wasmAttemptCount > 0 ? fallbackRate <= 5 : false,
        },
        {
            label: "Timeout <= 2%",
            ok: wasmAttemptCount > 0 ? timeoutRate <= 2 : false,
        },
    ];

    const readyCount = readinessChecks.filter((check) => check.ok).length;
    const totalChecks = readinessChecks.length;

    const nativeLayerBreakdown = {
        wrapper: nativeEvents.filter((event) => event.layer === "wrapper").length,
        core: nativeEvents.filter((event) => event.layer === "core").length,
        dal: nativeEvents.filter((event) => event.layer === "dal").length,
    };
    const lastUpdatedMs = usingRemoteEvents ? (remoteUpdatedAt ?? snapshot.updatedAt) : snapshot.updatedAt;

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
                <CardContent>
                    <p className="text-xs text-muted-foreground" data-testid="telemetry-api-hydration-status">
                        API feed: {hydration.message}
                    </p>
                </CardContent>
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
                            <p className="text-xs text-muted-foreground">P95 Call Duration</p>
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
                Last updated: {new Date(lastUpdatedMs).toISOString()}
            </p>
        </div>
    );
}
