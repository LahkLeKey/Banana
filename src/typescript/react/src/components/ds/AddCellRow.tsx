type AddCellRowProps = {
    onAddPython: () => void;
    onAddMarkdown: () => void;
};

export function AddCellRow({ onAddPython, onAddMarkdown }: AddCellRowProps) {
    return (
        <div className="flex items-center gap-2 pt-1">
            <button
                type="button"
                onClick={onAddPython}
                className="rounded border border-dashed border-slate-300 px-3 py-1.5 text-[11px] font-semibold text-slate-500 transition-colors hover:border-blue-300 hover:bg-blue-50 hover:text-blue-600"
            >
                + Python
            </button>
            <button
                type="button"
                onClick={onAddMarkdown}
                className="rounded border border-dashed border-slate-300 px-3 py-1.5 text-[11px] font-semibold text-slate-500 transition-colors hover:border-slate-400 hover:bg-slate-50"
            >
                + Markdown
            </button>
        </div>
    );
}
