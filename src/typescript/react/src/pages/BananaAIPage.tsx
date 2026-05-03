import { useState } from "react";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { Textarea } from "../components/ui/textarea";
import { Button } from "../components/ui/button";
import { fetchEnsembleVerdictWithEmbedding, resolveApiBaseUrl } from "../lib/api";

const modelLanes = [
    { name: "Banana Classifier", role: "Primary classification and ensemble verdict" },
    { name: "Ripeness Model", role: "Ripeness scoring and confidence explanation" },
    { name: "Not-Banana Detector", role: "Out-of-domain rejection and escalation" },
    { name: "Operator Reasoning", role: "Runbook-aware operational guidance" },
];

export function BananaAIPage() {
    const [query, setQuery] = useState("");
    const [result, setResult] = useState<string | null>(null);
    const [error, setError] = useState<string | null>(null);
    const [loading, setLoading] = useState(false);

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
                    <Button onClick={run} disabled={loading || !query.trim()}>
                        {loading ? "Running..." : "Run BananaAI"}
                    </Button>
                    {result && (
                        <pre className="rounded-md bg-muted p-3 text-xs whitespace-pre-wrap">{result}</pre>
                    )}
                    {error && (
                        <p className="text-sm text-destructive">{error}</p>
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
