import { Button } from "../ui/button";
import { Textarea } from "../ui/textarea";
import { CONTRACT_PILL_TONE, CONTRACT_TONE_LIGHT } from "../../lib/dsConstants";
import type { RuntimeContractDefinition, RuntimeState } from "../../lib/dsTypes";

type OperationsRailProps = {
    runtimeState: RuntimeState;
    runtimeMessage: string;
    importText: string;
    onImportTextChange: (v: string) => void;
    onImport: () => void;
    contracts: RuntimeContractDefinition[];
};

const RUNTIME_VALUE_CLASS: Record<RuntimeState, string> = {
    ready: "text-emerald-600",
    loading: "text-amber-600",
    error: "text-red-600",
    idle: "text-slate-500",
};

const RUNTIME_STATUS_LABEL: Record<RuntimeState, string> = {
    ready: "Python runtime is warm",
    loading: "Runtime booting…",
    error: "Runtime needs attention",
    idle: "Runtime idle — click Run All to start",
};

function RailSection({ label, children }: { label: string; children: React.ReactNode }) {
    return (
        <section className="border-b border-slate-100 px-4 py-3 last:border-0">
            <p className="mb-2 text-[10px] font-semibold uppercase tracking-wider text-slate-400">{label}</p>
            {children}
        </section>
    );
}

export function OperationsRail({ runtimeState, runtimeMessage, importText, onImportTextChange, onImport, contracts }: OperationsRailProps) {
    return (
        <aside className="w-full min-w-0 rounded-xl border border-slate-200 bg-white">
            <div className="border-b border-slate-200 px-4 py-2">
                <span className="text-[11px] font-semibold uppercase tracking-wider text-slate-500">Operations</span>
            </div>

            <div className="grid gap-3 p-3 lg:grid-cols-2">
                {/* Runtime status */}
                <div className="rounded-lg border border-slate-200 bg-slate-50">
                    <RailSection label="Runtime">
                        <p className={`text-sm font-semibold ${RUNTIME_VALUE_CLASS[runtimeState]}`}>
                            {RUNTIME_STATUS_LABEL[runtimeState]}
                        </p>
                        <p className="mt-0.5 text-[11px] leading-4 text-slate-400">{runtimeMessage}</p>
                    </RailSection>
                </div>

                {/* Import */}
                <div className="rounded-lg border border-slate-200 bg-slate-50">
                    <RailSection label="Import Notebook">
                        <Textarea
                            rows={4}
                            placeholder="Paste .ipynb JSON…"
                            value={importText}
                            onChange={(e) => onImportTextChange(e.target.value)}
                            className="font-mono text-xs"
                        />
                        <Button size="sm" className="mt-2 w-full" onClick={onImport}>
                            Import
                        </Button>
                    </RailSection>
                </div>

                {/* Working rules */}
                <div className="rounded-lg border border-slate-200 bg-slate-50">
                    <RailSection label="Working Rules">
                        <ul className="space-y-1.5 text-xs leading-5 text-slate-600">
                            <li>· Run notebooks as reusable workflows, not loose snippets.</li>
                            <li>· Keep markdown in preview so each notebook reads like a report.</li>
                            <li>· Use the cell stack to navigate — only the active cell expands.</li>
                            <li>· Export once a notebook is ready for reuse.</li>
                        </ul>
                    </RailSection>
                </div>

                {/* Runtime contract map */}
                <div className="rounded-lg border border-slate-200 bg-slate-50 lg:col-span-2">
                    <RailSection label="Runtime Contract Map">
                        <div className="grid gap-1.5 md:grid-cols-2 xl:grid-cols-3">
                            {contracts.map((c) => (
                                <div key={c.marker} className={`rounded border px-2.5 py-2 ${CONTRACT_TONE_LIGHT[c.status]}`}>
                                    <div className="flex items-start justify-between gap-1">
                                        <p className="text-xs font-semibold">{c.title}</p>
                                        <span className={`mt-0.5 shrink-0 rounded border px-1.5 py-0.5 text-[10px] font-semibold uppercase tracking-wider ${CONTRACT_PILL_TONE[c.status]}`}>
                                            {c.status}
                                        </span>
                                    </div>
                                    <p className="mt-0.5 text-[11px] leading-4 opacity-80">{c.summary}</p>
                                    <p className="mt-1 font-mono text-[10px] opacity-50">{c.marker}</p>
                                </div>
                            ))}
                        </div>
                    </RailSection>
                </div>
            </div>
        </aside>
    );
}
