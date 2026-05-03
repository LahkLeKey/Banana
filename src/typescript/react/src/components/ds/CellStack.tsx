import { summarizeSource } from "../../lib/dsNotebook";
import type { NotebookCell } from "../../lib/dsTypes";

type CellStackProps = {
    cells: NotebookCell[];
    activeCellId: number | null;
    onSelectCell: (id: number) => void;
};

export function CellStack({ cells, activeCellId, onSelectCell }: CellStackProps) {
    if (cells.length === 0) {
        return (
            <div className="flex min-h-[100px] items-center justify-center text-xs text-slate-400">
                No cells yet.
            </div>
        );
    }

    return (
        <div className="divide-y divide-slate-100">
            {cells.map((cell, index) => {
                const isActive = cell.id === activeCellId;
                return (
                    <button
                        key={cell.id}
                        type="button"
                        onClick={() => onSelectCell(cell.id)}
                        className={`group flex w-full items-stretch gap-0 text-left transition-colors ${isActive ? "bg-blue-50" : "bg-white hover:bg-slate-50"
                            }`}
                    >
                        {/* Jupyter-style left gutter with execution count */}
                        <div
                            className={`flex w-12 flex-none items-start justify-end py-2.5 pr-2.5 font-mono text-[11px] tabular-nums transition-colors ${isActive
                                    ? "border-l-[3px] border-l-blue-500 text-blue-500"
                                    : "border-l-[3px] border-l-transparent text-slate-400 group-hover:border-l-slate-300"
                                }`}
                        >
                            {cell.kind === "python" ? (cell.execCount != null ? `[${cell.execCount}]` : "[ ]") : "M"}
                        </div>

                        {/* Cell summary */}
                        <div className="min-w-0 flex-1 py-2 pr-3">
                            <div className="flex items-center gap-1.5">
                                <span className={`text-[11px] font-medium ${isActive ? "text-blue-700" : "text-slate-500"}`}>
                                    {index + 1}
                                </span>
                                <span className={`rounded px-1 text-[10px] ${isActive ? "bg-blue-100 text-blue-600" : "bg-slate-100 text-slate-500"}`}>
                                    {cell.kind}
                                </span>
                                {cell.running && (
                                    <span className="animate-pulse rounded bg-amber-100 px-1 text-[10px] font-medium text-amber-600">•</span>
                                )}
                                {cell.execCount != null && !cell.running && (
                                    <span className="text-[10px] text-emerald-600">✓</span>
                                )}
                            </div>
                            <p className={`mt-0.5 truncate font-mono text-[11px] ${isActive ? "text-slate-700" : "text-slate-500"}`}>
                                {summarizeSource(cell.source) || "Empty cell"}
                            </p>
                        </div>
                    </button>
                );
            })}
        </div>
    );
}
