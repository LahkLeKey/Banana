import { useState } from "react";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { Textarea } from "../components/ui/textarea";
import { Button } from "../components/ui/button";
import { fetchEnsembleVerdictWithEmbedding, predictRipeness, resolveApiBaseUrl } from "../lib/api";
import type { RipenessResponse } from "../lib/api";

const modelLanes = [
    { name: "Banana Classifier", role: "Primary classification and ensemble verdict" },
    { name: "Ripeness Model", role: "Ripeness scoring and confidence explanation" },
    { name: "Not-Banana Detector", role: "Out-of-domain rejection and escalation" },
    { name: "Operator Reasoning", role: "Runbook-aware operational guidance" },
];

function RipenessBar({ score }: { score: number }) {
    const pct = Math.min(100, Math.round(score * 100));
    const color = score < 0.33 ? "bg-green-500" : score < 0.66 ? "bg-yellow-500" : "bg-red-500";
    const label = score < 0.33 ? "unripe" : score < 0.66 ? "ripe" : "overripe";
    return (
        <div className="space-y-1">
            <div className="flex justify-between text-xs text-muted-foreground">
                <span>ripeness score</span>
                <span className="font-mono">{score.toFixed(3)} — {label}</span>
            </div>
            <div className="h-2 w-full rounded-full bg-muted overflow-hidden">
                <div className={`h-full rounded-full ${color}`} style={{ width: `${pct}%` }} />
            </div>
            <div className="flex justify-between text-xs text-muted-foreground/50">
                <span>0 unripe</span><span>0.5 ripe</span><span>1 overripe</span>
            </div>
        </div>
    );
}

export function BananaAIPage() {
    const [query, setQuery] = useState("");
    const [result, setResult] = useState<string | null>(null);
    const [error, setError] = useState<string | null>(null);
    const [loading, setLoading] = useState(false);
    const [ripenessData, setRipenessData] = useState<RipenessResponse | null>(null);
    const [ripenessLoading, setRipenessLoading] = useState(false);
    const [ripenessError, setRipenessError] = useState<string | null>(null);

    async function run() {
        if (!query.trim()) return;
        setLoading(true);
        setResult(null);
        setError(null);
        try {
            const base = resolveApiBaseUrl();
            const result = await fetchEnsembleVerdictWithEmbedding(base, query.trim());
            const v = result.verdict;
            const lines = [
                `label: ${v.label}`,
                `score: ${v.score?.toFixed(3) ?? "n/a"}`,
                `escalated: ${v.did_escalate}`,
                `calibration: ${v.calibration_magnitude?.toFixed(3) ?? "n/a"}`,
                `status: ${v.status}`,
                result.embedding ? `embedding: [${result.embedding.map((x) => x.toFixed(3)).join(", ")}]` : null,
            ].filter(Boolean).join("\n");
            setResult(lines);
        } catch (e) {
            setError(e instanceof Error ? e.message : String(e));
        } finally {
            setLoading(false);
        }
    }

    async function runRipeness() {
        if (!query.trim()) return;
        setRipenessLoading(true);
        setRipenessData(null);
        setRipenessError(null);
        try {
            const base = resolveApiBaseUrl();
            const data = await predictRipeness(base, JSON.stringify({ sample: query.trim() }));
            setRipenessData(data);
        } catch (e) {
            setRipenessError(e instanceof Error ? e.message : String(e));
        } finally {
            setRipenessLoading(false);
        }
    }

    return (
        <div className="space-y-4">
            <Card>
                <CardHeader>
                    <CardTitle>BananaAI</CardTitle>
                    <CardDescription>
                        Unified assistant that composes Banana custom models into a single Rovo-style replacement experience.
                    </CardDescription>
                </CardHeader>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Ask BananaAI</CardTitle>
                    <CardDescription>
                        Routes your query through the ensemble model (Banana Classifier + Not-Banana Detector) for a combined verdict.
                    </CardDescription>
                </CardHeader>
                <CardContent className="space-y-3">
                    <Textarea
                        value={query}
                        onChange={(e) => setQuery(e.target.value)}
                        placeholder="Ask about classification, ripeness, model drift, or describe an item..."
                        rows={4}
                    />
                    <div className="flex gap-2">
                        <Button onClick={run} disabled={loading || !query.trim()}>
                            {loading ? "Running..." : "Run Ensemble"}
                        </Button>
                        <Button variant="outline" onClick={runRipeness} disabled={ripenessLoading || !query.trim()}>
                            {ripenessLoading ? "Running..." : "Run Ripeness"}
                        </Button>
                    </div>
                    {result && (
                        <div className="grid gap-2 text-sm">
                            {result.split("\n").map((line) => {
                                const [key, ...rest] = line.split(": ");
                                const val = rest.join(": ");
                                const isEmbedding = key === "embedding";
                                return (
                                    <div key={key} className="flex items-start justify-between rounded-md border px-3 py-2">
                                        <span className="font-medium text-muted-foreground">{key}</span>
                                        {isEmbedding
                                            ? <span className="text-xs text-muted-foreground/60 truncate max-w-[60%]">{val}</span>
                                            : <span className="font-mono">{val}</span>
                                        }
                                    </div>
                                );
                            })}
                        </div>
                    )}
                    {error && (
                        <p className="text-sm text-destructive">{error}</p>
                    )}

                    {/* Spike 168: ripeness histogram */}
                    {ripenessData != null && (
                        <div className="rounded-md border p-3 space-y-2">
                            <p className="text-xs font-semibold text-muted-foreground uppercase tracking-widest">
                                Ripeness Pipeline — <span className="normal-case">{ripenessData.label}</span>
                            </p>
                            <RipenessBar score={ripenessData.confidence} />
                        </div>
                    )}
                    {ripenessError && (
                        <p className="text-sm text-destructive">{ripenessError}</p>
                    )}
                </CardContent>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Model Lanes</CardTitle>
                </CardHeader>
                <CardContent className="grid gap-2 text-sm">
                    {modelLanes.map((lane) => (
                        <div key={lane.name} className="rounded-md border p-3">
                            <p className="font-medium">{lane.name}</p>
                            <p className="text-muted-foreground">{lane.role}</p>
                        </div>
                    ))}
                </CardContent>
            </Card>
        </div>
    );
}
