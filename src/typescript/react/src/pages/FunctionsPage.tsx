import { useState } from "react";
import type { PromotionAuditEntry } from "@banana/ui";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { Button } from "../components/ui/button";
import { fetchTrainingWorkbenchHistory, predictRipeness, promoteTrainingWorkbenchRun, resolveApiBaseUrl, runTrainingWorkbench } from "../lib/api";

type FnStatus = "ready" | "blocked" | "running" | "done" | "error";
type FnGroup = { group: string; fns: FnDef[] };
type FnActionResult = { output: string; promotion?: PromotionAuditEntry };
type FnDef = { name: string; owner: string; status: FnStatus; action?: () => Promise<FnActionResult>; confirm?: string };

async function pingHealth(): Promise<FnActionResult> {
    const base = resolveApiBaseUrl();
    const res = await fetch(`${base}/health`);
    if (!res.ok) throw new Error(`health check returned ${res.status}`);
    const data = await res.json() as { status?: string };
    return { output: `health: ${data.status ?? "ok"}` };
}

async function fetchBananaSummaryFn(): Promise<FnActionResult> {
    const base = resolveApiBaseUrl();
    const res = await fetch(`${base}/banana?purchases=3&multiplier=2`);
    if (!res.ok) throw new Error(`banana summary returned ${res.status}`);
    const data = await res.json() as Record<string, unknown>;
    return { output: JSON.stringify(data, null, 2) };
}

async function fetchTrainingHistoryFn(): Promise<FnActionResult> {
    const base = resolveApiBaseUrl();
    const data = await fetchTrainingWorkbenchHistory(base);
    const rows = data.rows ?? [];
    if (rows.length === 0) return { output: "No training runs found." };
    return { output: rows.slice(0, 5).map((r) =>
        `[${r.lane}] ${r.status} — ${r.run_id.slice(-8)} @ ${r.finished_at ? new Date(r.finished_at).toLocaleString() : "running"}`
    ).join("\n") };
}

async function runLeftBrainCiFn(): Promise<FnActionResult> {
    const base = resolveApiBaseUrl();
    const data = await runTrainingWorkbench(base, {
        lane: "left-brain",
        training_profile: "ci",
        session_mode: "single",
        max_sessions: 1,
        operator_id: "suite-ui",
        notes: "Triggered from Function Catalog",
    });
    return { output: `run_id: ${data.run.run_id}\nstatus: ${data.run.status}\nlane: ${data.run.lane}` };
}

async function runRipenessSmokeFn(): Promise<FnActionResult> {
    const base = resolveApiBaseUrl();
    const data = await predictRipeness(base, {sample : "yellow banana, slightly soft, brown spots at tip"});
    return { output: JSON.stringify(data, null, 2) };
}

// Spike 170: promotion governance — fetches latest passed run and promotes to candidate
async function promoteLatestCandidateFn(): Promise<FnActionResult> {
    const base = resolveApiBaseUrl();
    const history = await fetchTrainingWorkbenchHistory(base);
    const passed = (history.rows ?? []).find((r) => r.status === "passed");
    if (!passed) return { output: "No passed runs available to promote." };
    const result = await promoteTrainingWorkbenchRun(base, passed.run_id, "candidate", "suite-ui", "Promoted via Function Catalog governance gate");
    const p = result.promoted;
    return {
        output : `run_id: ${p.run_id}\ntarget: ${p.target}\nthreshold_passed: ${p.threshold_passed}`,
        promotion : {
            run_id : p.run_id,
            promoted_at : new Date().toISOString(),
            lane : passed.lane,
        },
    };
}
const CATALOG: FnGroup[] = [
    {
        group: "Platform Health",
        fns: [
            { name: "Validate Runtime Health", owner: "Platform", status: "ready", action: pingHealth },
            { name: "Fetch Banana Summary", owner: "Classifier", status: "ready", action: fetchBananaSummaryFn },
        ],
    },
    {
        group: "Model Ops",
        fns: [
            { name: "Fetch Training History", owner: "Data Science", status: "ready", action: fetchTrainingHistoryFn },
            { name: "Run Left-Brain CI Workbench", owner: "Data Science", status: "ready", action: runLeftBrainCiFn },
            { name: "Run Ripeness Smoke", owner: "Data Science", status: "ready", action: runRipenessSmokeFn },
            {
                name: "Promote Latest to Candidate",
                owner: "Data Science",
                status: "ready",
                action: promoteLatestCandidateFn,
                confirm: "This will promote the latest passed training run to candidate. threshold_passed must be true. Proceed?",
            },
        ],
    },
    {
        group: "Delivery",
        fns: [
            { name: "Promote Vercel Release", owner: "Delivery", status: "blocked" },
            { name: "Validate Runtime Contracts", owner: "Integration", status: "blocked" },
        ],
    },
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
    const [promotionAudit, setPromotionAudit] = useState<PromotionAuditEntry[]>([]);

    async function run(fn: FnDef) {
        if (!fn.action) return;
        // Spike 170: confirmation gate for governance-protected actions
        if (fn.confirm && !window.confirm(fn.confirm)) return;
        setOutputs((o) => ({ ...o, [fn.name]: "" }));
        setStates((s) => ({ ...s, [fn.name]: "running" }));
        try {
            const result = await fn.action();
            setOutputs((o) => ({ ...o, [fn.name]: result.output }));
            if (result.promotion)
            {
                setPromotionAudit((prev) => [result.promotion, ...prev]);
            }
            setStates((s) => ({ ...s, [fn.name]: "done" }));
        } catch (e) {
            setOutputs((o) => ({ ...o, [fn.name]: e instanceof Error ? e.message : String(e) }));
            setStates((s) => ({ ...s, [fn.name]: "error" }));
        }
    }

    return (
        <div className="space-y-6">
            <Card>
                <CardHeader>
                    <CardTitle>Function Catalog</CardTitle>
                    <CardDescription>
                        Actionable automation and workflow entry points for the Banana platform.
                    </CardDescription>
                </CardHeader>
            </Card>

            {CATALOG.map(({ group, fns }) => (
                <div key={group} className="space-y-2">
                    <p className="text-xs font-semibold uppercase tracking-widest text-muted-foreground px-1">{group}</p>
                    <div className="grid gap-3">
                        {fns.map((fn) => {
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
            ))}

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Promotion Audit</CardTitle>
                    <CardDescription>Recent successful promotion actions from this session.</CardDescription>
                </CardHeader>
                <CardContent className="space-y-2 text-sm">
                    {promotionAudit.length === 0 ? (
                        <p className="text-muted-foreground">No promotions recorded yet.</p>
                    ) : (
                        promotionAudit.map((entry) => (
                            <div key={`${entry.run_id}-${entry.promoted_at}`} className="rounded border p-2">
                                <p><span className="text-muted-foreground">run</span> {entry.run_id}</p>
                                <p><span className="text-muted-foreground">lane</span> {entry.lane}</p>
                                <p><span className="text-muted-foreground">at</span> {entry.promoted_at}</p>
                            </div>
                        ))
                    )}
                </CardContent>
            </Card>
        </div>
    );
}
