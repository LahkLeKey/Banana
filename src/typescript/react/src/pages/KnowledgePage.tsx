import { useEffect, useState } from "react";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { fetchTrainingWorkbenchHistory, resolveApiBaseUrl } from "../lib/api";
import type { TrainingHistoryResponse } from "../lib/api";
import type { TrainingLane, TrainingRunResult } from "@banana/ui";

const ARTICLES = [
    {
        title: "Deployment Readiness Playbook",
        summary: "Release readiness evidence linked to runtime contracts, rollout checks, and post-deploy health thresholds.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/.specify/wiki/human-reference/operations/deployment.md",
    },
    {
        title: "Native Wrapper ABI",
        summary: "Wrapper ABI surface, status codes, buffer ownership, and downstream interop contracts.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/.specify/wiki/human-reference/architecture/native-wrapper-abi.md",
    },
    {
        title: "WSL2 Runtime Channels",
        summary: "Container-driven local runtime: Docker Desktop + Ubuntu WSL2 + Electron/Mobile channels.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/.specify/wiki/human-reference/operations/wsl2-runtime-channels.md",
    },
];

const SPACES = ["Platform Engineering", "Model Runtime", "Delivery Contracts", "Incident Playbooks"];

function laneRuns(rows: TrainingRunResult[], lane: TrainingLane) {
    return rows.filter((r) => r.lane === lane);
}

function metricVal(run: TrainingRunResult, key: string): number | null {
    const v = run.metrics?.[key];
    return typeof v === "number" ? v : null;
}

function passFailBadge(run: TrainingRunResult) {
    if (run.status === "passed") return <span className="rounded-full border border-green-500 px-2 py-0.5 text-xs text-green-600 dark:text-green-400">passed</span>;
    if (run.status === "failed") return <span className="rounded-full border border-red-500 px-2 py-0.5 text-xs text-red-600 dark:text-red-400">failed</span>;
    return <span className="rounded-full border border-yellow-500 px-2 py-0.5 text-xs text-yellow-600">running</span>;
}

function MiniBar({ value, max = 1 }: { value: number; max?: number }) {
    const pct = Math.min(100, Math.round((value / max) * 100));
    return (
        <div className="h-1.5 w-full rounded-full bg-muted overflow-hidden">
            <div className="h-full rounded-full bg-primary" style={{ width: `${pct}%` }} />
        </div>
    );
}

function LaneMetricsCard({ lane, rows }: { lane: TrainingLane; rows: TrainingRunResult[] }) {
    const runs = laneRuns(rows, lane).slice(0, 3);
    if (runs.length === 0) return null;
    return (
        <Card>
            <CardHeader className="pb-2">
                <CardTitle className="text-sm">{lane} — last {runs.length} run{runs.length > 1 ? "s" : ""}</CardTitle>
            </CardHeader>
            <CardContent className="space-y-3">
                {runs.map((run) => {
                    const acc = metricVal(run, "accuracy");
                    const f1  = metricVal(run, "f1");
                    return (
                        <div key={run.run_id} className="space-y-1 border-b pb-2 last:border-0 last:pb-0">
                            <div className="flex items-center justify-between text-xs">
                                <span className="font-mono text-muted-foreground">{run.run_id.slice(-10)}</span>
                                {passFailBadge(run)}
                            </div>
                            {acc != null && (
                                <div className="space-y-0.5">
                                    <div className="flex justify-between text-xs text-muted-foreground">
                                        <span>accuracy</span><span>{(acc * 100).toFixed(1)}%</span>
                                    </div>
                                    <MiniBar value={acc} />
                                </div>
                            )}
                            {f1 != null && (
                                <div className="space-y-0.5">
                                    <div className="flex justify-between text-xs text-muted-foreground">
                                        <span>F1</span><span>{f1.toFixed(3)}</span>
                                    </div>
                                    <MiniBar value={f1} />
                                </div>
                            )}
                        </div>
                    );
                })}
            </CardContent>
        </Card>
    );
}

function cosineDrift(a: number[], b: number[]): number | null {
    if (a.length !== b.length || a.length === 0) return null;
    const dot = a.reduce((s, v, i) => s + v * b[i], 0);
    const na = Math.sqrt(a.reduce((s, v) => s + v * v, 0));
    const nb = Math.sqrt(b.reduce((s, v) => s + v * v, 0));
    if (na === 0 || nb === 0) return null;
    return 1 - dot / (na * nb);
}

function extractEmbeddingFromRun(run: TrainingRunResult): number[] | null {
    const e = run.metrics?.["centroid_embedding"];
    if (Array.isArray(e) && e.every((v) => typeof v === "number")) return e as number[];
    return null;
}

export function KnowledgePage() {
    const [history, setHistory] = useState<TrainingHistoryResponse | null>(null);

    useEffect(() => {
        fetchTrainingWorkbenchHistory(resolveApiBaseUrl())
            .then(setHistory)
            .catch(() => setHistory(null));
    }, []);

    const rows = history?.rows ?? [];
    const recentRuns = rows.slice(0, 4);

    // Spike 169: cosine drift between most recent consecutive runs per lane
    const driftByLane: Record<string, number | null> = {};
    for (const lane of ["left-brain", "right-brain"] as TrainingLane[]) {
        const laneRows = laneRuns(rows, lane);
        if (laneRows.length >= 2) {
            const e0 = extractEmbeddingFromRun(laneRows[0]);
            const e1 = extractEmbeddingFromRun(laneRows[1]);
            driftByLane[lane] = e0 && e1 ? cosineDrift(e0, e1) : null;
        } else {
            driftByLane[lane] = null;
        }
    }

    return (
        <div className="grid gap-4 lg:grid-cols-[200px_1fr_260px]">
            {/* Spaces sidebar */}
            <Card>
                <CardHeader className="pb-2">
                    <CardTitle className="text-sm">Spaces</CardTitle>
                </CardHeader>
                <CardContent className="space-y-2 text-sm">
                    {SPACES.map((s) => (
                        <p key={s} className="text-muted-foreground hover:text-foreground cursor-pointer">{s}</p>
                    ))}
                </CardContent>
            </Card>

            {/* Article canvas + spike 167 metric cards */}
            <div className="space-y-4">
                <Card>
                    <CardHeader>
                        <CardTitle>Knowledge Base</CardTitle>
                        <CardDescription>Domain narratives, ADRs, and runbooks</CardDescription>
                    </CardHeader>
                </Card>
                {ARTICLES.map((a) => (
                    <Card key={a.title}>
                        <CardHeader>
                            <CardTitle className="text-base">
                                <a href={a.href} target="_blank" rel="noreferrer" className="hover:underline">{a.title}</a>
                            </CardTitle>
                            <CardDescription>{a.summary}</CardDescription>
                        </CardHeader>
                    </Card>
                ))}

                {/* Spike 167: per-lane training metric cards */}
                {rows.length > 0 && (
                    <>
                        <p className="text-xs font-semibold uppercase tracking-widest text-muted-foreground px-1">Training Metrics</p>
                        <LaneMetricsCard lane="left-brain" rows={rows} />
                        <LaneMetricsCard lane="right-brain" rows={rows} />
                    </>
                )}
            </div>

            {/* Activity + spike 169 drift */}
            <div className="space-y-3">
                <Card>
                    <CardHeader className="pb-2">
                        <CardTitle className="text-sm">Training Activity</CardTitle>
                    </CardHeader>
                    <CardContent className="space-y-2 text-xs text-muted-foreground">
                        {recentRuns.length > 0 ? recentRuns.map((run: TrainingRunResult) => (
                            <div key={run.run_id} className="border-b pb-1 last:border-0">
                                <p className="font-medium text-foreground">{run.lane}</p>
                                <p>{run.status} · {run.run_id.slice(-8)}</p>
                            </div>
                        )) : (
                            <p>No training runs available</p>
                        )}
                    </CardContent>
                </Card>

                {/* Spike 169: embedding drift delta */}
                <Card>
                    <CardHeader className="pb-2">
                        <CardTitle className="text-sm">Embedding Drift</CardTitle>
                    </CardHeader>
                    <CardContent className="space-y-2 text-xs">
                        {(["left-brain", "right-brain"] as TrainingLane[]).map((lane) => {
                            const drift = driftByLane[lane];
                            return (
                                <div key={lane} className="flex items-center justify-between">
                                    <span className="text-muted-foreground">{lane}</span>
                                    {drift == null
                                        ? <span className="text-muted-foreground/50">n/a</span>
                                        : <span className={drift > 0.1 ? "text-yellow-600 font-medium" : "text-green-600"}>
                                            Δ{drift.toFixed(4)}
                                          </span>
                                    }
                                </div>
                            );
                        })}
                        <p className="text-muted-foreground/50 pt-1">cosine dist between last 2 run centroids</p>
                    </CardContent>
                </Card>
            </div>
        </div>
    );
}
