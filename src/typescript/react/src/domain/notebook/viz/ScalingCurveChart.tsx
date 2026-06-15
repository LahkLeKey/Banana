/** One sampled point from the scaling-benchmark artifact. */
export type ScalingSeriesEntry = {
    readonly n: number;
    readonly k3h4Ns: number;
    readonly attentionNs: number;
};

/** Benchmark payload rendered by the scaling-curve notebook chart. */
export type K3h4ScalingBenchmarkPayload = {
    readonly contractVersion: number;
    readonly schemaVersion: number;
    readonly series: readonly ScalingSeriesEntry[];
    readonly metadata: {
        readonly calibratedSizes: readonly number[];
        readonly extrapolatedAbove: number | null;
    };
};

/** Props for the log-log benchmark chart. */
export type ScalingCurveChartProps = {
    readonly data: K3h4ScalingBenchmarkPayload;
    readonly width?: number;
    readonly height?: number;
};

type Point = { readonly x: number; readonly y: number };

const CHART_MARGIN = { top: 32, right: 32, bottom: 56, left: 72 };

/* Maps a strictly positive value from one log domain into screen space. */
function logScale(
    domain: readonly [number, number],
    range: readonly [number, number],
): (value: number) => number {
    const logMin = Math.log10(domain[0]);
    const logMax = Math.log10(domain[1]);
    return (value: number) => {
        const t = (Math.log10(value) - logMin) / (logMax - logMin);
        return range[0] + t * (range[1] - range[0]);
    };
}

function pointsToPolyline(points: readonly Point[]): string {
    return points.map((p) => `${p.x.toFixed(1)},${p.y.toFixed(1)}`).join(' ');
}

function formatNs(ns: number): string {
    if (ns >= 1_000_000_000) return `${(ns / 1_000_000_000).toFixed(1)}s`;
    if (ns >= 1_000_000) return `${(ns / 1_000_000).toFixed(1)}ms`;
    if (ns >= 1_000) return `${(ns / 1_000).toFixed(0)}μs`;
    return `${ns}ns`;
}

function formatN(n: number): string {
    if (n >= 1_000) return `${(n / 1_000).toFixed(0)}k`;
    return String(n);
}

/* Computes least-squares slopes for the two log-log benchmark series. */
function logLogSlope(series: readonly ScalingSeriesEntry[]): {
    k3h4: number;
    attention: number;
} {
    const logN = series.map((e) => Math.log10(e.n));
    const logK = series.map((e) => Math.log10(e.k3h4Ns));
    const logA = series.map((e) => Math.log10(e.attentionNs));
    const n = series.length;
    const sumX = logN.reduce((a, b) => a + b, 0);
    const sumX2 = logN.reduce((a, b) => a + b * b, 0);
    const denom = n * sumX2 - sumX * sumX;

    function slope(logY: readonly number[]): number {
        if (denom === 0) return 0;
        const sumY = logY.reduce((a, b) => a + b, 0);
        const sumXY = logN.reduce((acc, x, i) => acc + x * logY[i]!, 0);
        return (n * sumXY - sumX * sumY) / denom;
    }

    return { k3h4: slope(logK), attention: slope(logA) };
}

/**
 * Renders the native k3h4 benchmark against the quadratic attention baseline
 * using shared log scales so slope differences remain visually comparable.
 */
export function ScalingCurveChart({
    data,
    width = 640,
    height = 360,
}: ScalingCurveChartProps) {
    const { series } = data;
    if (!series || series.length < 2) {
        return (
            <div className="text-muted-foreground text-sm p-4">
                Insufficient benchmark data to render scaling curve.
            </div>
        );
    }

    const innerW = width - CHART_MARGIN.left - CHART_MARGIN.right;
    const innerH = height - CHART_MARGIN.top - CHART_MARGIN.bottom;

    const allN = series.map((e) => e.n);
    const allNs = series.flatMap((e) => [e.k3h4Ns, e.attentionNs]);
    const nMin = Math.min(...allN);
    const nMax = Math.max(...allN);
    const nsMin = Math.min(...allNs);
    const nsMax = Math.max(...allNs);

    const xScale = logScale([nMin, nMax], [0, innerW]);
    const yScale = logScale([nsMin, nsMax], [innerH, 0]);

    const k3h4Points: Point[] = series.map((e) => ({
        x: xScale(e.n),
        y: yScale(e.k3h4Ns),
    }));

    const attnPoints: Point[] = series.map((e) => ({
        x: xScale(e.n),
        y: yScale(e.attentionNs),
    }));

    const slopes = logLogSlope(series);

    /* Y-axis tick marks (evenly spaced in log space) */
    const yTicks: number[] = [];
    const logNsMin = Math.floor(Math.log10(nsMin));
    const logNsMax = Math.ceil(Math.log10(nsMax));
    for (let exp = logNsMin; exp <= logNsMax; exp++) {
        yTicks.push(10 ** exp);
    }

    return (
        <div className="flex flex-col gap-2">
            <div className="flex items-center gap-6 text-xs">
                <span className="flex items-center gap-1">
                    <svg width="20" height="4">
                        <line x1="0" y1="2" x2="20" y2="2" stroke="#22c55e" strokeWidth="2" />
                    </svg>
                    k3h4 power-mode (slope {slopes.k3h4.toFixed(2)})
                </span>
                <span className="flex items-center gap-1">
                    <svg width="20" height="4">
                        <line x1="0" y1="2" x2="20" y2="2" stroke="#f59e0b" strokeWidth="2" strokeDasharray="4 2" />
                    </svg>
                    Transformer attention (slope {slopes.attention.toFixed(2)})
                </span>
            </div>

            <svg
                viewBox={`0 0 ${width} ${height}`}
                width={width}
                height={height}
                role="img"
                aria-label="Log-log scaling curve: k3h4 vs transformer attention"
                style={{ overflow: 'visible', fontFamily: 'inherit' }}
            >
                <g transform={`translate(${CHART_MARGIN.left},${CHART_MARGIN.top})`}>
                    {/* Grid lines */}
                    {yTicks.map((tick) => (
                        <line
                            key={tick}
                            x1={0}
                            y1={yScale(tick)}
                            x2={innerW}
                            y2={yScale(tick)}
                            stroke="currentColor"
                            strokeOpacity={0.08}
                            strokeWidth={1}
                        />
                    ))}

                    {/* Attention series (dashed, amber) */}
                    <polyline
                        points={pointsToPolyline(attnPoints)}
                        fill="none"
                        stroke="#f59e0b"
                        strokeWidth={2}
                        strokeDasharray="6 3"
                    />

                    {/* k3h4 series (solid, green) */}
                    <polyline
                        points={pointsToPolyline(k3h4Points)}
                        fill="none"
                        stroke="#22c55e"
                        strokeWidth={2}
                    />

                    {/* Data point dots */}
                    {k3h4Points.map((p, i) => (
                        <circle key={i} cx={p.x} cy={p.y} r={3} fill="#22c55e" />
                    ))}
                    {attnPoints.map((p, i) => (
                        <circle key={i} cx={p.x} cy={p.y} r={3} fill="#f59e0b" />
                    ))}

                    {/* X-axis */}
                    <line x1={0} y1={innerH} x2={innerW} y2={innerH} stroke="currentColor" strokeOpacity={0.3} />
                    {series.map((e) => (
                        <g key={e.n} transform={`translate(${xScale(e.n)},${innerH})`}>
                            <line y2={5} stroke="currentColor" strokeOpacity={0.3} />
                            <text
                                y={18}
                                textAnchor="middle"
                                fontSize={10}
                                fill="currentColor"
                                fillOpacity={0.6}
                            >
                                {formatN(e.n)}
                            </text>
                        </g>
                    ))}
                    <text
                        x={innerW / 2}
                        y={innerH + 42}
                        textAnchor="middle"
                        fontSize={11}
                        fill="currentColor"
                        fillOpacity={0.6}
                    >
                        Input size n (log scale)
                    </text>

                    {/* Y-axis */}
                    <line x1={0} y1={0} x2={0} y2={innerH} stroke="currentColor" strokeOpacity={0.3} />
                    {yTicks.map((tick) => (
                        <g key={tick} transform={`translate(0,${yScale(tick)})`}>
                            <line x2={-5} stroke="currentColor" strokeOpacity={0.3} />
                            <text
                                x={-8}
                                textAnchor="end"
                                fontSize={9}
                                fill="currentColor"
                                fillOpacity={0.6}
                                dominantBaseline="middle"
                            >
                                {formatNs(tick)}
                            </text>
                        </g>
                    ))}
                    <text
                        transform={`translate(-56,${innerH / 2}) rotate(-90)`}
                        textAnchor="middle"
                        fontSize={11}
                        fill="currentColor"
                        fillOpacity={0.6}
                    >
                        Measured cost (log scale)
                    </text>

                    {/* Title */}
                    <text
                        x={innerW / 2}
                        y={-12}
                        textAnchor="middle"
                        fontSize={12}
                        fontWeight="600"
                        fill="currentColor"
                    >
                        k3h4 O(n·k) vs Transformer O(n²) — Asymptotic Scaling
                    </text>
                </g>
            </svg>
        </div>
    );
}
