import { useEffect, useMemo, useState } from "react";
import { Button } from "../components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { Input } from "../components/ui/input";
import { fetchTrainingWorkbenchHistory, resolveApiBaseUrl } from "../lib/api";
import type { TrainingHistoryResponse } from "../lib/api";
import { computeCosineDrift, TrainingMetricCard } from "@banana/ui";
import type { EmbeddingDriftSummary, TrainingLane, TrainingRunResult } from "@banana/ui";

type Space = "All" | "Platform Engineering" | "Model Runtime" | "Delivery Contracts" | "Incident Playbooks";
type HealthStatus = "loading" | "ok" | "degraded" | "unavailable";

type Article = {
    title: string;
    summary: string;
    detail: string;
    href: string;
    space: Exclude<Space, "All">;
};

const ARTICLES: Article[] = [
    // Platform Engineering
    {
        title: "Native Wrapper ABI",
        summary: "Wrapper ABI surface, status codes, buffer ownership, and downstream interop contracts.",
        detail: "Covers all exported C symbols, status code taxonomy, buffer lifetime rules, and how ASP.NET P/Invoke consumes the wrapper. Update whenever an export is added or renamed.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/.specify/wiki/human-reference/architecture/native-wrapper-abi.md",
        space: "Platform Engineering",
    },
    {
        title: "CMake Build Presets",
        summary: "CMakePresets.json configure targets, sanitizer lanes, and cross-platform build matrix.",
        detail: "Documents each named preset (debug, release, asan, ci), the output path contract (/out/native/bin), and the Docker native-builder stage dependencies.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/CMakePresets.json",
        space: "Platform Engineering",
    },
    {
        title: "ASP.NET Pipeline Architecture",
        summary: "Controller → Service → PipelineExecutor → IPipelineStep ordered processing model.",
        detail: "Ordered pipeline steps (InputValidation:10, DriftSampling:50, EnsembleGating:100, Escalation:200, Calibration:300, AbVariant:150). New steps must register with a deterministic Order value and integration tests for ordering behavior.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/src/c-sharp/asp.net/README.md",
        space: "Platform Engineering",
    },
    {
        title: "Shared UI Token Contract",
        summary: "Design tokens, Tailwind preset, and component contracts in @banana/ui.",
        detail: "All React, Electron, and React Native apps must consume @banana/ui tokens rather than inline values. Covers token generation, web/native split, and the Storybook documentation surface.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/.github/shared-typescript-ui.md",
        space: "Platform Engineering",
    },
    // Model Runtime
    {
        title: "Ensemble Verdict Pipeline",
        summary: "Left-brain + right-brain gating, escalation, calibration, and A/B variant tagging.",
        detail: "The full ensemble flow: gating step checks confidence threshold, escalation promotes low-confidence verdicts, calibration applies Platt scaling, and A/B variant step tags requests for experiment tracking.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/src/c-sharp/asp.net/Pipeline",
        space: "Model Runtime",
    },
    {
        title: "Training Profiles",
        summary: "CI (single-session, fast) vs local (multi-session) training profile contracts.",
        detail: "train-banana-model.py, train-ripeness-model.py, and train-not-banana-model.py each accept --training-profile ci|local and --session-mode single|multi. CI profiles enforce max-sessions 1 for deterministic runs.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/scripts/train-banana-model.py",
        space: "Model Runtime",
    },
    {
        title: "Embedding Drift Detection",
        summary: "Cosine distance between consecutive run centroids per lane surfaces model drift.",
        detail: "Drift > 0.1 cosine distance signals significant distribution shift. Computed in @banana/ui computeCosineDrift and surfaced on the Knowledge page. Feeds the Spike 078 drift-detection flow.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/src/typescript/shared/ui/src/index.ts",
        space: "Model Runtime",
    },
    {
        title: "Model Registry and Releases",
        summary: "Artifact paths, signed release contracts, and local vs CI output directories.",
        detail: "Models land in artifacts/training/<model>/local for local runs and .artifacts/<model> for CI. The release flow signs artifacts before promotion. BANANA_NATIVE_PATH must reference the correct .so path at runtime.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/artifacts",
        space: "Model Runtime",
    },
    // Delivery Contracts
    {
        title: "Deployment Readiness Playbook",
        summary: "Release readiness evidence linked to runtime contracts, rollout checks, and post-deploy health thresholds.",
        detail: "Covers pre-deploy checklist, canary health gate, post-deploy /health assertion, and rollback trigger conditions. Canonical source for go/no-go decisions.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/.specify/wiki/human-reference/operations/deployment.md",
        space: "Delivery Contracts",
    },
    {
        title: "Vercel Frontend Deploy Contract",
        summary: "VITE_BANANA_API_BASE_URL, build output, and Vercel project settings.",
        detail: "Production builds require VITE_BANANA_API_BASE_URL=https://api.banana.engineer. vercel --yes --prod deploys from the React dist output. Preview deploys use BANANA_CORS_ORIGINS env var expansion on the API side.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/.specify/specs/122-vercel-ci-deploy-workflow/plan.md",
        space: "Delivery Contracts",
    },
    // Incident Playbooks
    {
        title: "WSL2 Runtime Channels",
        summary: "Container-driven local runtime: Docker Desktop + Ubuntu WSL2 + Electron/Mobile channels.",
        detail: "Launch via scripts/launch-container-channels-with-wsl2-electron.sh from Windows shell. Requires Docker Desktop + Ubuntu-24.04 WSL2. Exit code 42 = integration preflight failure with actionable remediation.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/.specify/wiki/human-reference/operations/wsl2-runtime-channels.md",
        space: "Incident Playbooks",
    },
    {
        title: "API Swagger Reliability",
        summary: "Swashbuckle schema ID collision fix, CORS middleware ordering, and spec endpoint health.",
        detail: "Root cause: nested InputJsonRequest types across controllers collide on schemaId. Fix: CustomSchemaIds(type => type.FullName). CORS must be registered before UseSwagger. Validate with curl -i /swagger/v1/swagger.json post-deploy.",
        href: "https://github.com/LahkLeKey/Banana/blob/main/.specify/specs/181-swagger-runtime-hardening/plan.md",
        space: "Incident Playbooks",
    },
];

const SPACES: Space[] = ["All", "Platform Engineering", "Model Runtime", "Delivery Contracts", "Incident Playbooks"];

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
    const [selectedRun, setSelectedRun] = useState<TrainingRunResult | null>(null);
    const [search, setSearch] = useState("");
    const [activeSpace, setActiveSpace] = useState<Space>("All");
    const [expandedArticle, setExpandedArticle] = useState<string | null>(null);
    const [healthStatus, setHealthStatus] = useState<HealthStatus>("loading");
    const [healthDetail, setHealthDetail] = useState<string>("");

    useEffect(() => {
        fetchTrainingWorkbenchHistory(resolveApiBaseUrl())
            .then(setHistory)
            .catch(() => setHistory(null));
    }, []);

    useEffect(() => {
        const base = resolveApiBaseUrl();
        fetch(`${base}/health`)
            .then((r) => r.json())
            .then((data: unknown) => {
                const d = data as Record<string, unknown>;
                const status = typeof d.status === "string" ? d.status : "unknown";
                setHealthStatus(status === "healthy" ? "ok" : "degraded");
                setHealthDetail(status);
            })
            .catch(() => {
                setHealthStatus("unavailable");
                setHealthDetail("unavailable");
            });
    }, []);

    const filteredArticles = useMemo(() => {
        const q = search.trim().toLowerCase();
        return ARTICLES.filter((a) => {
            const spaceMatch = activeSpace === "All" || a.space === activeSpace;
            const textMatch =
                q === "" ||
                a.title.toLowerCase().includes(q) ||
                a.summary.toLowerCase().includes(q) ||
                a.detail.toLowerCase().includes(q);
            return spaceMatch && textMatch;
        });
    }, [search, activeSpace]);

    const rows = history?.rows ?? [];
    const recentRuns = rows.slice(0, 4);
    const latestTimestamp = rows[0]?.finished_at ? new Date(rows[0].finished_at).getTime() : null;
    const staleData = latestTimestamp != null && Date.now() - latestTimestamp > 1000 * 60 * 60 * 24;

    const laneHealth = useMemo(() => {
        return (lane: TrainingLane): "healthy" | "warning" | "critical" => {
            const run = laneRuns(rows, lane)[0];
            if (!run) return "critical";
            if (run.status === "passed") return "healthy";
            if (run.status === "running") return "warning";
            return "critical";
        };
    }, [rows]);

    function sparklineForLane(lane: TrainingLane): string {
        const laneRows = laneRuns(rows, lane).slice(0, 8).reverse();
        if (laneRows.length === 0) return "-";
        const blocks = ["▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"];
        return laneRows.map((run) => {
            const score = Math.max(0, Math.min(1, run.metrics?.accuracy ?? 0));
            const idx = Math.min(blocks.length - 1, Math.round(score * (blocks.length - 1)));
            return blocks[idx];
        }).join("");
    }

    async function copyVisibleMetrics() {
        const payload = {
            rows: rows.slice(0, 12).map((run) => ({
                run_id: run.run_id,
                lane: run.lane,
                status: run.status,
                accuracy: run.metrics?.accuracy ?? null,
                f1: run.metrics?.f1 ?? null,
                finished_at: run.finished_at ?? null,
            })),
            driftByLane,
        };
        await navigator.clipboard.writeText(JSON.stringify(payload, null, 2));
    }

    // Spike 169: cosine drift between most recent consecutive runs per lane
    const driftByLane: EmbeddingDriftSummary[] = [];
    for (const lane of ["left-brain", "right-brain"] as TrainingLane[]) {
        const laneRows = laneRuns(rows, lane);
        if (laneRows.length >= 2) {
            const e0 = extractEmbeddingFromRun(laneRows[0]);
            const e1 = extractEmbeddingFromRun(laneRows[1]);
            driftByLane.push({
                lane,
                drift: e0 && e1 ? computeCosineDrift(e0, e1) : null,
            });
        } else {
            driftByLane.push({ lane, drift: null });
        }
    }

    return (
        <div className="space-y-4">
            {/* Health strip */}
            <div className={`flex items-center gap-3 rounded-md border px-4 py-2 text-sm ${healthStatus === "ok" ? "border-green-500/40 bg-green-500/5 text-green-700" : healthStatus === "degraded" ? "border-yellow-500/40 bg-yellow-500/5 text-yellow-700" : healthStatus === "unavailable" ? "border-red-500/40 bg-red-500/5 text-red-700" : "border-border text-muted-foreground"}`}>
                <span className={`inline-block h-2 w-2 rounded-full ${healthStatus === "ok" ? "bg-green-500" : healthStatus === "degraded" ? "bg-yellow-500" : healthStatus === "unavailable" ? "bg-red-500" : "bg-muted-foreground/40 animate-pulse"}`} />
                <span className="font-medium">API</span>
                <span>{healthStatus === "loading" ? "checking…" : healthDetail}</span>
            </div>

            <div className="grid gap-4 lg:grid-cols-[200px_1fr_260px]">
                {/* Spaces sidebar */}
                <Card>
                    <CardHeader className="pb-2">
                        <CardTitle className="text-sm">Spaces</CardTitle>
                    </CardHeader>
                    <CardContent className="space-y-1 text-sm">
                        {SPACES.map((s) => (
                            <p
                                key={s}
                                onClick={() => setActiveSpace(s)}
                                className={`cursor-pointer rounded px-2 py-1 transition-colors ${activeSpace === s ? "bg-muted font-medium text-foreground" : "text-muted-foreground hover:text-foreground hover:bg-muted/50"}`}
                            >
                                {s}
                            </p>
                        ))}
                    </CardContent>
                </Card>

                {/* Article canvas */}
                <div className="space-y-4">
                    <Card>
                        <CardHeader>
                            <CardTitle>Knowledge Base</CardTitle>
                            <CardDescription>Domain narratives, ADRs, and runbooks</CardDescription>
                        </CardHeader>
                        <CardContent>
                            <Input
                                placeholder="Search articles…"
                                value={search}
                                onChange={(e) => setSearch(e.target.value)}
                                className="max-w-sm"
                            />
                        </CardContent>
                        {staleData && (
                            <CardContent className="pt-0">
                                <p className="rounded-md border border-yellow-500/50 bg-yellow-500/10 px-3 py-2 text-sm text-yellow-700">
                                    Warning: training data appears stale (older than 24h).
                                </p>
                            </CardContent>
                        )}
                    </Card>

                    {filteredArticles.length === 0 && (
                        <p className="px-1 text-sm text-muted-foreground">No articles match your search.</p>
                    )}

                    {filteredArticles.map((a) => (
                        <Card key={a.title}>
                            <CardHeader className="pb-2">
                                <div className="flex items-start justify-between gap-2">
                                    <div>
                                        <CardTitle className="text-base">
                                            <a href={a.href} target="_blank" rel="noreferrer" className="hover:underline">{a.title}</a>
                                        </CardTitle>
                                        <CardDescription className="mt-1">{a.summary}</CardDescription>
                                    </div>
                                    <button
                                        type="button"
                                        onClick={() => setExpandedArticle(expandedArticle === a.title ? null : a.title)}
                                        className="shrink-0 rounded border px-2 py-0.5 text-xs text-muted-foreground hover:bg-muted"
                                    >
                                        {expandedArticle === a.title ? "Less" : "More"}
                                    </button>
                                </div>
                                <span className="inline-block rounded-full border px-2 py-0.5 text-[10px] text-muted-foreground">{a.space}</span>
                            </CardHeader>
                            {expandedArticle === a.title && (
                                <CardContent className="pt-0">
                                    <p className="rounded-md bg-muted/40 px-3 py-2 text-sm text-foreground/80">{a.detail}</p>
                                </CardContent>
                            )}
                        </Card>
                    ))}

                    {/* Per-lane training metric cards */}
                    {rows.length > 0 && (
                        <>
                            <div className="flex items-center justify-between px-1">
                                <p className="text-xs font-semibold uppercase tracking-widest text-muted-foreground">Training Metrics</p>
                                <Button size="sm" variant="outline" onClick={copyVisibleMetrics}>Copy/Export Metrics</Button>
                            </div>
                            <TrainingMetricCard lane="left-brain" runs={laneRuns(rows, "left-brain")} />
                            <TrainingMetricCard lane="right-brain" runs={laneRuns(rows, "right-brain")} />

                            <Card>
                                <CardHeader className="pb-2">
                                    <CardTitle className="text-sm">Run Details</CardTitle>
                                    <CardDescription>Select a recent run to inspect details inline.</CardDescription>
                                </CardHeader>
                                <CardContent className="space-y-2 text-sm">
                                    <div className="grid gap-1">
                                        {rows.slice(0, 8).map((run) => (
                                            <button
                                                key={run.run_id}
                                                type="button"
                                                className="rounded border px-2 py-1 text-left hover:bg-muted"
                                                onClick={() => setSelectedRun(run)}
                                            >
                                                {run.lane} · {run.status} · {run.run_id.slice(-8)}
                                            </button>
                                        ))}
                                    </div>

                                    {selectedRun && (
                                        <div className="rounded-md border bg-muted/30 p-3 text-xs space-y-1">
                                            <p><span className="text-muted-foreground">run_id</span> {selectedRun.run_id}</p>
                                            <p><span className="text-muted-foreground">lane</span> {selectedRun.lane}</p>
                                            <p><span className="text-muted-foreground">status</span> {selectedRun.status}</p>
                                            <p><span className="text-muted-foreground">accuracy</span> {selectedRun.metrics?.accuracy ?? "n/a"}</p>
                                            <p><span className="text-muted-foreground">f1</span> {selectedRun.metrics?.f1 ?? "n/a"}</p>
                                            <p><span className="text-muted-foreground">finished_at</span> {selectedRun.finished_at ?? "n/a"}</p>
                                        </div>
                                    )}
                                </CardContent>
                            </Card>
                        </>
                    )}
                </div>

                {/* Activity + drift sidebar */}
                <div className="space-y-3">
                    <Card>
                        <CardHeader className="pb-2">
                            <CardTitle className="text-sm">Training Activity</CardTitle>
                        </CardHeader>
                        <CardContent className="space-y-2 text-xs text-muted-foreground">
                            {recentRuns.length > 0 ? recentRuns.map((run: TrainingRunResult) => (
                                <div key={run.run_id} className="border-b pb-1 last:border-0">
                                    <div className="flex items-center justify-between gap-2">
                                        <p className="font-medium text-foreground">{run.lane}</p>
                                        <span className={`rounded-full border px-2 py-0.5 text-[10px] ${laneHealth(run.lane) === "healthy" ? "border-green-500 text-green-600" : laneHealth(run.lane) === "warning" ? "border-yellow-500 text-yellow-600" : "border-red-500 text-red-600"}`}>
                                            {laneHealth(run.lane)}
                                        </span>
                                    </div>
                                    <p>{run.status} · {run.run_id.slice(-8)}</p>
                                    <p className="font-mono">{sparklineForLane(run.lane)}</p>
                                </div>
                            )) : (
                                <p>No training runs available</p>
                            )}
                        </CardContent>
                    </Card>

                    {/* Embedding drift delta */}
                    <Card>
                        <CardHeader className="pb-2">
                            <CardTitle className="text-sm">Embedding Drift</CardTitle>
                        </CardHeader>
                        <CardContent className="space-y-2 text-xs">
                            {driftByLane.map(({ lane, drift }) => (
                                <div key={lane} className="flex items-center justify-between">
                                    <span className="text-muted-foreground">{lane}</span>
                                    {drift == null
                                        ? <span className="text-muted-foreground/50">—</span>
                                        : <span className={drift > 0.1 ? "text-yellow-600 font-medium" : "text-green-600"}>
                                            Δ{drift.toFixed(4)}
                                        </span>
                                    }
                                </div>
                            ))}
                            <p className="text-muted-foreground/50 pt-1">cosine dist between last 2 run centroids</p>
                        </CardContent>
                    </Card>
                </div>
            </div>
        </div>
    );
}

