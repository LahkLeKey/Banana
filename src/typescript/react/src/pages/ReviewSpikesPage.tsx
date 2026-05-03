import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";

type SpikeStatus = "defined" | "in-progress" | "complete" | "new";

type Spike = {
    id: string;
    title: string;
    focus: string;
    status: SpikeStatus;
    outcome?: string;
};

const STATUS_STYLES: Record<SpikeStatus, string> = {
    defined: "border-border text-muted-foreground",
    "in-progress": "border-yellow-500 text-yellow-600 dark:text-yellow-400",
    complete: "border-green-500 text-green-600 dark:text-green-400",
    new: "border-blue-500 text-blue-600 dark:text-blue-400",
};

const SUITE_SPIKES: Spike[] = [
    {
        id: "161",
        title: "Suite Information Architecture",
        focus: "Define spaces, navigation taxonomy, and role-based wayfinding.",
        status: "complete",
        outcome: "WorkspaceShell with sidebar nav, 6 routed pages.",
    },
    {
        id: "162",
        title: "Shadcn Confluence Workbench",
        focus: "Validate article layout patterns, activity stream, and collaborative editing affordances.",
        status: "in-progress",
        outcome: "KnowledgePage 3-col scaffold; activity feed wired to training history API.",
    },
    {
        id: "163",
        title: "Function Web App Capability",
        focus: "Map function registry, execution guardrails, and auditable run outcomes.",
        status: "complete",
        outcome: "FunctionsPage with live health check, banana summary, and model ops actions.",
    },
    {
        id: "164",
        title: "BananaAI Rovo Replacement",
        focus: "Define orchestration contract to combine custom models behind a unified BananaAI assistant.",
        status: "complete",
        outcome: "BananaAIPage wired to ensemble verdict + embedding; Operator wired to ChatController.",
    },
    {
        id: "165",
        title: "Feedback Loop Promotion",
        focus: "Define memory-first promotion loop from runtime/spec signals into prioritized follow-up slices.",
        status: "in-progress",
        outcome: "Promotion API available; governance thresholds spike (170) queued.",
    },
];

const DS_SPIKES: Spike[] = [
    {
        id: "166",
        title: "Telemetry Reduction via Semantic Thresholds",
        focus: "Replace high-noise signal fans with semantic confidence bands. Define per-lane calibration thresholds that suppress redundant escalations and route only high-uncertainty samples to human review.",
        status: "new",
        outcome: "Target: ≥40% reduction in escalated-but-correct verdicts without precision loss.",
    },
    {
        id: "167",
        title: "Workbench Evaluation Harness",
        focus: "Wire runTrainingWorkbench + fetchTrainingWorkbenchHistory into a repeatable eval loop. Surface per-run metrics (accuracy, F1, drift delta) in the Functions Catalog so training outcomes are operator-visible without leaving the suite.",
        status: "new",
        outcome: "Metric cards per lane; pass/fail threshold badge; history sparkline.",
    },
    {
        id: "168",
        title: "Ripeness Signal Pipeline",
        focus: "Formalize predictRipeness as a first-class pipeline stage. Histogram ripeness score distribution across corpus sessions, tie outlier bins into ensemble escalation, and expose the distribution in BananaAI output.",
        status: "new",
        outcome: "Ripeness histogram component; escalation tie-in contract; per-sample audit row.",
    },
    {
        id: "169",
        title: "Feature Importance & Embedding Drift Surface",
        focus: "Expose corpus-level feature importance rankings and embedding centroid drift as operator-visible signals. Use existing embedding arrays from fetchEnsembleVerdictWithEmbedding to compute cosine drift per lane across runs.",
        status: "new",
        outcome: "Drift delta card in KnowledgePage activity; top-N feature table in BananaAI.",
    },
    {
        id: "170",
        title: "Promotion Governance & Rollback Triggers",
        focus: "Define promotion thresholds, audit trail surfacing, and rollback triggers for promoteTrainingWorkbenchRun. Operator must confirm threshold_passed before stable promotion; failed threshold blocks via UI guard, not API bypass.",
        status: "new",
        outcome: "Promotion confirmation dialog; audit event timeline; rollback action in Functions Catalog.",
    },
];

const ENHANCEMENT_SPIKES: Spike[] = [
    {
        id: "171",
        title: "Operator Command Center UX",
        focus: "Enhancements: (1) pinned recent sessions, (2) message search + filter, (3) token usage meter, (4) retry failed turn button, (5) quick command chips for common operator actions.",
        status: "new",
        outcome: "Operator page becomes a high-throughput command center with faster session recovery and less manual typing.",
    },
    {
        id: "172",
        title: "Knowledge Reliability Upgrade",
        focus: "Enhancements: (1) lane health badges, (2) history sparkline per lane, (3) drill-in run details drawer, (4) stale-data warning banner, (5) copy-export of visible metrics.",
        status: "new",
        outcome: "Knowledge view shifts from static dashboard to reliable analysis workspace with quick diagnostics.",
    },
    {
        id: "173",
        title: "Functions Automation Safety Layer",
        focus: "Enhancements: (1) capability discovery endpoint check, (2) per-action preflight validation, (3) dry-run mode toggle, (4) structured error panel with remediation, (5) execution history persisted in local storage.",
        status: "new",
        outcome: "Function runs become safer and self-explanatory, reducing failed attempts and operator confusion.",
    },
    {
        id: "174",
        title: "BananaAI Explainability Pack",
        focus: "Enhancements: (1) top feature attribution list, (2) confidence calibration hint, (3) ensemble lane contribution chart, (4) out-of-domain trigger reasons, (5) one-click copy of structured verdict JSON.",
        status: "new",
        outcome: "BananaAI responses are transparent and auditable for both DS and operator users.",
    },
    {
        id: "175",
        title: "Release & Rollback Control Plane",
        focus: "Enhancements: (1) release checklist card, (2) canary health gate, (3) rollback playbook action, (4) environment drift summary, (5) post-release verification matrix.",
        status: "new",
        outcome: "Delivery flow becomes predictable with explicit gates before promotion and faster rollback confidence.",
    },
];

function SpikeCard({ spike }: { spike: Spike }) {
    return (
        <Card>
            <CardHeader className="pb-2">
                <div className="flex items-start justify-between gap-2">
                    <CardTitle className="text-base">{spike.id} — {spike.title}</CardTitle>
                    <span className={`shrink-0 rounded-full border px-2 py-0.5 text-xs font-medium ${STATUS_STYLES[spike.status]}`}>
                        {spike.status}
                    </span>
                </div>
            </CardHeader>
            <CardContent className="space-y-1">
                <p className="text-sm text-muted-foreground">{spike.focus}</p>
                {spike.outcome && (
                    <p className="text-xs text-foreground/60 border-l-2 border-border pl-2">{spike.outcome}</p>
                )}
            </CardContent>
        </Card>
    );
}

export function ReviewSpikesPage() {
    return (
        <div className="space-y-6">
            <Card>
                <CardHeader>
                    <CardTitle>Review Spikes</CardTitle>
                    <CardDescription>
                        Scoped research slices. Suite architecture (161–165), data science scale-up (166–170), and enhancement program (171–175).
                    </CardDescription>
                </CardHeader>
            </Card>

            <div className="space-y-2">
                <p className="text-xs font-semibold uppercase tracking-widest text-muted-foreground px-1">Suite Architecture</p>
                <div className="grid gap-3">
                    {SUITE_SPIKES.map((s) => <SpikeCard key={s.id} spike={s} />)}
                </div>
            </div>

            <div className="space-y-2">
                <p className="text-xs font-semibold uppercase tracking-widest text-muted-foreground px-1">Data Science Scale-Up</p>
                <div className="grid gap-3">
                    {DS_SPIKES.map((s) => <SpikeCard key={s.id} spike={s} />)}
                </div>
            </div>

            <div className="space-y-2">
                <p className="text-xs font-semibold uppercase tracking-widest text-muted-foreground px-1">Enhancement Program</p>
                <div className="grid gap-3">
                    {ENHANCEMENT_SPIKES.map((s) => <SpikeCard key={s.id} spike={s} />)}
                </div>
            </div>
        </div>
    );
}
