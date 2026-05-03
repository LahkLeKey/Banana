import { useEffect, useState } from "react";
import type { PromotionAuditEntry } from "@banana/ui";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { Button } from "../components/ui/button";
import { fetchTrainingWorkbenchHistory, predictRipeness, promoteTrainingWorkbenchRun, resolveApiBaseUrl, runTrainingWorkbench } from "../lib/api";

type FnStatus = "ready" | "blocked" | "running" | "done" | "error";
type FnGroup = { group: string; fns: FnDef[] };
type FnActionResult = { output: string; promotion?: PromotionAuditEntry; details?: Record<string, unknown> };
type FnDef = {
    name: string;
    owner: string;
    status: FnStatus;
    action?: () => Promise<FnActionResult>;
    confirm?: string;
    requiresWorkbench?: boolean;
};

type CapabilityMap = {
    workbench: boolean;
    health: boolean;
};

type StructuredError = {
    code: string;
    message: string;
    remediation: string;
    source: string;
};

type ExecutionEntry = {
    name: string;
    status: "done" | "error" | "dry-run";
    at: string;
    output: string;
};

const EXEC_HISTORY_KEY = "banana.functions.execution-history.v1";

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
    return {
        output: rows.slice(0, 5).map((r) =>
            `[${r.lane}] ${r.status} — ${r.run_id.slice(-8)} @ ${r.finished_at ? new Date(r.finished_at).toLocaleString() : "running"}`
        ).join("\n")
    };
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
    return {
        output: `run_id: ${data.run.run_id}\nstatus: ${data.run.status}\nlane: ${data.run.lane}`,
        details: { runId: data.run.run_id, lane: data.run.lane, status: data.run.status },
    };
}

async function runRipenessSmokeFn(): Promise<FnActionResult> {
    const base = resolveApiBaseUrl();
    const data = await predictRipeness(base, { sample: "yellow banana, slightly soft, brown spots at tip" });
    return { output: JSON.stringify(data, null, 2), details: data as unknown as Record<string, unknown> };
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
        output: `run_id: ${p.run_id}\ntarget: ${p.target}\nthreshold_passed: ${p.threshold_passed}`,
        promotion: {
            run_id: p.run_id,
            promoted_at: new Date().toISOString(),
            lane: passed.lane,
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
            {
                name: "Fetch Training History",
                owner: "Data Science",
                status: "ready",
                action: fetchTrainingHistoryFn,
                requiresWorkbench: true,
            },
            {
                name: "Run Left-Brain CI Workbench",
                owner: "Data Science",
                status: "ready",
                action: runLeftBrainCiFn,
                requiresWorkbench: true,
            },
            {
                name: "Run Ripeness Smoke",
                owner: "Data Science",
                status: "ready",
                action: runRipenessSmokeFn,
                requiresWorkbench: true,
            },
            {
                name: "Promote Latest to Candidate",
                owner: "Data Science",
                status: "ready",
                action: promoteLatestCandidateFn,
                confirm: "This will promote the latest passed training run to candidate. threshold_passed must be true. Proceed?",
                requiresWorkbench: true,
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
    const [workbenchAvailable, setWorkbenchAvailable] = useState<boolean | null>(null);
    const [capabilities, setCapabilities] = useState<CapabilityMap>({ workbench: false, health: false });
    const [dryRun, setDryRun] = useState<boolean>(false);
    const [structuredError, setStructuredError] = useState<StructuredError | null>(null);
    const [executionHistory, setExecutionHistory] = useState<ExecutionEntry[]>([]);
    const [releaseChecklist, setReleaseChecklist] = useState({ docs: false, tests: false, approvals: false });
    const [canaryHealthy, setCanaryHealthy] = useState<boolean>(false);
    const [rollbackLog, setRollbackLog] = useState<string[]>([]);

    useEffect(() => {
        const raw = localStorage.getItem(EXEC_HISTORY_KEY);
        if (!raw) return;
        try {
            const parsed = JSON.parse(raw) as ExecutionEntry[];
            if (Array.isArray(parsed)) setExecutionHistory(parsed);
        } catch {
            setExecutionHistory([]);
        }
    }, []);

    useEffect(() => {
        localStorage.setItem(EXEC_HISTORY_KEY, JSON.stringify(executionHistory));
    }, [executionHistory]);

    useEffect(() => {
        let active = true;
        const base = resolveApiBaseUrl();
        if (!base) {
            setWorkbenchAvailable(false);
            return () => {
                active = false;
            };
        }

        // Current production ASP.NET host does not expose workbench routes.
        // Skip probing to avoid noisy 404s in browser diagnostics.
        if (base.includes("api.banana.engineer")) {
            setWorkbenchAvailable(false);
            return () => {
                active = false;
            };
        }

        (async () => {
            try {
                const health = await fetch(`${base}/health`);
                if (!active) return;
                setCapabilities((prev) => ({ ...prev, health: health.ok }));

                const res = await fetch(`${base}/training/workbench/history`);
                if (!active) return;
                setWorkbenchAvailable(res.status !== 404);
                setCapabilities((prev) => ({ ...prev, workbench: res.status !== 404 }));
            } catch {
                if (!active) return;
                setWorkbenchAvailable(false);
                setCapabilities((prev) => ({ ...prev, workbench: false }));
            }
        })();

        return () => {
            active = false;
        };
    }, []);

    function preflight(fn: FnDef): string | null {
        if (fn.requiresWorkbench && capabilities.workbench === false) {
            return "workbench capability unavailable";
        }
        if (fn.name.includes("Health") && capabilities.health === false) {
            return "runtime health capability unavailable";
        }
        return null;
    }

    function recordExecution(entry: ExecutionEntry) {
        setExecutionHistory((prev) => [entry, ...prev].slice(0, 30));
    }

    async function run(fn: FnDef) {
        setStructuredError(null);
        if (!fn.action) return;
        if (fn.requiresWorkbench && workbenchAvailable === false) {
            setOutputs((o) => ({
                ...o,
                [fn.name]: "This deployment does not expose /training/workbench endpoints yet.",
            }));
            setStates((s) => ({ ...s, [fn.name]: "blocked" }));
            return;
        }

        const preflightError = preflight(fn);
        if (preflightError) {
            setOutputs((o) => ({ ...o, [fn.name]: `Preflight blocked: ${preflightError}` }));
            setStates((s) => ({ ...s, [fn.name]: "blocked" }));
            return;
        }

        const mutatingAction = fn.name.includes("Run") || fn.name.includes("Promote");
        if (dryRun && mutatingAction) {
            const msg = `Dry-run: skipped mutating action for ${fn.name}`;
            setOutputs((o) => ({ ...o, [fn.name]: msg }));
            setStates((s) => ({ ...s, [fn.name]: "done" }));
            recordExecution({ name: fn.name, status: "dry-run", at: new Date().toISOString(), output: msg });
            return;
        }

        // Spike 170: confirmation gate for governance-protected actions
        if (fn.confirm && !window.confirm(fn.confirm)) return;
        setOutputs((o) => ({ ...o, [fn.name]: "" }));
        setStates((s) => ({ ...s, [fn.name]: "running" }));
        try {
            const result = await fn.action();
            setOutputs((o) => ({ ...o, [fn.name]: result.output }));
            recordExecution({ name: fn.name, status: "done", at: new Date().toISOString(), output: result.output });
            const promotion = result.promotion;
            if (promotion) {
                setPromotionAudit((prev) => [promotion, ...prev]);
            }
            setStates((s) => ({ ...s, [fn.name]: "done" }));
        } catch (e) {
            const message = e instanceof Error ? e.message : String(e);
            setOutputs((o) => ({ ...o, [fn.name]: message }));
            recordExecution({ name: fn.name, status: "error", at: new Date().toISOString(), output: message });
            setStructuredError({
                code: "FN_EXECUTION_FAILED",
                message,
                remediation: "Confirm capability checks, validate API base URL, and retry with Dry-run enabled first.",
                source: fn.name,
            });
            setStates((s) => ({ ...s, [fn.name]: "error" }));
        }
    }

    async function checkCanaryHealth() {
        try {
            const base = resolveApiBaseUrl();
            const res = await fetch(`${base}/health`);
            setCanaryHealthy(res.ok);
        } catch {
            setCanaryHealthy(false);
        }
    }

    function runRollbackPlaybook() {
        const steps = [
            "Freeze promotions and disable mutating actions",
            "Restore previous known-good deployment alias",
            "Run health checks and compare baseline metrics",
            "Post incident update with verification matrix",
        ];
        setRollbackLog(steps);
    }

    const releaseReady = releaseChecklist.docs && releaseChecklist.tests && releaseChecklist.approvals && canaryHealthy;

    return (
        <div className="space-y-6">
            <Card>
                <CardHeader>
                    <CardTitle>Function Catalog</CardTitle>
                    <CardDescription>
                        Actionable automation and workflow entry points for the Banana platform.
                    </CardDescription>
                </CardHeader>
                <CardContent className="space-y-2 text-sm">
                    <div className="flex flex-wrap items-center gap-2">
                        <span className="rounded-full border px-2 py-1 text-xs">health capability: {capabilities.health ? "available" : "unavailable"}</span>
                        <span className="rounded-full border px-2 py-1 text-xs">workbench capability: {capabilities.workbench ? "available" : "unavailable"}</span>
                        <Button size="sm" variant={dryRun ? "default" : "outline"} onClick={() => setDryRun((v) => !v)}>
                            Dry-run {dryRun ? "On" : "Off"}
                        </Button>
                    </div>
                </CardContent>
            </Card>

            {CATALOG.map(({ group, fns }) => (
                <div key={group} className="space-y-2">
                    <p className="text-xs font-semibold uppercase tracking-widest text-muted-foreground px-1">{group}</p>
                    <div className="grid gap-3">
                        {fns.map((fn) => {
                            const blockedByCapability = fn.requiresWorkbench && workbenchAvailable === false;
                            const effectiveStatus: FnStatus = blockedByCapability
                                ? "blocked"
                                : (states[fn.name] ?? fn.status);
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

            {structuredError && (
                <Card>
                    <CardHeader>
                        <CardTitle className="text-base">Structured Error Panel</CardTitle>
                    </CardHeader>
                    <CardContent className="space-y-1 text-sm">
                        <p><span className="text-muted-foreground">code</span> {structuredError.code}</p>
                        <p><span className="text-muted-foreground">source</span> {structuredError.source}</p>
                        <p><span className="text-muted-foreground">message</span> {structuredError.message}</p>
                        <p><span className="text-muted-foreground">remediation</span> {structuredError.remediation}</p>
                    </CardContent>
                </Card>
            )}

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Execution History</CardTitle>
                    <CardDescription>Persisted local execution log for recent function actions.</CardDescription>
                </CardHeader>
                <CardContent className="space-y-2 text-xs">
                    {executionHistory.length === 0 ? (
                        <p className="text-muted-foreground">No execution history yet.</p>
                    ) : executionHistory.map((entry, idx) => (
                        <div key={`${entry.name}-${entry.at}-${idx}`} className="rounded border p-2">
                            <p className="font-medium">{entry.name}</p>
                            <p>{entry.status} · {entry.at}</p>
                            <p className="text-muted-foreground whitespace-pre-wrap">{entry.output}</p>
                        </div>
                    ))}
                </CardContent>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Release & Rollback Control Plane</CardTitle>
                    <CardDescription>Checklist gate, canary health, rollback playbook, drift summary, and verification matrix.</CardDescription>
                </CardHeader>
                <CardContent className="space-y-3 text-sm">
                    <div className="space-y-1">
                        <p className="text-xs uppercase tracking-widest text-muted-foreground">Release Checklist</p>
                        <label className="flex items-center gap-2"><input type="checkbox" checked={releaseChecklist.docs} onChange={(e) => setReleaseChecklist((v) => ({ ...v, docs: e.target.checked }))} /> docs updated</label>
                        <label className="flex items-center gap-2"><input type="checkbox" checked={releaseChecklist.tests} onChange={(e) => setReleaseChecklist((v) => ({ ...v, tests: e.target.checked }))} /> tests passing</label>
                        <label className="flex items-center gap-2"><input type="checkbox" checked={releaseChecklist.approvals} onChange={(e) => setReleaseChecklist((v) => ({ ...v, approvals: e.target.checked }))} /> approvals complete</label>
                    </div>

                    <div className="flex flex-wrap items-center gap-2">
                        <Button size="sm" variant="outline" onClick={checkCanaryHealth}>Check Canary Health</Button>
                        <span className={`rounded-full border px-2 py-1 text-xs ${canaryHealthy ? "border-green-500 text-green-600" : "border-red-500 text-red-600"}`}>
                            canary {canaryHealthy ? "healthy" : "unhealthy"}
                        </span>
                        <span className={`rounded-full border px-2 py-1 text-xs ${releaseReady ? "border-green-500 text-green-600" : "border-yellow-500 text-yellow-600"}`}>
                            promotion gate {releaseReady ? "pass" : "blocked"}
                        </span>
                    </div>

                    <div className="space-y-1">
                        <p className="text-xs uppercase tracking-widest text-muted-foreground">Environment Drift Summary</p>
                        <div className="rounded border p-2 text-xs">
                            <p>VITE_BANANA_API_BASE_URL: expected https://api.banana.engineer</p>
                            <p>BANANA_NATIVE_PATH: required for ASP.NET local runtime</p>
                            <p>BANANA_PG_CONNECTION: required for PostgreSQL-backed flows</p>
                        </div>
                    </div>

                    <div className="space-y-1">
                        <Button size="sm" onClick={runRollbackPlaybook}>Run Rollback Playbook Action</Button>
                        {rollbackLog.length > 0 && (
                            <ol className="list-decimal pl-5 text-xs space-y-1">
                                {rollbackLog.map((step) => <li key={step}>{step}</li>)}
                            </ol>
                        )}
                    </div>

                    <div className="space-y-1">
                        <p className="text-xs uppercase tracking-widest text-muted-foreground">Post-Release Verification Matrix</p>
                        <div className="grid gap-1 text-xs">
                            <div className="rounded border p-2">API health endpoint responds 200: {capabilities.health ? "yes" : "no"}</div>
                            <div className="rounded border p-2">Workbench endpoints available: {capabilities.workbench ? "yes" : "no"}</div>
                            <div className="rounded border p-2">Release gate ready: {releaseReady ? "yes" : "no"}</div>
                        </div>
                    </div>
                </CardContent>
            </Card>
        </div>
    );
}
