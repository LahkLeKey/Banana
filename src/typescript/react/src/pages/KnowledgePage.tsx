import { useEffect, useState } from "react";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { fetchTrainingWorkbenchHistory, resolveApiBaseUrl } from "../lib/api";
import type { TrainingHistoryResponse } from "../lib/api";
import type { TrainingRunResult } from "@banana/ui";

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

export function KnowledgePage() {
    const [history, setHistory] = useState<TrainingHistoryResponse | null>(null);

    useEffect(() => {
        fetchTrainingWorkbenchHistory(resolveApiBaseUrl())
            .then(setHistory)
            .catch(() => setHistory(null));
    }, []);

    const recentRuns = history?.rows?.slice(0, 4) ?? [];

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

            {/* Article canvas */}
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
            </div>

            {/* Activity feed */}
            <Card>
                <CardHeader className="pb-2">
                    <CardTitle className="text-sm">Training Activity</CardTitle>
                </CardHeader>
                <CardContent className="space-y-2 text-xs text-muted-foreground">
                    {recentRuns.length > 0 ? recentRuns.map((run: TrainingRunResult) => (
                        <div key={run.run_id} className="border-b pb-1 last:border-0">
                            <p className="font-medium text-foreground">{run.lane ?? "unknown"}</p>
                            <p>{run.status} · {run.run_id?.slice(-8)}</p>
                        </div>
                    )) : (
                        <p>No training runs available</p>
                    )}
                </CardContent>
            </Card>
        </div>
    );
}
