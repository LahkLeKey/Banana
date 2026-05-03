import { Button } from "../ui/button";
import type { NotebookLibraryEntry } from "../../lib/dsTypes";

type NotebookCatalogProps = {
    activeNotebookLabel: string;
    library: NotebookLibraryEntry[];
    onLoadNotebook: (entry: NotebookLibraryEntry) => void;
    onClose: () => void;
    onAddPythonCell: () => void;
    onAddMarkdownCell: () => void;
    onToggleAdvancedTools: () => void;
    showAdvancedTools: boolean;
};

export function NotebookCatalog({
    activeNotebookLabel,
    library,
    onLoadNotebook,
    onClose,
    onAddPythonCell,
    onAddMarkdownCell,
    onToggleAdvancedTools,
    showAdvancedTools,
}: NotebookCatalogProps) {
    return (
        <aside className="flex w-full min-w-0 flex-col rounded-lg border border-slate-200 bg-white">
            {/* Header */}
            <div className="flex items-center justify-between border-b border-slate-200 px-3 py-2">
                <span className="text-[11px] font-semibold uppercase tracking-wider text-slate-500">Notebooks</span>
                <button
                    type="button"
                    onClick={onClose}
                    title="Close catalog"
                    className="rounded p-0.5 text-slate-400 hover:bg-slate-100 hover:text-slate-600"
                >
                    ‹
                </button>
            </div>

            {/* Library list */}
            <div className="max-h-44 overflow-y-auto py-1">
                {library.map((entry) => {
                    const isActive = activeNotebookLabel === entry.label;
                    return (
                        <button
                            key={entry.path}
                            type="button"
                            onClick={() => onLoadNotebook(entry)}
                            className={`w-full px-3 py-2 text-left transition-colors ${isActive ? "bg-blue-50 text-blue-700" : "text-slate-700 hover:bg-slate-50"
                                }`}
                        >
                            <span className={`block truncate text-xs ${isActive ? "font-semibold" : "font-medium"}`}>
                                {entry.label}
                            </span>
                            <span className="mt-0.5 block truncate text-[11px] text-slate-400">{entry.description}</span>
                        </button>
                    );
                })}
            </div>

            {/* Quick actions footer */}
            <div className="space-y-1 border-t border-slate-200 p-2">
                <p className="px-1 pb-0.5 text-[10px] font-semibold uppercase tracking-wider text-slate-400">Add Cell</p>
                <Button data-testid="add-python-cell" size="sm" className="h-7 w-full justify-start text-xs" onClick={onAddPythonCell}>
                    + Python
                </Button>
                <Button size="sm" variant="outline" className="h-7 w-full justify-start text-xs" onClick={onAddMarkdownCell}>
                    + Markdown
                </Button>
                <Button
                    size="sm"
                    variant={showAdvancedTools ? "secondary" : "ghost"}
                    className="h-7 w-full justify-start text-xs"
                    onClick={onToggleAdvancedTools}
                >
                    {showAdvancedTools ? "Hide Tools" : "Advanced Tools"}
                </Button>
            </div>
        </aside>
    );
}
