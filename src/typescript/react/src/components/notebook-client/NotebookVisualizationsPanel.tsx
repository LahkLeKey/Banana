import { RoutePill as Pill, RouteSubActionBar } from '@banana/ui';
import { useMemo, useState } from 'react';

import {
    HypersphereGeometryOverlay,
    type EpochGeometryPayload,
} from '../../domain/notebook/viz/HypersphereGeometryOverlay';
import {
    ModeDifferentiationView,
    type TokenScore,
} from '../../domain/notebook/viz/ModeDifferentiationView';
import { ScalingCurveChart } from '../../domain/notebook/viz/ScalingCurveChart';
import { useK3h4ScalingBenchmark } from '../../domain/notebook/viz/useK3h4ScalingBenchmark';

type NotebookVisualizationsPanelProps = {
    readonly analyticsAvailable?: boolean;
    readonly analyticsCohort?: string;
    readonly k3h4Clusters?: number;
    readonly k3h4Convergence?: string;
    readonly k3h4RuntimeMode?: 'multiplicative' | 'power';
    readonly k3h4ProjectionNodes?: readonly {
        id: 'intel' | 'objectives' | 'player' | 'ops';
        x: number;
        y: number;
        z: number;
        coherence: number;
        inradius: number;
        nearestNeighborDistance: number;
    }[];
    readonly k3h4Centers?: readonly {
        clusterId: number;
        centerQ16: readonly number[];
        memberVectorIds: readonly number[];
        memberCount: number;
    }[];
    readonly k3h4Radii?: readonly {
        clusterId: number;
        nearestNeighborDistanceQ16: number;
        inscribedRadiusQ16: number;
        radiusState: 'ok' | 'degenerate' | 'invalid';
    }[];
    readonly k3h4WeightedVoronoiScores?: readonly {
        vectorId: number;
        clusterId: number;
        distanceToCenterQ16: number;
        weightedScoreQ16: number;
        scoreValidity: 'valid' | 'clamped' | 'invalid';
    }[];
};

function q16ToFloat(value: number): number {
    return value / 65536;
}

type WeightedScoreEntry = {
    vectorId: number;
    clusterId: number;
    weightedScoreQ16: number;
    scoreValidity: 'valid' | 'clamped' | 'invalid';
};

function collapseBestScoresByVector(
    weightedScores: readonly WeightedScoreEntry[],
): WeightedScoreEntry[] {
    const bestByVector = new Map<number, WeightedScoreEntry>();

    for (const entry of weightedScores) {
        if (entry.scoreValidity === 'invalid') {
            continue;
        }

        const current = bestByVector.get(entry.vectorId);
        if (!current || entry.weightedScoreQ16 > current.weightedScoreQ16) {
            bestByVector.set(entry.vectorId, entry);
        }
    }

    return [...bestByVector.values()].sort((left, right) => left.vectorId - right.vectorId);
}

function buildModeScores(
    weightedScores: readonly WeightedScoreEntry[],
    runtimeMode: 'multiplicative' | 'power',
): { multiplicativeScores: TokenScore[]; powerScores: TokenScore[] } {
    const canonical = collapseBestScoresByVector(weightedScores)
        .map((entry) => ({
            tokenId: `vec-${entry.vectorId}`,
            clusterId: entry.clusterId,
            score: Math.max(0, q16ToFloat(entry.weightedScoreQ16)),
        }));

    if (runtimeMode === 'power') {
        return { multiplicativeScores: [...canonical], powerScores: canonical };
    }

    return { multiplicativeScores: canonical, powerScores: [...canonical] };
}

function buildGeometryPayload(params: {
    runtimeMode: 'multiplicative' | 'power';
    centers: readonly {
        clusterId: number;
        centerQ16: readonly number[];
    }[];
    radii: readonly {
        clusterId: number;
        inscribedRadiusQ16: number;
    }[];
    weightedScores: readonly WeightedScoreEntry[];
    projectionNodes: readonly {
        x: number;
        y: number;
    }[];
}): EpochGeometryPayload | null {
    const { centers, projectionNodes, radii, runtimeMode, weightedScores } = params;

    if (centers.length === 0 || weightedScores.length === 0) {
        return null;
    }

    const collapsedScores = collapseBestScoresByVector(weightedScores);

    const radiusByCluster = new Map(
        radii.map((entry) => [entry.clusterId, Math.max(0.03, q16ToFloat(entry.inscribedRadiusQ16))]),
    );
    const projectionFallback = projectionNodes.length > 0 ? projectionNodes : [{ x: 0, y: 0 }];

    const clusters = centers.map((center, index) => {
        const fallback = projectionFallback[index % projectionFallback.length];
        const centroidX = center.centerQ16[0] !== undefined ? q16ToFloat(center.centerQ16[0]) : fallback.x;
        const centroidY = center.centerQ16[1] !== undefined ? q16ToFloat(center.centerQ16[1]) : fallback.y;
        const radius = radiusByCluster.get(center.clusterId) ?? 0.08;

        return {
            clusterId: center.clusterId,
            centroid: { x: centroidX, y: centroidY },
            inscribedRadius: radius,
            polygon: Array.from({ length: 6 }, (_, spoke) => {
                const theta = (Math.PI * 2 * spoke) / 6;
                return {
                    x: centroidX + Math.cos(theta) * radius * 1.4,
                    y: centroidY + Math.sin(theta) * radius * 1.1,
                };
            }),
        };
    });

    const centroidByCluster = new Map(clusters.map((cluster) => [cluster.clusterId, cluster.centroid]));
    const tokens = collapsedScores
        .map((entry) => {
            const centroid = centroidByCluster.get(entry.clusterId) ?? { x: 0, y: 0 };
            const angle = (entry.vectorId % 12) * (Math.PI / 6);
            const radialOffset = 0.04 + Math.min(0.12, q16ToFloat(entry.weightedScoreQ16) * 0.08);
            return {
                tokenId: `vec-${entry.vectorId}`,
                x: centroid.x + Math.cos(angle) * radialOffset,
                y: centroid.y + Math.sin(angle) * radialOffset,
                clusterId: entry.clusterId,
                weightedScore: Math.max(0, q16ToFloat(entry.weightedScoreQ16)),
            };
        });

    return {
        epoch: 0,
        mode: runtimeMode,
        clusters,
        tokens,
        projectionMetadata: {
            method: 'k3h4-q16-projection',
            components: 2,
            explainedVariance: 1,
        },
    };
}

export function NotebookVisualizationsPanel({
    analyticsAvailable = true,
    analyticsCohort = 'default',
    k3h4Clusters = 0,
    k3h4Convergence = 'unknown',
    k3h4RuntimeMode = 'multiplicative',
    k3h4ProjectionNodes = [],
    k3h4Centers = [],
    k3h4Radii = [],
    k3h4WeightedVoronoiScores = [],
}: NotebookVisualizationsPanelProps) {
    const [activeTab, setActiveTab] = useState<'overview' | 'scaling' | 'modes' | 'geometry'>('overview');
    const { state: scalingBenchmarkState, refresh: refreshScalingBenchmark } = useK3h4ScalingBenchmark();

    const { multiplicativeScores, powerScores } = useMemo(
        () => buildModeScores(k3h4WeightedVoronoiScores, k3h4RuntimeMode),
        [k3h4RuntimeMode, k3h4WeightedVoronoiScores],
    );

    const geometryPayload = useMemo(
        () => buildGeometryPayload({
            runtimeMode: k3h4RuntimeMode,
            centers: k3h4Centers,
            radii: k3h4Radii,
            weightedScores: k3h4WeightedVoronoiScores,
            projectionNodes: k3h4ProjectionNodes,
        }),
        [k3h4Centers, k3h4ProjectionNodes, k3h4Radii, k3h4RuntimeMode, k3h4WeightedVoronoiScores],
    );

    const scalingCard = (
        <div style={{
            borderRadius: 10,
            border: '1px solid rgba(45, 212, 191, 0.24)',
            background: 'rgba(3, 14, 25, 0.32)',
            padding: 10,
            display: 'grid',
            gap: 8,
        }}>
            <div style={{ fontSize: 11, letterSpacing: '0.06em', textTransform: 'uppercase', color: '#67e8f9' }}>
                Asymptotic Scaling Benchmark
            </div>
            {scalingBenchmarkState.status === 'loading' || scalingBenchmarkState.status === 'idle' ? (
                <div style={{ fontSize: 12, color: '#94a3b8' }}>Loading benchmark artifact...</div>
            ) : null}
            {scalingBenchmarkState.status === 'not_found' ? (
                <div style={{ display: 'grid', gap: 6 }}>
                    <div style={{ fontSize: 12, color: '#fda4af' }}>Scaling benchmark artifact is not available.</div>
                    <button
                        type="button"
                        onClick={refreshScalingBenchmark}
                        style={{
                            borderRadius: 8,
                            border: '1px solid rgba(148, 163, 184, 0.4)',
                            background: 'rgba(15, 23, 42, 0.55)',
                            color: '#e2e8f0',
                            padding: '6px 10px',
                            width: 'fit-content',
                            cursor: 'pointer',
                            fontSize: 11,
                        }}
                    >
                        Retry
                    </button>
                </div>
            ) : null}
            {scalingBenchmarkState.status === 'error' ? (
                <div style={{ fontSize: 12, color: '#fda4af' }}>
                    Failed to load scaling benchmark: {scalingBenchmarkState.message}
                </div>
            ) : null}
            {scalingBenchmarkState.status === 'ok' ? (
                <div style={{ overflowX: 'auto' }}>
                    <ScalingCurveChart data={scalingBenchmarkState.data} width={420} height={260} />
                </div>
            ) : null}
        </div>
    );

    const modesCard = multiplicativeScores.length > 0 && powerScores.length > 0 ? (
        <ModeDifferentiationView
            multiplicativeScores={multiplicativeScores}
            powerScores={powerScores}
            comparisonMode="synthetic"
        />
    ) : (
        <div style={{ borderRadius: 10, border: '1px solid rgba(148, 163, 184, 0.24)', background: 'rgba(15, 23, 42, 0.46)', padding: 12, color: '#94a3b8', fontSize: 12 }}>
            Mode differentiation becomes available when weighted Voronoi scores are present.
        </div>
    );

    const geometryCard = geometryPayload ? (
        <div style={{ overflowX: 'auto' }}>
            <HypersphereGeometryOverlay geometry={geometryPayload} width={420} height={300} />
        </div>
    ) : (
        <div style={{ borderRadius: 10, border: '1px solid rgba(148, 163, 184, 0.24)', background: 'rgba(15, 23, 42, 0.46)', padding: 12, color: '#94a3b8', fontSize: 12 }}>
            Geometry overlay becomes available when cluster centers and weighted scores are present.
        </div>
    );

    return (
        <div style={{ display: 'flex', flexDirection: 'column', minHeight: 0 }}>
            <RouteSubActionBar
                tabs={[
                    { id: 'overview', label: 'Overview' },
                    { id: 'scaling', label: 'Scaling' },
                    { id: 'modes', label: 'Modes' },
                    { id: 'geometry', label: 'Geometry' },
                ]}
                activeTab={activeTab}
                onTabChange={(id) => setActiveTab(id as 'overview' | 'scaling' | 'modes' | 'geometry')}
                meta={`Cohort ${analyticsCohort} · K3H4 ${k3h4Clusters} · ${k3h4Convergence}`}
            />

            <div style={{ display: 'flex', gap: 6, padding: '6px 12px', borderBottom: '1px solid rgba(45, 212, 191, 0.08)', flexWrap: 'wrap', flexShrink: 0 }}>
                <Pill color={analyticsAvailable ? '#86efac' : '#fda4af'} borderColor={analyticsAvailable ? 'rgba(34, 197, 94, 0.28)' : 'rgba(244, 63, 94, 0.3)'}>
                    {analyticsAvailable ? 'Analytics Ready' : 'Analytics Unavailable'}
                </Pill>
                <Pill color="#a7f3d0" borderColor="rgba(16, 185, 129, 0.28)">K3H4: {k3h4Clusters} clusters</Pill>
                <Pill color="#bae6fd" borderColor="rgba(56, 189, 248, 0.3)">Convergence: {k3h4Convergence}</Pill>
                <Pill color="#e9d5ff" borderColor="rgba(168, 85, 247, 0.26)">Mode: {k3h4RuntimeMode}</Pill>
            </div>

            <div style={{ flex: 1, minHeight: 0, overflow: 'auto', padding: 10, display: 'grid', gap: 10 }}>
                {activeTab === 'overview' ? (
                    <>
                        {scalingCard}
                        {modesCard}
                        {geometryCard}
                    </>
                ) : null}
                {activeTab === 'scaling' ? scalingCard : null}
                {activeTab === 'modes' ? modesCard : null}
                {activeTab === 'geometry' ? geometryCard : null}
            </div>
        </div>
    );
}