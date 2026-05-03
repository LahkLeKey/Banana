import { useState } from "react";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { Button } from "../components/ui/button";
import { resolveApiBaseUrl } from "../lib/api";

type FnStatus = "ready" | "blocked" | "running" | "done" | "error";
type FnDef = { name: string; owner: string; status: FnStatus; action?: () => Promise<string> };

async function pingHealth(): Promise<string> {
    const base = resolveApiBaseUrl();
    const res = await fetch(`${base}/health`);
    if (!res.ok) throw new Error(`health check returned ${res.status}`);
    const data = await res.json() as { status?: string };
    return `health: ${data.status ?? "ok"}`;
}

async function fetchBananaSummaryFn(): Promise<string> {
    const base = resolveApiBaseUrl();
    const res = await fetch(`${base}/banana?purchases=3&multiplier=2`);
    if (!res.ok) throw new Error(`banana summary returned ${res.status}`);
    const data = await res.json() as Record<string, unknown>;
    return JSON.stringify(data, null, 2);
}

const CATALOG: FnDef[] = [
    { name: "Validate Runtime Health", owner: "Platform", status: "ready", action: pingHealth },
    { name: "Fetch Banana Summary", owner: "Classifier", status: "ready", action: fetchBananaSummaryFn },
    { name: "Promote Vercel Release", owner: "Delivery", status: "blocked" },
    { name: "Validate Runtime Contracts", owner: "Integration", status: "blocked" },
];

const STATUS_LABEL: Record<FnStatus, string> = {
    ready: "Ready",
    blocked: "Blocked",
    running: "Running…",
    done: "Done",
    error: "Error",
};

export function FunctionsPage() {
    const [states, setStates] = useState<Record<string, FnStatus>>({});
    const [outputs, setOutputs] = useState<Record<string, string>>({});

    async function run(fn: FnDef) {
        if (!fn.action) return;
        setStates((s) => ({ ...s, [fn.name]: "running" }));
        try {
            const out = await fn.action();
            setOutputs((o) => ({ ...o, [fn.name]: out }));
            setStates((s) => ({ ...s, [fn.name]: "done" }));
        } catch (e) {
            setOutputs((o) => ({ ...o, [fn.name]: e instanceof Error ? e.message : String(e) }));
            setStates((s) => ({ ...s, [fn.name]: "error" }));
        }
    }

    return (
        <div className="space-y-4">
            <Card>
                <CardHeader>
                    <CardTitle>Function Catalog</CardTitle>
                    <CardDescription>
                        Actionable automation and workflow entry points for the Banana platform.
                    </CardDescription>
                </CardHeader>
            </Card>

            <div className="grid gap-3">
                {CATALOG.map((fn) => {
                    const effectiveStatus: FnStatus = states[fn.name] ?? fn.status;
                    const output = outputs[fn.name];
                    return (
                        <Card key={fn.name}>
                            <CardContent className="py-4 space-y-2">
                                <div className="flex items-center justify-between text-sm">
                                    <div>
                                        <p className="font-medium">{fn.name}</p>
                                        <p className="text-muted-foreground">Owner: {fn.owner}</p>
                                    </div>
                                    <div className="flex items-center gap-2">
                                        <span className="rounded-full border px-2 py-1 text-xs">
                                            {STATUS_LABEL[effectiveStatus]}
                                        </span>
                                        {fn.action && (
                                            <Button
                                                size="sm"
                                                variant="outline"
                                                disabled={effectiveStatus === "running" || effectiveStatus === "blocked"}
                                                onClick={() => run(fn)}
                                            >
                                                Run
                                            </Button>
                                        )}
                                    </div>
                                </div>
                                {output && (
                                    <pre className="rounded-md bg-muted p-2 text-xs whitespace-pre-wrap">{output}</pre>
                                )}
                            </CardContent>
                        </Card>
                    );
                })}
            </div>
        </div>
    );
}
