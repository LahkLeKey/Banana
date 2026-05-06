import { useEffect, useMemo, useState } from "react";
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

const DASHBOARD_SOURCES: TelemetryEvent["source"][] = ["runtime", "api", "frontend", "wasm-worker", "native"];

const RANGE_PRESETS = {
    "15m": 15 * 60 * 1000,
    "60m": 60 * 60 * 1000,
    "6h": 6 * 60 * 60 * 1000,
    "24h": 24 * 60 * 60 * 1000,
} as const;

type RangePreset = keyof typeof RANGE_PRESETS;
type EventStatusFilter = "all" | TelemetryEvent["status"];

type Bucket = {
    label: string;
    total: number;
    errors: number;
    bySource: Record<TelemetryEvent["source"], number>;
};

const SOURCE_STYLES: Record<TelemetryEvent["source"], { label: string; bar: string; chip: string }> = {
    runtime: {
        label: "Runtime",
        bar: "bg-cyan-500",
        chip: "border-cyan-300 bg-cyan-50 text-cyan-700 dark:border-cyan-800 dark:bg-cyan-950/40 dark:text-cyan-200",
    },
    api: {
        label: "API",
        bar: "bg-violet-500",
        chip: "border-violet-300 bg-violet-50 text-violet-700 dark:border-violet-800 dark:bg-violet-950/40 dark:text-violet-200",
    },
    frontend: {
        label: "Frontend",
        bar: "bg-emerald-500",
        chip: "border-emerald-300 bg-emerald-50 text-emerald-700 dark:border-emerald-800 dark:bg-emerald-950/40 dark:text-emerald-200",
    },
    "wasm-worker": {
        label: "WASM",
        bar: "bg-amber-500",
        chip: "border-amber-300 bg-amber-50 text-amber-700 dark:border-amber-800 dark:bg-amber-950/40 dark:text-amber-200",
    },
    native: {
        label: "Native",
        bar: "bg-fuchsia-500",
        chip: "border-fuchsia-300 bg-fuchsia-50 text-fuchsia-700 dark:border-fuchsia-800 dark:bg-fuchsia-950/40 dark:text-fuchsia-200",
    },
};

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

function percentile(values: number[], ratio: number): number {
    if (values.length === 0) return 0;
    const sorted = [...values].sort((a, b) => a - b);
    const idx = Math.min(sorted.length - 1, Math.ceil(ratio * sorted.length) - 1);
    return sorted[idx] ?? 0;
}

function formatPct(value: number): string {
    return `${value.toFixed(1)}%`;
}

function formatRangeLabel(range: RangePreset): string {
    const value = RANGE_PRESETS[range];
    if (value < 60 * 60 * 1000) {
        return `${Math.round(value / 60000)}m`;
    }

    return `${Math.round(value / (60 * 60 * 1000))}h`;
}

function formatBucketLabel(timestamp: number, range: RangePreset): string {
    const date = new Date(timestamp);
    if (range === "24h") {
        return `${String(date.getHours()).padStart(2, "0")}:00`;
    }

    return `${String(date.getHours()).padStart(2, "0")}:${String(date.getMinutes()).padStart(2, "0")}`;
}

function computeP95FromEvents(events: TelemetryEvent[]): number {
    const durations = events
        .map((event) => event.durationMs)
        .filter((duration): duration is number => typeof duration === "number" && Number.isFinite(duration));
    return p95(durations);
}

function buildBuckets(events: TelemetryEvent[], range: RangePreset, now: number): Bucket[] {
    const rangeMs = RANGE_PRESETS[range];
    const bucketCount = 24;
    const bucketSizeMs = Math.max(1, Math.floor(rangeMs / bucketCount));
    const start = now - rangeMs;

    const buckets: Bucket[] = Array.from({ length: bucketCount }, (_, index) => {
        const timestamp = start + index * bucketSizeMs;
        return {
            label: formatBucketLabel(timestamp, range),
            total: 0,
            errors: 0,
            bySource: {
                runtime: 0,
                api: 0,
                frontend: 0,
                "wasm-worker": 0,
                native: 0,
            },
        };
    });

    for (const event of events) {
        if (event.timestamp < start || event.timestamp > now) {
            continue;
        }

        const rawIndex = Math.floor((event.timestamp - start) / bucketSizeMs);
        const index = Math.min(bucketCount - 1, Math.max(0, rawIndex));
        const bucket = buckets[index];
        if (!bucket) {
            continue;
        }

        bucket.total += 1;
        if (event.status === "error") {
            bucket.errors += 1;
        }
        bucket.bySource[event.source] += 1;
    }

    return buckets;
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
    const [rangePreset, setRangePreset] = useState<RangePreset>("60m");
    const [sourceFilter, setSourceFilter] = useState<TelemetryEvent["source"] | "all">("all");
    const [statusFilter, setStatusFilter] = useState<EventStatusFilter>("all");
    const [eventQuery, setEventQuery] = useState("");
    const [visibleTimelineSources, setVisibleTimelineSources] = useState<Record<TelemetryEvent["source"], boolean>>({
        runtime: true,
        api: true,
        frontend: true,
        "wasm-worker": true,
        native: true,
    });
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

            try {
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
            } catch (error) {
                if (!cancelled) {
                    setHydration({
                        status: "error",
                        message: error instanceof Error
                            ? `Telemetry hydration failed: ${error.message}`
                            : "Telemetry hydration failed due to an unexpected error.",
                    });
                }
            } finally {
                inFlight = false;
            }
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
    const now = Date.now();
    const rangeStart = now - RANGE_PRESETS[rangePreset];

    const filteredEvents = useMemo(() => {
        const query = eventQuery.trim().toLowerCase();
        return telemetryEvents.filter((event) => {
            if (event.timestamp < rangeStart || event.timestamp > now) {
                return false;
            }

            if (sourceFilter !== "all" && event.source !== sourceFilter) {
                return false;
            }

            if (statusFilter !== "all" && event.status !== statusFilter) {
                return false;
            }

            if (query.length === 0) {
                return true;
            }

            const detailsText = JSON.stringify(event.details ?? {}).toLowerCase();
            return event.event.toLowerCase().includes(query) || detailsText.includes(query);
        });
    }, [telemetryEvents, sourceFilter, statusFilter, eventQuery, rangeStart, now]);

    const sourceBreakdown = useMemo(() => {
        const counts: Record<TelemetryEvent["source"], number> = {
            runtime: 0,
            api: 0,
            frontend: 0,
            "wasm-worker": 0,
            native: 0,
        };

        for (const event of filteredEvents) {
            counts[event.source] += 1;
        }

        return counts;
    }, [filteredEvents]);

    const timelineBuckets = useMemo(() => buildBuckets(filteredEvents, rangePreset, now), [filteredEvents, rangePreset, now]);

    const activeTimelineSources = useMemo(() => {
        const selected = DASHBOARD_SOURCES.filter((source) => visibleTimelineSources[source]);
        return selected.length > 0 ? selected : DASHBOARD_SOURCES;
    }, [visibleTimelineSources]);

    const maxVisibleSourceBucketCount = useMemo(() => {
        return Math.max(
            1,
            ...timelineBuckets.map((bucket) =>
                Math.max(...activeTimelineSources.map((source) => bucket.bySource[source]))
            )
        );
    }, [timelineBuckets, activeTimelineSources]);

    const topEventSignatures = useMemo(() => {
        const map = new Map<string, { count: number; lastTimestamp: number; errors: number }>();
        for (const event of filteredEvents) {
            const existing = map.get(event.event);
            if (!existing) {
                map.set(event.event, {
                    count: 1,
                    lastTimestamp: event.timestamp,
                    errors: event.status === "error" ? 1 : 0,
                });
                continue;
            }

            existing.count += 1;
            existing.lastTimestamp = Math.max(existing.lastTimestamp, event.timestamp);
            if (event.status === "error") {
                existing.errors += 1;
            }
        }

        return Array.from(map.entries())
            .map(([eventName, summary]) => ({ eventName, ...summary }))
            .sort((a, b) => b.count - a.count)
            .slice(0, 8);
    }, [filteredEvents]);

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

    const filteredErrorCount = filteredEvents.filter((event) => event.status === "error").length;
    const filteredErrorRate = percent(filteredErrorCount, filteredEvents.length);
    const filteredP95 = computeP95FromEvents(filteredEvents);
    const activeSignals = new Set(filteredEvents.map((event) => event.event)).size;
    const latestEvents = [...filteredEvents]
        .sort((a, b) => b.timestamp - a.timestamp)
        .slice(0, 20);

    const wasmLatencyPercentiles = useMemo(() => {
        const durations = filteredEvents
            .filter((event) => event.source === "wasm-worker" && typeof event.durationMs === "number")
            .map((event) => event.durationMs as number);

        return {
            p50: percentile(durations, 0.5),
            p95: percentile(durations, 0.95),
            p99: percentile(durations, 0.99),
        };
    }, [filteredEvents]);

    const nativeLatencyPercentiles = useMemo(() => {
        const durations = filteredEvents
            .filter((event) => event.source === "native" && typeof event.durationMs === "number")
            .map((event) => event.durationMs as number);

        return {
            p50: percentile(durations, 0.5),
            p95: percentile(durations, 0.95),
            p99: percentile(durations, 0.99),
        };
    }, [filteredEvents]);

    const errorFingerprints = useMemo(() => {
        const groups = new Map<string, {
            source: TelemetryEvent["source"];
            event: string;
            message: string;
            code: string;
            count: number;
            lastSeen: number;
        }>();

        for (const event of filteredEvents) {
            if (event.status !== "error") {
                continue;
            }

            const message = typeof event.details?.message === "string"
                ? event.details.message
                : typeof event.details?.reason === "string"
                    ? event.details.reason
                    : "n/a";
            const code = typeof event.code === "number" ? String(event.code) : "-";
            const key = `${event.source}|${event.event}|${code}|${message}`;

            const existing = groups.get(key);
            if (!existing) {
                groups.set(key, {
                    source: event.source,
                    event: event.event,
                    message,
                    code,
                    count: 1,
                    lastSeen: event.timestamp,
                });
                continue;
            }

            existing.count += 1;
            existing.lastSeen = Math.max(existing.lastSeen, event.timestamp);
        }

        return Array.from(groups.values())
            .sort((a, b) => b.count - a.count || b.lastSeen - a.lastSeen)
            .slice(0, 12);
    }, [filteredEvents]);

    const lastUpdatedMs = usingRemoteEvents ? (remoteUpdatedAt ?? snapshot.updatedAt) : snapshot.updatedAt;

    return (
        <div className="space-y-6" data-testid="telemetry-dashboard-page">
            <Card className="overflow-hidden border-slate-300/80 bg-gradient-to-br from-slate-100 via-sky-50 to-teal-50 shadow-sm dark:from-slate-950 dark:via-slate-900 dark:to-slate-950">
                <CardHeader className="gap-4">
                    <div className="flex flex-wrap items-start justify-between gap-3">
                        <div>
                            <CardTitle data-testid="telemetry-dashboard-title">Telemetry Dashboard</CardTitle>
                            <CardDescription>
                                Grafana-style observability drill-down for runtime, API, frontend, WASM workers, and native layers.
                            </CardDescription>
                        </div>
                        <div className="rounded-lg border border-slate-300/70 bg-white/70 px-3 py-2 text-xs text-slate-700 shadow-sm dark:border-slate-700 dark:bg-slate-900/60 dark:text-slate-200">
                            Window: {formatRangeLabel(rangePreset)}
                        </div>
                    </div>

                    <div className="grid gap-3 md:grid-cols-2 xl:grid-cols-4">
                        <label className="space-y-1 text-xs">
                            <span className="font-medium text-slate-600 dark:text-slate-300">Source</span>
                            <select
                                className="h-9 w-full rounded-md border border-slate-300 bg-white px-2 text-sm dark:border-slate-700 dark:bg-slate-950"
                                value={sourceFilter}
                                onChange={(event) => setSourceFilter(event.target.value as TelemetryEvent["source"] | "all")}
                            >
                                <option value="all">All sources</option>
                                {DASHBOARD_SOURCES.map((source) => (
                                    <option key={source} value={source}>{source}</option>
                                ))}
                            </select>
                        </label>

                        <label className="space-y-1 text-xs">
                            <span className="font-medium text-slate-600 dark:text-slate-300">Status</span>
                            <select
                                className="h-9 w-full rounded-md border border-slate-300 bg-white px-2 text-sm dark:border-slate-700 dark:bg-slate-950"
                                value={statusFilter}
                                onChange={(event) => setStatusFilter(event.target.value as EventStatusFilter)}
                            >
                                <option value="all">All statuses</option>
                                <option value="ok">ok</option>
                                <option value="info">info</option>
                                <option value="error">error</option>
                            </select>
                        </label>

                        <label className="space-y-1 text-xs">
                            <span className="font-medium text-slate-600 dark:text-slate-300">Window</span>
                            <select
                                className="h-9 w-full rounded-md border border-slate-300 bg-white px-2 text-sm dark:border-slate-700 dark:bg-slate-950"
                                value={rangePreset}
                                onChange={(event) => setRangePreset(event.target.value as RangePreset)}
                            >
                                <option value="15m">Last 15m</option>
                                <option value="60m">Last 60m</option>
                                <option value="6h">Last 6h</option>
                                <option value="24h">Last 24h</option>
                            </select>
                        </label>

                        <label className="space-y-1 text-xs">
                            <span className="font-medium text-slate-600 dark:text-slate-300">Find event</span>
                            <input
                                className="h-9 w-full rounded-md border border-slate-300 bg-white px-2 text-sm placeholder:text-slate-400 dark:border-slate-700 dark:bg-slate-950"
                                value={eventQuery}
                                onChange={(event) => setEventQuery(event.target.value)}
                                placeholder="wasm.worker.call..."
                            />
                        </label>
                    </div>

                    <div className="grid gap-3 md:grid-cols-2 xl:grid-cols-4">
                        <div className="rounded-lg border border-cyan-300/60 bg-cyan-50/80 p-3 dark:border-cyan-900/70 dark:bg-cyan-950/30">
                            <p className="text-xs text-cyan-800 dark:text-cyan-200">Visible Events</p>
                            <p className="text-2xl font-semibold text-cyan-950 dark:text-cyan-50">{filteredEvents.length}</p>
                        </div>
                        <div className="rounded-lg border border-rose-300/60 bg-rose-50/80 p-3 dark:border-rose-900/70 dark:bg-rose-950/30">
                            <p className="text-xs text-rose-800 dark:text-rose-200">Error Rate</p>
                            <p className="text-2xl font-semibold text-rose-900 dark:text-rose-100">{formatPct(filteredErrorRate)}</p>
                        </div>
                        <div className="rounded-lg border border-indigo-300/60 bg-indigo-50/80 p-3 dark:border-indigo-900/70 dark:bg-indigo-950/30">
                            <p className="text-xs text-indigo-800 dark:text-indigo-200">P95 Duration</p>
                            <p className="text-2xl font-semibold text-indigo-950 dark:text-indigo-50">{Math.round(filteredP95)} ms</p>
                        </div>
                        <div className="rounded-lg border border-emerald-300/60 bg-emerald-50/80 p-3 dark:border-emerald-900/70 dark:bg-emerald-950/30">
                            <p className="text-xs text-emerald-800 dark:text-emerald-200">Active Signals</p>
                            <p className="text-2xl font-semibold text-emerald-950 dark:text-emerald-50">{activeSignals}</p>
                        </div>
                    </div>

                    <p className="text-xs text-muted-foreground" data-testid="telemetry-api-hydration-status">
                        API feed: {hydration.message}
                    </p>
                </CardHeader>
            </Card>

            <div className="grid gap-4 xl:grid-cols-[2fr,1fr]">
                <Card>
                    <CardHeader>
                        <CardTitle className="text-base">Traffic Timeline</CardTitle>
                        <CardDescription>Layered traffic bars with per-source toggles and error overlays.</CardDescription>
                    </CardHeader>
                    <CardContent className="space-y-4">
                        <div className="flex flex-wrap gap-2">
                            {DASHBOARD_SOURCES.map((source) => {
                                const selected = visibleTimelineSources[source];
                                const styles = SOURCE_STYLES[source];
                                return (
                                    <button
                                        key={source}
                                        type="button"
                                        className={`rounded-full border px-2 py-1 text-[11px] transition ${selected
                                            ? styles.chip
                                            : "border-slate-300 text-slate-500 dark:border-slate-700 dark:text-slate-400"
                                            }`}
                                        onClick={() => {
                                            setVisibleTimelineSources((current) => ({
                                                ...current,
                                                [source]: !current[source],
                                            }));
                                        }}
                                    >
                                        {styles.label}
                                    </button>
                                );
                            })}
                        </div>

                        <div className="h-40">
                            <div className="flex h-full items-end gap-1">
                                {timelineBuckets.map((bucket, index) => {
                                    const maxCount = Math.max(1, ...timelineBuckets.map((entry) => entry.total));
                                    const totalHeight = Math.max(6, Math.round((bucket.total / maxCount) * 140));
                                    const errorHeight = bucket.total > 0
                                        ? Math.max(2, Math.round((bucket.errors / Math.max(1, bucket.total)) * totalHeight))
                                        : 0;
                                    return (
                                        <div key={`${bucket.label}-${index}`} className="group relative flex h-full flex-1 items-end">
                                            <div
                                                className="w-full rounded-t bg-slate-300/60 transition-colors dark:bg-slate-700/70"
                                                style={{ height: `${totalHeight}px` }}
                                            />
                                            <div className="absolute bottom-0 flex w-full items-end gap-px px-[1px]">
                                                {activeTimelineSources.map((source) => {
                                                    const layerCount = bucket.bySource[source];
                                                    const layerHeight = layerCount > 0
                                                        ? Math.max(2, Math.round((layerCount / maxVisibleSourceBucketCount) * 130))
                                                        : 0;
                                                    return (
                                                        <div
                                                            key={source}
                                                            className={`flex-1 rounded-t ${SOURCE_STYLES[source].bar}`}
                                                            style={{ height: `${layerHeight}px` }}
                                                        />
                                                    );
                                                })}
                                            </div>
                                            {errorHeight > 0 ? (
                                                <div
                                                    className="absolute bottom-0 w-full rounded-t bg-rose-500/80"
                                                    style={{ height: `${errorHeight}px` }}
                                                />
                                            ) : null}
                                            <div className="pointer-events-none absolute -top-8 left-1/2 hidden -translate-x-1/2 whitespace-nowrap rounded bg-slate-900 px-2 py-1 text-[10px] text-slate-50 group-hover:block">
                                                {bucket.label} • {bucket.total} events
                                            </div>
                                        </div>
                                    );
                                })}
                            </div>
                        </div>
                        <div className="flex justify-between text-[10px] text-muted-foreground">
                            <span>{timelineBuckets[0]?.label ?? ""}</span>
                            <span>{timelineBuckets[timelineBuckets.length - 1]?.label ?? ""}</span>
                        </div>
                    </CardContent>
                </Card>

                <Card>
                    <CardHeader>
                        <CardTitle className="text-base">Source Mix</CardTitle>
                        <CardDescription>Share of event traffic by layer.</CardDescription>
                    </CardHeader>
                    <CardContent className="space-y-3">
                        {DASHBOARD_SOURCES.map((source) => {
                            const count = sourceBreakdown[source];
                            const width = Math.max(0, Math.min(100, percent(count, Math.max(1, filteredEvents.length))));
                            return (
                                <div key={source} className="space-y-1">
                                    <div className="flex items-center justify-between text-xs">
                                        <span className="font-medium">{source}</span>
                                        <span>{count}</span>
                                    </div>
                                    <div className="h-2 rounded bg-slate-200 dark:bg-slate-800">
                                        <div
                                            className="h-2 rounded bg-gradient-to-r from-cyan-500 to-blue-500"
                                            style={{ width: `${width}%` }}
                                        />
                                    </div>
                                </div>
                            );
                        })}
                    </CardContent>
                </Card>
            </div>

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

            <div className="grid gap-4 xl:grid-cols-[1.4fr,1fr]">
                <Card>
                    <CardHeader>
                        <CardTitle className="text-base">Top Signals</CardTitle>
                        <CardDescription>Most frequent event signatures in the selected window.</CardDescription>
                    </CardHeader>
                    <CardContent>
                        {topEventSignatures.length === 0 ? (
                            <p className="text-sm text-muted-foreground">No events match current filters.</p>
                        ) : (
                            <div className="overflow-x-auto">
                                <table className="w-full text-left text-xs">
                                    <thead>
                                        <tr className="border-b text-muted-foreground">
                                            <th className="py-2 pr-2 font-medium">Event</th>
                                            <th className="py-2 pr-2 font-medium">Count</th>
                                            <th className="py-2 pr-2 font-medium">Errors</th>
                                            <th className="py-2 font-medium">Last Seen</th>
                                        </tr>
                                    </thead>
                                    <tbody>
                                        {topEventSignatures.map((signal) => (
                                            <tr key={signal.eventName} className="border-b last:border-b-0">
                                                <td className="py-2 pr-2 font-mono text-[11px]">{signal.eventName}</td>
                                                <td className="py-2 pr-2">{signal.count}</td>
                                                <td className="py-2 pr-2">{signal.errors}</td>
                                                <td className="py-2">{new Date(signal.lastTimestamp).toLocaleTimeString()}</td>
                                            </tr>
                                        ))}
                                    </tbody>
                                </table>
                            </div>
                        )}
                    </CardContent>
                </Card>

                <Card>
                    <CardHeader>
                        <CardTitle className="text-base">Live Event Stream</CardTitle>
                        <CardDescription>Latest 20 filtered events with payload highlights.</CardDescription>
                    </CardHeader>
                    <CardContent>
                        {latestEvents.length === 0 ? (
                            <p className="text-sm text-muted-foreground">No events for current filters.</p>
                        ) : (
                            <div className="max-h-72 space-y-2 overflow-auto pr-1">
                                {latestEvents.map((event, index) => (
                                    <div
                                        key={`${event.event}-${event.timestamp}-${index}`}
                                        className="rounded border border-slate-200 bg-slate-50/60 p-2 dark:border-slate-700 dark:bg-slate-900/60"
                                    >
                                        <div className="flex items-center justify-between gap-2 text-[11px]">
                                            <span className="font-mono">{event.event}</span>
                                            <span className={`rounded-full px-2 py-0.5 ${event.status === "error"
                                                ? "bg-rose-100 text-rose-700 dark:bg-rose-900/40 dark:text-rose-200"
                                                : "bg-emerald-100 text-emerald-700 dark:bg-emerald-900/40 dark:text-emerald-200"
                                                }`}>
                                                {event.status}
                                            </span>
                                        </div>
                                        <div className="mt-1 flex flex-wrap gap-x-3 text-[10px] text-muted-foreground">
                                            <span>{new Date(event.timestamp).toLocaleTimeString()}</span>
                                            <span>{event.source}</span>
                                            {typeof event.durationMs === "number" ? <span>{Math.round(event.durationMs)} ms</span> : null}
                                        </div>
                                    </div>
                                ))}
                            </div>
                        )}
                    </CardContent>
                </Card>
            </div>

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Error Explorer</CardTitle>
                    <CardDescription>Grouped fingerprints by source, event, code, and reason/message.</CardDescription>
                </CardHeader>
                <CardContent>
                    {errorFingerprints.length === 0 ? (
                        <p className="text-sm text-muted-foreground">No error fingerprints for current filters.</p>
                    ) : (
                        <div className="overflow-x-auto">
                            <table className="w-full text-left text-xs" data-testid="telemetry-error-explorer">
                                <thead>
                                    <tr className="border-b text-muted-foreground">
                                        <th className="py-2 pr-2 font-medium">Source</th>
                                        <th className="py-2 pr-2 font-medium">Event</th>
                                        <th className="py-2 pr-2 font-medium">Code</th>
                                        <th className="py-2 pr-2 font-medium">Reason</th>
                                        <th className="py-2 pr-2 font-medium">Count</th>
                                        <th className="py-2 font-medium">Last Seen</th>
                                    </tr>
                                </thead>
                                <tbody>
                                    {errorFingerprints.map((fingerprint) => (
                                        <tr
                                            key={`${fingerprint.source}-${fingerprint.event}-${fingerprint.code}-${fingerprint.message}`}
                                            className="border-b last:border-b-0"
                                        >
                                            <td className="py-2 pr-2">{fingerprint.source}</td>
                                            <td className="py-2 pr-2 font-mono text-[11px]">{fingerprint.event}</td>
                                            <td className="py-2 pr-2">{fingerprint.code}</td>
                                            <td className="max-w-[360px] truncate py-2 pr-2" title={fingerprint.message}>{fingerprint.message}</td>
                                            <td className="py-2 pr-2">{fingerprint.count}</td>
                                            <td className="py-2">{new Date(fingerprint.lastSeen).toLocaleTimeString()}</td>
                                        </tr>
                                    ))}
                                </tbody>
                            </table>
                        </div>
                    )}
                </CardContent>
            </Card>

            <Card data-testid="wasm-worker-drilldown">
                <CardHeader>
                    <CardTitle>WASM Worker Health</CardTitle>
                    <CardDescription>
                        Production execution metrics and readiness thresholds.
                    </CardDescription>
                </CardHeader>
                <CardContent>
                    <div className="space-y-4">
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

                        <div className="grid gap-3 md:grid-cols-3" data-testid="wasm-latency-percentiles">
                            <div className="rounded border p-3">
                                <p className="text-xs text-muted-foreground">WASM p50</p>
                                <p className="text-base font-semibold">{Math.round(wasmLatencyPercentiles.p50)} ms</p>
                            </div>
                            <div className="rounded border p-3">
                                <p className="text-xs text-muted-foreground">WASM p95</p>
                                <p className="text-base font-semibold">{Math.round(wasmLatencyPercentiles.p95)} ms</p>
                            </div>
                            <div className="rounded border p-3">
                                <p className="text-xs text-muted-foreground">WASM p99</p>
                                <p className="text-base font-semibold">{Math.round(wasmLatencyPercentiles.p99)} ms</p>
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
                        <div className="space-y-3">
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
                            <div className="grid gap-3 md:grid-cols-3" data-testid="native-latency-percentiles">
                                <div className="rounded border p-3">
                                    <p className="text-xs text-muted-foreground">Native p50</p>
                                    <p className="text-base font-semibold">{Math.round(nativeLatencyPercentiles.p50)} ms</p>
                                </div>
                                <div className="rounded border p-3">
                                    <p className="text-xs text-muted-foreground">Native p95</p>
                                    <p className="text-base font-semibold">{Math.round(nativeLatencyPercentiles.p95)} ms</p>
                                </div>
                                <div className="rounded border p-3">
                                    <p className="text-xs text-muted-foreground">Native p99</p>
                                    <p className="text-base font-semibold">{Math.round(nativeLatencyPercentiles.p99)} ms</p>
                                </div>
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
