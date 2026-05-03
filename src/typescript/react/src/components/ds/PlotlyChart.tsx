import { useEffect, useRef, useState } from "react";
import { ensurePlotly } from "../../lib/dsNotebook";
import type { PlotlyFigure } from "../../lib/dsTypes";

export function PlotlyChart({ figure }: { figure: PlotlyFigure }) {
    const divRef = useRef<HTMLDivElement>(null);
    const hasRenderedRef = useRef(false);
    const [downloading, setDownloading] = useState(false);

    useEffect(() => {
        let cancelled = false;
        const el = divRef.current;
        if (!el) return;

        void ensurePlotly().then(() => {
            if (cancelled || !window.Plotly || !divRef.current) return;
            const layout: Record<string, unknown> = {
                margin: { t: 40, r: 16, b: 40, l: 48 },
                paper_bgcolor: "rgba(0,0,0,0)",
                plot_bgcolor: "rgba(0,0,0,0)",
                font: { size: 11 },
                ...(figure.layout ?? {}),
            };
            const method = hasRenderedRef.current ? window.Plotly.react : window.Plotly.newPlot;
            void method(divRef.current, figure.data as unknown[], layout, { responsive: true, displayModeBar: false }).then(() => {
                hasRenderedRef.current = true;
            });
        });

        return () => {
            cancelled = true;
            if (el && window.Plotly) window.Plotly.purge(el);
            hasRenderedRef.current = false;
        };
        // eslint-disable-next-line react-hooks/exhaustive-deps
    }, [figure]);

    function handleDownload() {
        const el = divRef.current;
        if (!el || !window.Plotly) return;
        setDownloading(true);
        void window.Plotly.downloadImage(el, { format: "png", filename: "banana-chart", width: 1200, height: 600 })
            .finally(() => setDownloading(false));
    }

    return (
        <div className="group relative">
            <div ref={divRef} className="h-72 w-full" />
            <button
                type="button"
                onClick={handleDownload}
                disabled={downloading}
                title="Download chart as PNG"
                className="absolute right-2 top-2 hidden rounded border border-zinc-700 bg-zinc-900/80 px-2 py-1 font-mono text-[10px] text-zinc-400 transition-colors hover:border-zinc-500 hover:text-zinc-200 group-hover:flex"
            >
                {downloading ? "…" : "↓ PNG"}
            </button>
        </div>
    );
}
