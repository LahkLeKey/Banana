import { useMemo, useState } from 'react';

export type GeometryPoint2d = {
    readonly x: number;
    readonly y: number;
};

export type GeometryCluster = {
    readonly clusterId: number;
    readonly centroid: GeometryPoint2d;
    readonly inscribedRadius: number;
    readonly polygon: readonly GeometryPoint2d[];
};

export type GeometryToken = {
    readonly tokenId: string;
    readonly x: number;
    readonly y: number;
    readonly clusterId: number;
    readonly weightedScore: number;
};

export type GeometryProjectionMetadata = {
    readonly method: string;
    readonly components: number;
    readonly explainedVariance: number;
};

export type EpochGeometryPayload = {
    readonly epoch: number;
    readonly mode: 'multiplicative' | 'power';
    readonly clusters: readonly GeometryCluster[];
    readonly tokens: readonly GeometryToken[];
    readonly projectionMetadata: GeometryProjectionMetadata;
};

export type HypersphereGeometryOverlayProps = {
    readonly geometry: EpochGeometryPayload;
    readonly width?: number;
    readonly height?: number;
};

const MARGIN = 36;

function scaleToViewport(
    value: number,
    min: number,
    max: number,
    size: number,
): number {
    if (Math.abs(max - min) < 0.000001) {
        return size / 2;
    }
    const t = (value - min) / (max - min);
    return t * size;
}

function pointsToPolygon(points: readonly GeometryPoint2d[]): string {
    return points.map((point) => `${point.x.toFixed(2)},${point.y.toFixed(2)}`).join(' ');
}

function boundsFromGeometry(geometry: EpochGeometryPayload): {
    minX: number;
    maxX: number;
    minY: number;
    maxY: number;
} {
    const xs = [
        ...geometry.clusters.flatMap((cluster) => [cluster.centroid.x, ...cluster.polygon.map((point) => point.x)]),
        ...geometry.tokens.map((token) => token.x),
    ];
    const ys = [
        ...geometry.clusters.flatMap((cluster) => [cluster.centroid.y, ...cluster.polygon.map((point) => point.y)]),
        ...geometry.tokens.map((token) => token.y),
    ];

    return {
        minX: Math.min(...xs),
        maxX: Math.max(...xs),
        minY: Math.min(...ys),
        maxY: Math.max(...ys),
    };
}

export function HypersphereGeometryOverlay({
    geometry,
    width = 720,
    height = 420,
}: HypersphereGeometryOverlayProps) {
    const [selectedClusterId, setSelectedClusterId] = useState<number | null>(null);

    const innerWidth = width - MARGIN * 2;
    const innerHeight = height - MARGIN * 2;

    const bounds = useMemo(() => boundsFromGeometry(geometry), [geometry]);

    const projectedClusters = useMemo(() => {
        return geometry.clusters.map((cluster) => {
            const centroidX = scaleToViewport(cluster.centroid.x, bounds.minX, bounds.maxX, innerWidth);
            const centroidY = innerHeight -
                scaleToViewport(cluster.centroid.y, bounds.minY, bounds.maxY, innerHeight);

            const scaledPolygon = cluster.polygon.map((point) => ({
                x: scaleToViewport(point.x, bounds.minX, bounds.maxX, innerWidth),
                y: innerHeight - scaleToViewport(point.y, bounds.minY, bounds.maxY, innerHeight),
            }));

            const xRange = Math.max(bounds.maxX - bounds.minX, 1);
            const radiusPx = (cluster.inscribedRadius / xRange) * innerWidth;

            return {
                cluster,
                centroidX,
                centroidY,
                radiusPx: Math.max(4, radiusPx),
                scaledPolygon,
            };
        });
    }, [bounds, geometry, innerHeight, innerWidth]);

    const projectedTokens = useMemo(() => {
        return geometry.tokens.map((token) => ({
            token,
            x: scaleToViewport(token.x, bounds.minX, bounds.maxX, innerWidth),
            y: innerHeight - scaleToViewport(token.y, bounds.minY, bounds.maxY, innerHeight),
        }));
    }, [bounds, geometry.tokens, innerHeight, innerWidth]);

    const selectedTokens = useMemo(() => {
        if (selectedClusterId === null) {
            return [] as GeometryToken[];
        }

        return geometry.tokens.filter((token) => token.clusterId === selectedClusterId);
    }, [geometry.tokens, selectedClusterId]);

    return (
        <section className="flex flex-col gap-4 rounded-xl border border-slate-200 bg-white p-4 shadow-sm">
            <header className="flex flex-col gap-1">
                <h2 className="text-base font-semibold text-slate-950">Hypersphere Geometry Overlay</h2>
                <p className="text-sm text-slate-500">
                    Epoch {geometry.epoch} · Mode {geometry.mode}
                </p>
                <p className="text-xs text-slate-500" data-testid="projection-metadata">
                    Projection: {geometry.projectionMetadata.method} · Components {geometry.projectionMetadata.components} · Explained variance {geometry.projectionMetadata.explainedVariance.toFixed(2)}
                </p>
            </header>

            <svg
                viewBox={`0 0 ${width} ${height}`}
                width={width}
                height={height}
                role="img"
                aria-label="Hypersphere geometry overlay"
            >
                <g transform={`translate(${MARGIN},${MARGIN})`}>
                    <rect x={0} y={0} width={innerWidth} height={innerHeight} fill="#f8fafc" stroke="#cbd5e1" />

                    {projectedClusters.map(({ cluster, scaledPolygon, centroidX, centroidY, radiusPx }) => {
                        const selected = selectedClusterId === cluster.clusterId;
                        return (
                            <g key={cluster.clusterId}>
                                <polygon
                                    data-testid="overlay-voronoi-cell"
                                    points={pointsToPolygon(scaledPolygon)}
                                    fill={selected ? 'rgba(59,130,246,0.20)' : 'rgba(59,130,246,0.10)'}
                                    stroke={selected ? '#1d4ed8' : '#2563eb'}
                                    strokeWidth={selected ? 2 : 1}
                                    onClick={() => setSelectedClusterId(cluster.clusterId)}
                                />
                                <circle
                                    data-testid="overlay-radius-circle"
                                    cx={centroidX}
                                    cy={centroidY}
                                    r={radiusPx}
                                    fill="none"
                                    stroke={selected ? '#ea580c' : '#f97316'}
                                    strokeDasharray="4 3"
                                    strokeWidth={selected ? 2 : 1.5}
                                />
                                <text x={centroidX + 6} y={centroidY - 6} fontSize={11} fill="#0f172a">
                                    c{cluster.clusterId}
                                </text>
                            </g>
                        );
                    })}

                    {projectedTokens.map(({ token, x, y }) => {
                        const highlighted = selectedClusterId !== null && token.clusterId === selectedClusterId;
                        return (
                            <g key={token.tokenId}>
                                <circle
                                    data-testid="overlay-token"
                                    cx={x}
                                    cy={y}
                                    r={highlighted ? 4.5 : 3}
                                    fill={highlighted ? '#16a34a' : '#334155'}
                                    opacity={highlighted ? 1 : 0.8}
                                />
                            </g>
                        );
                    })}
                </g>
            </svg>

            <div className="rounded-md border border-slate-200 bg-slate-50 p-3" data-testid="cluster-selection-panel">
                {selectedClusterId === null ? (
                    <p className="text-sm text-slate-500">Select a cluster to inspect token scores.</p>
                ) : (
                    <div className="flex flex-col gap-2">
                        <h3 className="text-sm font-semibold text-slate-900">Cluster {selectedClusterId} tokens</h3>
                        {selectedTokens.length === 0 ? (
                            <p className="text-sm text-slate-500">No tokens assigned.</p>
                        ) : (
                            <ul className="grid gap-1 text-xs text-slate-700">
                                {selectedTokens.map((token) => (
                                    <li key={token.tokenId}>
                                        {token.tokenId} · score {token.weightedScore.toFixed(3)}
                                    </li>
                                ))}
                            </ul>
                        )}
                    </div>
                )}
            </div>
        </section>
    );
}
