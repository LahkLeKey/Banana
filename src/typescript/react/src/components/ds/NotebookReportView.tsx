import { renderMarkdown } from "../../lib/dsNotebook";
import type { NotebookCell } from "../../lib/dsTypes";
import { CellOutput } from "./CellOutput";

type NotebookReportViewProps = {
    cells: NotebookCell[];
};

export function NotebookReportView({ cells }: NotebookReportViewProps) {
    if (cells.length === 0) {
        return (
            <div className="flex min-h-[280px] items-center justify-center text-sm text-slate-400">
                No cells to display in report view.
            </div>
        );
    }

    return (
        <div className="divide-y divide-slate-100 rounded-lg border border-slate-200 bg-white">
            {cells.map((cell) => (
                <div key={cell.id} className="group">
                    {cell.kind === "markdown" ? (
                        <div
                            className="prose prose-sm max-w-none px-6 py-5 text-sm"
                            // eslint-disable-next-line react/no-danger
                            dangerouslySetInnerHTML={{ __html: renderMarkdown(cell.source) }}
                        />
                    ) : (
                        <>
                            {/* Python source — collapsed by default, expand on hover */}
                            <details className="group/src">
                                <summary className="flex cursor-pointer list-none items-center gap-2 border-b border-slate-100 bg-slate-50 px-4 py-1.5 text-[11px] text-slate-400 hover:text-slate-600">
                                    <span className="font-mono">
                                        In [{cell.execCount ?? " "}]:
                                    </span>
                                    <span className="truncate font-mono text-[11px] text-slate-500">
                                        {cell.source.split("\n")[0]}
                                    </span>
                                    <span className="ml-auto shrink-0 text-[10px] opacity-50 group-open/src:hidden">
                                        show source
                                    </span>
                                </summary>
                                <pre className="overflow-auto bg-slate-950 px-5 py-3 font-mono text-xs leading-5 text-zinc-300">
                                    {cell.source}
                                </pre>
                            </details>

                            <CellOutput
                                output={cell.output}
                                error={cell.error}
                                richOutput={cell.richOutput}
                                execCount={cell.execCount}
                            />
                        </>
                    )}
                </div>
            ))}
        </div>
    );
}
