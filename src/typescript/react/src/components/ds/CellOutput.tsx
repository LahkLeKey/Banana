import { CONTRACT_PILL_TONE, CONTRACT_TONE_DARK } from "../../lib/dsConstants";
import type { NotebookRichOutput } from "../../lib/dsTypes";
import { NotebookSurface } from "./NotebookSurface";
import { PlotlyChart } from "./PlotlyChart";

type CellOutputProps = {
    output?: string;
    error?: string;
    richOutput?: NotebookRichOutput[];
    execCount?: number;
};

function RichArtifact({ out, index }: { out: NotebookRichOutput; index: number }) {
    if (out.kind === "plotly") {
        return (
            <div key={`plotly-${index}`} className="border-b border-zinc-800 p-3">
                <PlotlyChart figure={out.figure} />
            </div>
        );
    }

    if (out.kind === "image") {
        const src = out.data.startsWith("data:")
            ? out.data
            : `data:${out.mimeType};base64,${out.data}`;
        return (
            <div key={`image-${index}`} className="border-b border-zinc-800 p-4">
                <img
                    src={src}
                    alt={out.alt ?? "notebook output"}
                    className="max-h-96 max-w-full rounded object-contain"
                    loading="lazy"
                />
            </div>
        );
    }

    if (out.kind === "table") {
        return (
            <div key={`table-${index}`} className="max-h-72 overflow-auto border-b border-zinc-800 p-4">
                <table className="min-w-full border-collapse text-xs">
                    <thead className="sticky top-0 bg-zinc-900">
                        <tr>
                            {out.table.columns.map((c) => (
                                <th key={c} className="border border-zinc-700 px-2 py-1 text-left font-semibold text-zinc-300">{c}</th>
                            ))}
                        </tr>
                    </thead>
                    <tbody>
                        {out.table.rows.map((row, ridx) => (
                            <tr key={ridx} className="even:bg-zinc-900">
                                {row.map((val, cidx) => (
                                    <td key={cidx} className="border border-zinc-800 px-2 py-1 text-zinc-300">{String(val ?? "")}</td>
                                ))}
                            </tr>
                        ))}
                    </tbody>
                </table>
            </div>
        );
    }

    if (out.kind === "surface") {
        return (
            <div key={`surface-${index}`} className="border-b border-zinc-800 p-4">
                <NotebookSurface surface={out} />
            </div>
        );
    }

    if (out.kind === "error") {
        return (
            <div key={`error-${index}`} className="border-b border-zinc-800 bg-red-950/30 px-4 py-3">
                <div className="mb-1 flex items-center gap-2">
                    <span className="rounded bg-red-900/60 px-1.5 py-0.5 font-mono text-[11px] font-semibold text-red-300">{out.ename}</span>
                    <span className="text-xs text-red-400">{out.evalue}</span>
                </div>
                <pre className="max-h-48 overflow-auto whitespace-pre-wrap font-mono text-[11px] leading-5 text-red-400/80">{out.traceback}</pre>
            </div>
        );
    }

    if (out.kind === "contract") {
        return (
            <div key={`contract-${index}`} className="border-b border-zinc-800 px-4 py-3">
                <div className={`rounded border px-3 py-3 ${CONTRACT_TONE_DARK[out.status]}`}>
                    <div className="flex flex-wrap items-center justify-between gap-2">
                        <div>
                            <p className="text-[11px] font-semibold uppercase tracking-wider opacity-70">{out.family}</p>
                            <p className="mt-0.5 text-sm font-semibold">{out.title}</p>
                        </div>
                        <span className={`rounded border px-2 py-0.5 text-[10px] font-semibold uppercase tracking-wider ${CONTRACT_PILL_TONE[out.status]}`}>
                            {out.status}
                        </span>
                    </div>
                    <p className="mt-2 text-xs leading-5">{out.summary}</p>
                    {out.detail && <p className="mt-1 text-xs opacity-70">{out.detail}</p>}
                    {out.payloadText && (
                        <details className="mt-3 rounded border border-white/10 bg-black/10">
                            <summary className="cursor-pointer list-none px-2 py-1.5 text-[11px] font-semibold uppercase tracking-wider opacity-80">
                                Inspect Payload
                            </summary>
                            <pre className="max-h-56 overflow-auto border-t border-white/10 px-2 py-2 font-mono text-[11px] leading-5 opacity-90">{out.payloadText}</pre>
                        </details>
                    )}
                    <p className="mt-2 font-mono text-[10px] opacity-50">{out.marker}</p>
                </div>
            </div>
        );
    }

    // notice (info/warning)
    return (
        <div key={`notice-${index}`} className={`border-b border-zinc-800 px-4 py-2 font-mono text-xs ${out.level === "warning" ? "text-amber-300" : "text-zinc-400"}`}>
            {out.message}
        </div>
    );
}

export function CellOutput({ output, error, richOutput, execCount }: CellOutputProps) {
    const hasContent = output || error || (richOutput && richOutput.length > 0);
    if (!hasContent) return null;

    return (
        <div className="border-t border-zinc-800 bg-zinc-950 text-white">
            <div className="flex items-center justify-between border-b border-zinc-800 px-4 py-2">
                <span className="font-mono text-[11px] text-zinc-500">
                    Out [{execCount ?? " "}]:
                </span>
                {richOutput && richOutput.length > 0 && (
                    <span className="text-[11px] text-zinc-500">{richOutput.length} artifact{richOutput.length !== 1 ? "s" : ""}</span>
                )}
            </div>

            {richOutput?.map((out, i) => <RichArtifact key={i} out={out} index={i} />)}

            {output && (
                <pre className="max-h-64 overflow-auto whitespace-pre-wrap px-4 py-3 font-mono text-xs text-zinc-300">{output}</pre>
            )}
            {error && (
                <pre className="max-h-48 overflow-auto whitespace-pre-wrap bg-red-950/40 px-4 py-3 font-mono text-xs text-red-400">{error}</pre>
            )}
        </div>
    );
}
