import { useEffect, useState } from "react";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { fetchTrainingWorkbenchHistory, resolveApiBaseUrl } from "../lib/api";
import type { TrainingHistoryResponse } from "../lib/api";
import { computeCosineDrift, TrainingMetricCard } from "@banana/ui";
import type { EmbeddingDriftSummary, TrainingLane, TrainingRunResult } from "@banana/ui";

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
    const driftByLane: EmbeddingDriftSummary[] = [];
    for (const lane of ["left-brain", "right-brain"] as TrainingLane[]) {
        const laneRows = laneRuns(rows, lane);
        if (laneRows.length >= 2) {
            const e0 = extractEmbeddingFromRun(laneRows[0]);
            const e1 = extractEmbeddingFromRun(laneRows[1]);
            driftByLane.push({
                lane,
                drift : e0 && e1 ? computeCosineDrift(e0, e1) : null,
            });
        } else {
            driftByLane.push({lane, drift : null});
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
                        <TrainingMetricCard lane="left-brain" runs={laneRuns(rows, "left-brain")} />
                        <TrainingMetricCard lane="right-brain" runs={laneRuns(rows, "right-brain")} />
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
                        {driftByLane.map(({lane, drift}) => {
                            return (
                                <div key={lane} className="flex items-center justify-between">
                                    <span className="text-muted-foreground">{lane}</span>
                                    {drift == null
                                        ? <span className="text-muted-foreground/50">—</span>
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
