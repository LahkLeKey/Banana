import { Button } from "../ui/button";
import type { RuntimeState } from "../../lib/dsTypes";

type NotebookToolbarProps = {
    notebookLabel: string;
    cellCount: number;
    executedCount: number;
    runtimeState: RuntimeState;
    runtimeMessage: string;
    showLibrary: boolean;
    onRunAll: () => void;
    onSave: () => void;
    onExport: () => void;
    onReset: () => void;
    onToggleCatalog: () => void;
    onWarmRuntime: () => void;
};

const RUNTIME_BADGE_CLASS: Record<RuntimeState, string> = {
    ready: "border-emerald-300 bg-emerald-50 text-emerald-700",
    loading: "border-amber-300 bg-amber-50 text-amber-700",
    error: "border-red-300 bg-red-50 text-red-700",
    idle: "border-slate-200 bg-slate-50 text-slate-500",
};

const RUNTIME_LABEL: Record<RuntimeState, string> = {
    ready: "● Runtime ready",
    loading: "◌ Loading…",
    error: "✕ Runtime error",
    idle: "○ Idle",
};

export function NotebookToolbar({
    notebookLabel,
    cellCount,
    executedCount,
    runtimeState,
    runtimeMessage,
    showLibrary,
    onRunAll,
    onSave,
    onExport,
    onReset,
    onToggleCatalog,
    onWarmRuntime,
}: NotebookToolbarProps) {
    return (
        <div className="flex h-10 items-center gap-2 rounded-lg border border-slate-200 bg-white px-3">
            {/* Catalog reveal toggle */}
            {!showLibrary && (
                <button
                    type="button"
                    onClick={onToggleCatalog}
                    title="Show notebook catalog"
                    className="rounded px-1 py-0.5 text-sm text-slate-400 hover:bg-slate-100 hover:text-slate-600"
                >
                    ›
                </button>
            )}

            {/* Notebook name + stats */}
            <span className="truncate text-sm font-semibold text-slate-800">{notebookLabel}</span>
            <span className="shrink-0 text-xs text-slate-400">
                {cellCount} cells · {executedCount} run
            </span>

            <div className="flex-1" />

            {/* Action buttons */}
            <Button size="sm" onClick={onRunAll} className="h-7 px-3 text-xs">
                ▶▶ Run All
            </Button>
            <Button size="sm" variant="outline" onClick={onSave} className="h-7 px-3 text-xs">
                Save
            </Button>
            <Button size="sm" variant="outline" onClick={onExport} className="h-7 px-3 text-xs">
                Export
            </Button>
            <Button size="sm" variant="ghost" onClick={onReset} className="h-7 px-3 text-xs text-slate-500">
                Reset
            </Button>

            {/* Divider */}
            <div className="h-4 w-px bg-slate-200" />

            {/* Runtime badge — click to warm when idle */}
            <button
                type="button"
                title={runtimeMessage}
                onClick={runtimeState === "idle" ? onWarmRuntime : undefined}
                className={`shrink-0 rounded border px-2 py-0.5 text-[11px] font-medium ${RUNTIME_BADGE_CLASS[runtimeState]} ${runtimeState === "idle" ? "cursor-pointer hover:opacity-80" : "cursor-default"
                    }`}
            >
                {RUNTIME_LABEL[runtimeState]}
            </button>
        </div>
    );
}
