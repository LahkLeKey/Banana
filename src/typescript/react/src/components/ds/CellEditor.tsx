import { Button } from "../ui/button";
import { Textarea } from "../ui/textarea";
import { renderMarkdown } from "../../lib/dsNotebook";
import type { NotebookCell } from "../../lib/dsTypes";
import { CellOutput } from "./CellOutput";

type CellEditorProps = {
    cell: NotebookCell;
    cellIndex: number;
    isActive?: boolean;
    onActivate?: () => void;
    onUpdateSource: (source: string) => void;
    onRun: () => void;
    onDelete: () => void;
    onTogglePreview: () => void;
    onMoveUp?: () => void;
    onMoveDown?: () => void;
    onDuplicate?: () => void;
};

export function CellEditor({ cell, cellIndex, isActive = true, onActivate, onUpdateSource, onRun, onDelete, onTogglePreview, onMoveUp, onMoveDown, onDuplicate }: CellEditorProps) {
    const lineCount = cell.source.split("\n").length;
    const isMarkdown = cell.kind === "markdown";
    const showPreview = isMarkdown && (cell.previewMode ?? true);

    return (
        <div
            className={`overflow-hidden rounded-lg border bg-white transition-shadow ${isActive ? "border-blue-300 shadow-sm" : "border-slate-200 opacity-80"}`}
            onClick={!isActive ? onActivate : undefined}
            style={!isActive ? { cursor: "pointer" } : undefined}
        >
            {/* Cell header bar */}
            <div className="flex items-center justify-between gap-3 border-b border-slate-100 bg-slate-50 px-3 py-2">
                <div className="flex items-center gap-2.5">
                    <span className="font-mono text-[11px] text-slate-400 tabular-nums">
                        {isMarkdown ? "Md" : `In [${cell.execCount ?? " "}]:`}
                    </span>
                    <span className="rounded bg-slate-200 px-1.5 py-0.5 text-[10px] font-semibold text-slate-600">
                        Cell {cellIndex + 1}
                    </span>
                    {cell.running && (
                        <span className="animate-pulse rounded bg-amber-100 px-1.5 py-0.5 text-[10px] font-semibold text-amber-700">
                            Running…
                        </span>
                    )}
                </div>
                <div className="flex items-center gap-1">
                    {onMoveUp && (
                        <Button
                            size="sm"
                            variant="ghost"
                            onClick={(e) => { e.stopPropagation(); onMoveUp(); }}
                            className="h-7 px-2 text-xs text-slate-400 hover:text-slate-700"
                            title="Move cell up"
                        >
                            ↑
                        </Button>
                    )}
                    {onMoveDown && (
                        <Button
                            size="sm"
                            variant="ghost"
                            onClick={(e) => { e.stopPropagation(); onMoveDown(); }}
                            className="h-7 px-2 text-xs text-slate-400 hover:text-slate-700"
                            title="Move cell down"
                        >
                            ↓
                        </Button>
                    )}
                    {onDuplicate && (
                        <Button
                            size="sm"
                            variant="ghost"
                            onClick={(e) => { e.stopPropagation(); onDuplicate(); }}
                            className="h-7 px-2 text-xs text-slate-400 hover:text-slate-700"
                            title="Duplicate cell"
                        >
                            ⎘
                        </Button>
                    )}
                    {!isMarkdown && (
                        <Button
                            size="sm"
                            onClick={(e) => { e.stopPropagation(); onRun(); }}
                            disabled={!!cell.running}
                            className="h-7 px-3 text-xs"
                            data-testid="cell-run-btn"
                        >
                            ▶ Run
                        </Button>
                    )}
                    {isMarkdown && (
                        <Button
                            size="sm"
                            variant="outline"
                            onClick={(e) => { e.stopPropagation(); onTogglePreview(); }}
                            className="h-7 px-3 text-xs"
                        >
                            {showPreview ? "Edit" : "Preview"}
                        </Button>
                    )}
                    <Button
                        size="sm"
                        variant="ghost"
                        onClick={(e) => { e.stopPropagation(); onDelete(); }}
                        className="h-7 px-2 text-xs text-slate-400 hover:text-destructive"
                    >
                        ✕
                    </Button>
                </div>
            </div>

            {/* Editor or markdown preview */}
            {showPreview ? (
                <div
                    className="prose prose-sm max-w-none cursor-pointer px-5 py-4 text-sm"
                    onClick={onTogglePreview}
                    // eslint-disable-next-line react/no-danger
                    dangerouslySetInnerHTML={{
                        __html: cell.source
                            ? renderMarkdown(cell.source)
                            : '<span class="text-slate-400 text-xs italic">Click to edit…</span>',
                    }}
                />
            ) : (
                <Textarea
                    className="min-h-[200px] rounded-none border-0 bg-white px-5 py-4 font-mono text-sm leading-6 focus-visible:ring-0"
                    rows={Math.max(8, lineCount + 2)}
                    value={cell.source}
                    onChange={(e) => onUpdateSource(e.target.value)}
                    onBlur={() => isMarkdown && onTogglePreview()}
                    placeholder={isMarkdown ? "## Notes" : "# Python code"}
                    data-testid="cell-code-editor"
                />
            )}

            {/* Output zone — dark terminal, Jupyter convention */}
            {!isMarkdown && (
                <CellOutput
                    output={cell.output}
                    error={cell.error}
                    richOutput={cell.richOutput}
                    execCount={cell.execCount}
                />
            )}
        </div>
    );
}
