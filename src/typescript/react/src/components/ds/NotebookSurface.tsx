import { useEffect, useMemo, useRef, useState } from "react";
import type { NotebookSurfaceOutput } from "../../lib/dsTypes";

type Point = { label: string; x: number; y: number; value?: number; color?: string };
type SeriesPoint = { x: string; y: number };

function coerceSeriesPoints(value: unknown): SeriesPoint[] {
    if (!Array.isArray(value)) return [];
    return value.flatMap((item, index) => {
        if (!item || typeof item !== "object") return [];
        const entry = item as Record<string, unknown>;
        const x = entry.x;
        const y = entry.y;
        if ((typeof x === "string" || typeof x === "number") && typeof y === "number") {
            return [{ x: String(x), y }];
        }
        return [{ x: `item-${index + 1}`, y: typeof y === "number" ? y : index + 1 }];
    });
}

function coercePoints(value: unknown): Point[] {
    if (!Array.isArray(value)) return [];
    return value.flatMap((item, index) => {
        if (!item || typeof item !== "object") return [];
        const entry = item as Record<string, unknown>;
        const label = typeof entry.label === "string" ? entry.label : `Point ${index + 1}`;
        const x = typeof entry.x === "number" ? entry.x : 20 + index * 15;
        const y = typeof entry.y === "number" ? entry.y : 20 + (index % 4) * 18;
        const valueNum = typeof entry.value === "number" ? entry.value : undefined;
        const color = typeof entry.color === "string" ? entry.color : undefined;
        return [{ label, x, y, value: valueNum, color }];
    });
}

function panelTone(family: NotebookSurfaceOutput["family"]): string {
    switch (family) {
        case "vega":
            return "border-cyan-700/40 bg-cyan-950/30";
        case "altair":
            return "border-emerald-700/40 bg-emerald-950/30";
        case "bokeh":
            return "border-fuchsia-700/40 bg-fuchsia-950/30";
        case "widgets":
        case "bqplot":
        case "ipympl":
            return "border-amber-700/40 bg-amber-950/20";
        case "geospatial":
            return "border-lime-700/40 bg-lime-950/20";
        case "network":
            return "border-sky-700/40 bg-sky-950/20";
        case "three-d":
            return "border-violet-700/40 bg-violet-950/20";
        case "canvas":
            return "border-rose-700/40 bg-rose-950/20";
        default:
            return "border-zinc-700/40 bg-zinc-950/20";
    }
}

function SurfaceShell({
    family,
    title,
    summary,
    controls,
    children,
}: {
    family: NotebookSurfaceOutput["family"];
    title: string;
    summary?: string;
    controls?: React.ReactNode;
    children: React.ReactNode;
}) {
    return (
        <div className={`rounded-lg border p-3 ${panelTone(family)}`}>
            <div className="mb-3 flex flex-wrap items-start justify-between gap-3">
                <div>
                    <p className="text-[11px] font-semibold uppercase tracking-wider text-zinc-400">{family}</p>
                    <p className="mt-0.5 text-sm font-semibold text-white">{title}</p>
                    {summary && <p className="mt-1 text-xs leading-5 text-zinc-300">{summary}</p>}
                </div>
                {controls}
            </div>
            {children}
        </div>
    );
}

function ChartSurface({
    family,
    title,
    summary,
    points,
    mode,
    accent,
}: {
    family: NotebookSurfaceOutput["family"];
    title: string;
    summary?: string;
    points: SeriesPoint[];
    mode: "bar" | "line" | "scatter";
    accent: string;
}) {
    const [hoverIndex, setHoverIndex] = useState<number | null>(null);
    const width = 560;
    const height = 220;
    const margin = { top: 16, right: 16, bottom: 42, left: 38 };
    const chartWidth = width - margin.left - margin.right;
    const chartHeight = height - margin.top - margin.bottom;
    const maxY = Math.max(1, ...points.map((point) => point.y));

    const coords = points.map((point, index) => {
        const x = margin.left + (points.length <= 1 ? chartWidth / 2 : (index / (points.length - 1)) * chartWidth);
        const y = margin.top + chartHeight - (point.y / maxY) * chartHeight;
        return { ...point, x, y };
    });

    const path = coords.map((point, index) => `${index === 0 ? "M" : "L"} ${point.x} ${point.y}`).join(" ");

    return (
        <SurfaceShell family={family} title={title} summary={summary}>
            <div className="space-y-3">
                <svg viewBox={`0 0 ${width} ${height}`} className="w-full rounded border border-white/10 bg-zinc-950/70">
                    <line x1={margin.left} y1={margin.top + chartHeight} x2={width - margin.right} y2={margin.top + chartHeight} stroke="rgba(255,255,255,0.2)" />
                    <line x1={margin.left} y1={margin.top} x2={margin.left} y2={margin.top + chartHeight} stroke="rgba(255,255,255,0.2)" />
                    {mode === "line" && <path d={path} fill="none" stroke={accent} strokeWidth="3" strokeLinejoin="round" strokeLinecap="round" />}
                    {mode === "bar" && coords.map((point, index) => {
                        const barWidth = chartWidth / Math.max(coords.length, 1) - 12;
                        return (
                            <g key={point.x} onMouseEnter={() => setHoverIndex(index)} onMouseLeave={() => setHoverIndex(null)}>
                                <rect x={point.x - barWidth / 2} y={point.y} width={Math.max(18, barWidth)} height={margin.top + chartHeight - point.y} rx="6" fill={hoverIndex === index ? "#f8fafc" : accent} opacity={hoverIndex === index ? 1 : 0.88} />
                            </g>
                        );
                    })}
                    {(mode === "line" || mode === "scatter") && coords.map((point, index) => (
                        <g key={point.x} onMouseEnter={() => setHoverIndex(index)} onMouseLeave={() => setHoverIndex(null)}>
                            <circle cx={point.x} cy={point.y} r={hoverIndex === index ? 6 : 4} fill={hoverIndex === index ? "#ffffff" : accent} />
                        </g>
                    ))}
                    {coords.map((point) => (
                        <text key={`${point.x}-label`} x={point.x} y={height - 16} textAnchor="middle" fill="rgba(255,255,255,0.6)" fontSize="11">{point.x}</text>
                    ))}
                </svg>
                <div className="grid gap-2 sm:grid-cols-3">
                    {coords.map((point, index) => (
                        <button key={`${point.x}-card`} type="button" onMouseEnter={() => setHoverIndex(index)} onMouseLeave={() => setHoverIndex(null)} className={`rounded border px-3 py-2 text-left text-xs transition-colors ${hoverIndex === index ? "border-white/40 bg-white/10 text-white" : "border-white/10 bg-black/10 text-zinc-300"}`}>
                            <span className="block font-semibold">{point.x}</span>
                            <span className="block opacity-70">{point.y.toFixed(2)}</span>
                        </button>
                    ))}
                </div>
            </div>
        </SurfaceShell>
    );
}

function WidgetSurface({ title, summary, payload }: { title: string; summary?: string; payload: Record<string, unknown> }) {
    const datasets = payload.datasets && typeof payload.datasets === "object" && !Array.isArray(payload.datasets)
        ? payload.datasets as Record<string, unknown>
        : {};
    const names = Object.keys(datasets);
    const [active, setActive] = useState(names[0] ?? "default");
    const [scale, setScale] = useState(1);
    const points = coerceSeriesPoints(datasets[active]);
    const scaled = points.map((point) => ({ ...point, y: Number((point.y * scale).toFixed(2)) }));

    return (
        <SurfaceShell
            family="widgets"
            title={title}
            summary={summary}
            controls={
                <div className="flex flex-wrap gap-2">
                    <select value={active} onChange={(event) => setActive(event.target.value)} className="rounded border border-white/10 bg-black/20 px-2 py-1 text-xs text-white">
                        {names.map((name) => <option key={name} value={name}>{name}</option>)}
                    </select>
                    <label className="flex items-center gap-2 rounded border border-white/10 bg-black/20 px-2 py-1 text-xs text-white">
                        Scale
                        <input type="range" min="0.5" max="1.8" step="0.1" value={scale} onChange={(event) => setScale(Number(event.target.value))} />
                    </label>
                </div>
            }
        >
            <ChartSurface family="widgets" title={title} summary={summary} points={scaled} mode="bar" accent="#f59e0b" />
        </SurfaceShell>
    );
}

function WindowedLineSurface({ family, title, summary, payload, accent }: { family: "bokeh" | "ipympl"; title: string; summary?: string; payload: Record<string, unknown>; accent: string }) {
    const allPoints = coerceSeriesPoints(payload.series);
    const windowSize = Math.max(3, Math.min(8, Number(payload.windowSize) || 6));
    const [offset, setOffset] = useState(0);
    const maxOffset = Math.max(0, allPoints.length - windowSize);
    const visible = allPoints.slice(offset, offset + windowSize);

    return (
        <SurfaceShell
            family={family}
            title={title}
            summary={summary}
            controls={
                <label className="flex items-center gap-2 rounded border border-white/10 bg-black/20 px-2 py-1 text-xs text-white">
                    Window
                    <input type="range" min="0" max={maxOffset} step="1" value={offset} onChange={(event) => setOffset(Number(event.target.value))} />
                </label>
            }
        >
            <ChartSurface family={family} title={title} summary={summary} points={visible} mode="line" accent={accent} />
        </SurfaceShell>
    );
}

function BqplotSurface({ title, summary, payload }: { title: string; summary?: string; payload: Record<string, unknown> }) {
    const points = coerceSeriesPoints(payload.points);
    return <ChartSurface family="bqplot" title={title} summary={summary} points={points} mode="scatter" accent="#fbbf24" />;
}

function GeoSurface({ title, summary, payload }: { title: string; summary?: string; payload: Record<string, unknown> }) {
    const points = coercePoints(payload.points);
    const [selected, setSelected] = useState(0);
    const [zoom, setZoom] = useState(1);
    const active = points[selected] ?? points[0];

    return (
        <SurfaceShell
            family="geospatial"
            title={title}
            summary={summary}
            controls={
                <label className="flex items-center gap-2 rounded border border-white/10 bg-black/20 px-2 py-1 text-xs text-white">
                    Zoom
                    <input type="range" min="1" max="2.4" step="0.1" value={zoom} onChange={(event) => setZoom(Number(event.target.value))} />
                </label>
            }
        >
            <div className="grid gap-3 lg:grid-cols-[minmax(0,1fr)_180px]">
                <svg viewBox="0 0 100 70" className="w-full rounded border border-white/10 bg-[radial-gradient(circle_at_top,_rgba(163,230,53,0.12),_transparent_50%),linear-gradient(180deg,rgba(10,15,10,0.95),rgba(12,20,12,1))]">
                    {Array.from({ length: 9 }).map((_, index) => <line key={`v-${index}`} x1={10 + index * 10} y1="0" x2={10 + index * 10} y2="70" stroke="rgba(255,255,255,0.06)" />)}
                    {Array.from({ length: 7 }).map((_, index) => <line key={`h-${index}`} x1="0" y1={10 + index * 10} x2="100" y2={10 + index * 10} stroke="rgba(255,255,255,0.06)" />)}
                    <g transform={`scale(${zoom}) translate(${(1 - zoom) * 14} ${(1 - zoom) * 10})`}>
                        {points.map((point, index) => (
                            <g key={point.label} onClick={() => setSelected(index)} className="cursor-pointer">
                                <circle cx={point.x} cy={point.y} r={selected === index ? 3.8 : 2.6} fill={point.color ?? (selected === index ? "#fef08a" : "#bef264")} />
                                <text x={point.x + 2.5} y={point.y - 2.5} fill="rgba(255,255,255,0.82)" fontSize="3.2">{point.label}</text>
                            </g>
                        ))}
                    </g>
                </svg>
                <div className="space-y-2">
                    {points.map((point, index) => (
                        <button key={point.label} type="button" onClick={() => setSelected(index)} className={`w-full rounded border px-3 py-2 text-left text-xs ${selected === index ? "border-lime-300/50 bg-lime-200/10 text-white" : "border-white/10 bg-black/10 text-zinc-300"}`}>
                            <span className="block font-semibold">{point.label}</span>
                            <span className="block opacity-70">{point.value ?? Math.round(point.x + point.y)} units</span>
                        </button>
                    ))}
                    {active && <p className="rounded border border-white/10 bg-black/10 px-3 py-2 text-xs text-zinc-300">Focused marker: <span className="font-semibold text-white">{active.label}</span></p>}
                </div>
            </div>
        </SurfaceShell>
    );
}

function NetworkSurface({ title, summary, payload }: { title: string; summary?: string; payload: Record<string, unknown> }) {
    const rawNodes = Array.isArray(payload.nodes) ? payload.nodes : [];
    const edges = Array.isArray(payload.edges) ? payload.edges : [];
    const nodes = rawNodes.map((node, index) => {
        const entry = node as Record<string, unknown>;
        const angle = (Math.PI * 2 * index) / Math.max(rawNodes.length, 1);
        return {
            id: typeof entry.id === "string" ? entry.id : `n-${index + 1}`,
            label: typeof entry.label === "string" ? entry.label : `Node ${index + 1}`,
            x: 180 + Math.cos(angle) * 120,
            y: 120 + Math.sin(angle) * 80,
        };
    });
    const [activeId, setActiveId] = useState(nodes[0]?.id ?? "");
    const linkedIds = new Set<string>(
        edges.flatMap((edge) => {
            const entry = edge as Record<string, unknown>;
            const source = typeof entry.source === "string" ? entry.source : "";
            const target = typeof entry.target === "string" ? entry.target : "";
            if (source === activeId || target === activeId) return [source, target];
            return [];
        }),
    );

    return (
        <SurfaceShell family="network" title={title} summary={summary}>
            <div className="grid gap-3 lg:grid-cols-[minmax(0,1fr)_200px]">
                <svg viewBox="0 0 360 240" className="w-full rounded border border-white/10 bg-zinc-950/70">
                    {edges.map((edge, index) => {
                        const entry = edge as Record<string, unknown>;
                        const source = nodes.find((node) => node.id === entry.source);
                        const target = nodes.find((node) => node.id === entry.target);
                        if (!source || !target) return null;
                        const active = source.id === activeId || target.id === activeId;
                        return <line key={`${source.id}-${target.id}-${index}`} x1={source.x} y1={source.y} x2={target.x} y2={target.y} stroke={active ? "#7dd3fc" : "rgba(255,255,255,0.18)"} strokeWidth={active ? 3 : 1.5} />;
                    })}
                    {nodes.map((node) => (
                        <g key={node.id} onClick={() => setActiveId(node.id)} className="cursor-pointer">
                            <circle cx={node.x} cy={node.y} r={linkedIds.has(node.id) || node.id === activeId ? 16 : 12} fill={node.id === activeId ? "#38bdf8" : linkedIds.has(node.id) ? "#7dd3fc" : "#1f2937"} stroke="rgba(255,255,255,0.4)" />
                            <text x={node.x} y={node.y + 4} textAnchor="middle" fill="white" fontSize="10">{node.label.slice(0, 8)}</text>
                        </g>
                    ))}
                </svg>
                <div className="space-y-2">
                    {nodes.map((node) => (
                        <button key={node.id} type="button" onClick={() => setActiveId(node.id)} className={`w-full rounded border px-3 py-2 text-left text-xs ${node.id === activeId ? "border-sky-300/50 bg-sky-300/10 text-white" : "border-white/10 bg-black/10 text-zinc-300"}`}>
                            {node.label}
                        </button>
                    ))}
                </div>
            </div>
        </SurfaceShell>
    );
}

function ThreeDSurface({ title, summary, payload }: { title: string; summary?: string; payload: Record<string, unknown> }) {
    const canvasRef = useRef<HTMLCanvasElement | null>(null);
    const [speed, setSpeed] = useState(typeof payload.rotationSpeed === "number" ? payload.rotationSpeed : 0.02);
    const [tilt, setTilt] = useState(0.7);

    useEffect(() => {
        const canvas = canvasRef.current;
        if (!canvas) return;
        const context = canvas.getContext("2d");
        if (!context) return;

        let frame = 0;
        let animation = 0;
        const vertices = [
            [-1, -1, -1],
            [1, -1, -1],
            [1, 1, -1],
            [-1, 1, -1],
            [-1, -1, 1],
            [1, -1, 1],
            [1, 1, 1],
            [-1, 1, 1],
        ];
        const edges = [[0, 1], [1, 2], [2, 3], [3, 0], [4, 5], [5, 6], [6, 7], [7, 4], [0, 4], [1, 5], [2, 6], [3, 7]];

        const draw = () => {
            frame += speed;
            context.clearRect(0, 0, canvas.width, canvas.height);
            context.fillStyle = "#020617";
            context.fillRect(0, 0, canvas.width, canvas.height);
            const projected = vertices.map(([x, y, z]) => {
                const cosY = Math.cos(frame);
                const sinY = Math.sin(frame);
                const cosX = Math.cos(tilt);
                const sinX = Math.sin(tilt);
                const rx = x * cosY - z * sinY;
                const rz = x * sinY + z * cosY;
                const ry = y * cosX - rz * sinX;
                const depth = rz * cosX + y * sinX + 4;
                const scale = 90 / depth;
                return {
                    x: canvas.width / 2 + rx * scale,
                    y: canvas.height / 2 + ry * scale,
                };
            });

            context.strokeStyle = "#c4b5fd";
            context.lineWidth = 2;
            edges.forEach(([start, end]) => {
                context.beginPath();
                context.moveTo(projected[start].x, projected[start].y);
                context.lineTo(projected[end].x, projected[end].y);
                context.stroke();
            });

            animation = requestAnimationFrame(draw);
        };

        draw();
        return () => cancelAnimationFrame(animation);
    }, [speed, tilt]);

    return (
        <SurfaceShell
            family="three-d"
            title={title}
            summary={summary}
            controls={
                <div className="flex flex-wrap gap-2">
                    <label className="flex items-center gap-2 rounded border border-white/10 bg-black/20 px-2 py-1 text-xs text-white">
                        Spin
                        <input type="range" min="0.005" max="0.06" step="0.005" value={speed} onChange={(event) => setSpeed(Number(event.target.value))} />
                    </label>
                    <label className="flex items-center gap-2 rounded border border-white/10 bg-black/20 px-2 py-1 text-xs text-white">
                        Tilt
                        <input type="range" min="0.2" max="1.4" step="0.05" value={tilt} onChange={(event) => setTilt(Number(event.target.value))} />
                    </label>
                </div>
            }
        >
            <canvas ref={canvasRef} width={560} height={260} className="w-full rounded border border-white/10 bg-zinc-950/70" />
        </SurfaceShell>
    );
}

function CanvasSurface({ title, summary, payload }: { title: string; summary?: string; payload: Record<string, unknown> }) {
    const canvasRef = useRef<HTMLCanvasElement | null>(null);
    const [amplitude, setAmplitude] = useState(typeof payload.amplitude === "number" ? payload.amplitude : 0.7);
    const [frequency, setFrequency] = useState(typeof payload.frequency === "number" ? payload.frequency : 2.4);

    useEffect(() => {
        const canvas = canvasRef.current;
        if (!canvas) return;
        const context = canvas.getContext("2d");
        if (!context) return;
        let tick = 0;
        let animation = 0;
        const draw = () => {
            tick += 0.03;
            context.clearRect(0, 0, canvas.width, canvas.height);
            context.fillStyle = "#0f172a";
            context.fillRect(0, 0, canvas.width, canvas.height);
            context.strokeStyle = "#fb7185";
            context.lineWidth = 2;
            context.beginPath();
            for (let x = 0; x <= canvas.width; x += 6) {
                const progress = x / canvas.width;
                const wave = Math.sin(progress * Math.PI * frequency + tick) * amplitude;
                const y = canvas.height / 2 + wave * 70;
                if (x === 0) context.moveTo(x, y);
                else context.lineTo(x, y);
            }
            context.stroke();
            animation = requestAnimationFrame(draw);
        };
        draw();
        return () => cancelAnimationFrame(animation);
    }, [amplitude, frequency]);

    return (
        <SurfaceShell
            family="canvas"
            title={title}
            summary={summary}
            controls={
                <div className="flex flex-wrap gap-2">
                    <label className="flex items-center gap-2 rounded border border-white/10 bg-black/20 px-2 py-1 text-xs text-white">
                        Amplitude
                        <input type="range" min="0.2" max="1.2" step="0.05" value={amplitude} onChange={(event) => setAmplitude(Number(event.target.value))} />
                    </label>
                    <label className="flex items-center gap-2 rounded border border-white/10 bg-black/20 px-2 py-1 text-xs text-white">
                        Frequency
                        <input type="range" min="1" max="6" step="0.2" value={frequency} onChange={(event) => setFrequency(Number(event.target.value))} />
                    </label>
                </div>
            }
        >
            <canvas ref={canvasRef} width={560} height={220} className="w-full rounded border border-white/10 bg-zinc-950/70" />
        </SurfaceShell>
    );
}

export function NotebookSurface({ surface }: { surface: NotebookSurfaceOutput }) {
    if (surface.family === "vega" || surface.family === "altair") {
        const spec = surface.payload.spec && typeof surface.payload.spec === "object"
            ? surface.payload.spec as Record<string, unknown>
            : {};
        const points = coerceSeriesPoints(spec.values);
        const mark = spec.mark === "line" ? "line" : "bar";
        return <ChartSurface family={surface.family} title={surface.title} summary={surface.summary} points={points} mode={mark} accent={surface.family === "vega" ? "#22d3ee" : "#34d399"} />;
    }

    if (surface.family === "bokeh") {
        return <WindowedLineSurface family="bokeh" title={surface.title} summary={surface.summary} payload={surface.payload} accent="#e879f9" />;
    }

    if (surface.family === "widgets") {
        return <WidgetSurface title={surface.title} summary={surface.summary} payload={surface.payload} />;
    }

    if (surface.family === "ipympl") {
        return <WindowedLineSurface family="ipympl" title={surface.title} summary={surface.summary} payload={surface.payload} accent="#f59e0b" />;
    }

    if (surface.family === "bqplot") {
        return <BqplotSurface title={surface.title} summary={surface.summary} payload={surface.payload} />;
    }

    if (surface.family === "geospatial") {
        return <GeoSurface title={surface.title} summary={surface.summary} payload={surface.payload} />;
    }

    if (surface.family === "network") {
        return <NetworkSurface title={surface.title} summary={surface.summary} payload={surface.payload} />;
    }

    if (surface.family === "three-d") {
        return <ThreeDSurface title={surface.title} summary={surface.summary} payload={surface.payload} />;
    }

    return <CanvasSurface title={surface.title} summary={surface.summary} payload={surface.payload} />;
}