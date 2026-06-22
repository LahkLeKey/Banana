import { RoutePill as Pill } from '@banana/ui';
import { useMemo } from 'react';

import { useK3h4TrainingSessionContext } from './K3h4TrainingSessionContext';
import type { K3h4TrainingMode } from '../../../lib/api';
import { RadarMini } from './RadarMini';

// ── Shared style primitives ──────────────────────────────────────────────────

const CARD: React.CSSProperties = {
    borderRadius: 10,
    border: '1px solid rgba(45, 212, 191, 0.2)',
    background: 'rgba(3, 14, 25, 0.35)',
    padding: 12,
    display: 'grid',
    gap: 8,
};

const LABEL: React.CSSProperties = {
    fontSize: 10,
    letterSpacing: '0.07em',
    textTransform: 'uppercase',
    color: '#2dd4bf',
};

const MUTED: React.CSSProperties = { fontSize: 11, color: '#94a3b8' };

const ROW: React.CSSProperties = {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    gap: 6,
    fontSize: 11,
};

// ── Confidence sparkline (SVG, no charting dep) ───────────────────────────────

function ConfidenceSparkline({ values, width = 380, height = 72 }: {
    values: readonly number[];
    width?: number;
    height?: number;
}) {
    const points = useMemo(() => {
        if (values.length < 2) return null;
        const min = Math.min(...values);
        const max = Math.max(...values);
        const range = max - min || 1;
        return values.map((v, i) => ({
            x: (i / (values.length - 1)) * (width - 20) + 10,
            y: height - 8 - ((v - min) / range) * (height - 16),
            v,
        }));
    }, [values, width, height]);

    if (!points) {
        return <div style={{ ...MUTED, textAlign: 'center', padding: '10px 0' }}>Waiting for first epoch…</div>;
    }

    const pathD = points.map((p, i) => `${i === 0 ? 'M' : 'L'} ${p.x.toFixed(1)} ${p.y.toFixed(1)}`).join(' ');
    const last = points.at(-1)!;
    const peak = points.reduce((best, p) => (p.v > best.v ? p : best));
    const fillD = `${pathD} L ${last.x.toFixed(1)} ${height - 8} L ${points[0]!.x.toFixed(1)} ${height - 8} Z`;

    return (
        <svg width={width} height={height} style={{ overflow: 'visible', display: 'block' }}>
            <defs>
                <linearGradient id="bm-grad" x1="0" y1="0" x2="0" y2="1">
                    <stop offset="0%" stopColor="#2dd4bf" stopOpacity="0.28" />
                    <stop offset="100%" stopColor="#2dd4bf" stopOpacity="0" />
                </linearGradient>
            </defs>
            <path d={fillD} fill="url(#bm-grad)" />
            <path d={pathD} fill="none" stroke="#2dd4bf" strokeWidth="1.6" strokeLinejoin="round" />
            {points.map((p, i) => (
                <circle key={i} cx={p.x} cy={p.y} r={2.5}
                    fill={i === points.length - 1 ? '#2dd4bf' : '#0d1117'}
                    stroke="#2dd4bf" strokeWidth="1.2" />
            ))}
            <circle cx={peak.x} cy={peak.y} r={4} fill="#3fb950" />
            <text x={peak.x + 5} y={peak.y - 4} fill="#3fb950" fontSize={8}>
                peak {peak.v.toFixed(3)}
            </text>
            <text x={10} y={height} fill="#475569" fontSize={8}>e0</text>
            <text x={last.x - 10} y={height} fill="#475569" fontSize={8}>e{points.length - 1}</text>
        </svg>
    );
}

// ── Epoch table ──────────────────────────────────────────────────────────────

function EpochTable({ epochs, mode }: {
    epochs: readonly { epochIndex: number; confidence: number; mode: K3h4TrainingMode }[];
    mode: K3h4TrainingMode;
}) {
    const displayEpochs = epochs.slice(-6);

    return (
        <div style={{ display: 'grid', gap: 3 }}>
            {displayEpochs.map((e) => (
                <div key={e.epochIndex} style={{
                    display: 'grid',
                    gridTemplateColumns: '28px 1fr 60px',
                    alignItems: 'center',
                    gap: 8,
                    fontSize: 10,
                    padding: '3px 6px',
                    borderRadius: 5,
                    background: 'rgba(15, 23, 42, 0.45)',
                    border: '1px solid rgba(148, 163, 184, 0.1)',
                }}>
                    <span style={{ color: '#475569' }}>e{e.epochIndex}</span>
                    <div style={{
                        height: 5,
                        borderRadius: 999,
                        background: 'rgba(15, 23, 42, 0.7)',
                        overflow: 'hidden',
                    }}>
                        <div style={{
                            width: `${(e.confidence * 100).toFixed(0)}%`,
                            height: '100%',
                            borderRadius: 999,
                            background: e.confidence >= 0.55
                                ? 'linear-gradient(90deg, #3fb950, #2dd4bf)'
                                : e.confidence >= 0.45
                                    ? 'linear-gradient(90deg, #d29922, #fb923c)'
                                    : 'linear-gradient(90deg, #f85149, #fb7185)',
                        }} />
                    </div>
                    <span style={{
                        color: e.confidence >= 0.55 ? '#3fb950' : e.confidence >= 0.45 ? '#d29922' : '#f85149',
                        textAlign: 'right',
                        fontFamily: 'ui-monospace, monospace',
                    }}>
                        {e.confidence.toFixed(3)}
                    </span>
                </div>
            ))}
        </div>
    );
}

// ── Geometry cluster card ────────────────────────────────────────────────────

function GeometryCard({ geometry }: {
    geometry: NonNullable<ReturnType<typeof useK3h4TrainingSessionContext>['latestGeometry']>;
}) {
    const spectralAvailable = geometry.clusters.length > 0;

    return (
        <div style={{ display: 'flex', gap: 8, flexWrap: 'wrap', alignItems: 'center' }}>
            <Pill color="#a7f3d0" borderColor="rgba(16, 185, 129, 0.28)">
                {geometry.clusters.length} cluster{geometry.clusters.length !== 1 ? 's' : ''}
            </Pill>
            <Pill color="#bae6fd" borderColor="rgba(56, 189, 248, 0.3)">
                PCA 2D · {(geometry.projectionMetadata.explainedVariance * 100).toFixed(1)}% var
            </Pill>
            {spectralAvailable ? (
                <Pill color="#e9d5ff" borderColor="rgba(168, 85, 247, 0.26)">spectral active</Pill>
            ) : null}
            <Pill color="#fed7aa" borderColor="rgba(251, 146, 60, 0.28)">
                epoch {geometry.epoch}
            </Pill>
        </div>
    );
}

// ── Main panel ───────────────────────────────────────────────────────────────

export function BulkTrainingMonitorPanel() {
    const {
        phase,
        session,
        confidence,
        latestGeometry,
        errorMessage,
        workflowState,
        runWorkflow,
        orchestrateRealtimeTraining,
    } = useK3h4TrainingSessionContext();

    const isCreating = phase === 'creating';
    const confidenceValues = confidence?.epochs.map((e) => e.confidence) ?? [];
    const peakEpoch = confidence?.metadata.peakEpoch;
    const rollingAvg = confidence?.metadata.rollingAverage3;
    const epochCount = confidence?.epochs.length ?? 0;
    const sessionStatus = confidence?.status ?? (session ? 'pending' : null);
    const latestConfidence = confidenceValues.length > 0 ? confidenceValues[confidenceValues.length - 1]! : 0;
    const radarBulkAxes = useMemo(
        () => [
            { label: 'conf', value: latestConfidence },
            { label: 'epochs', value: Math.min(epochCount, 24) / 24 },
            { label: 'flow', value: workflowState.completedPasses > 0 && workflowState.totalPasses > 0 ? workflowState.completedPasses / workflowState.totalPasses : 0 },
            { label: 'cluster', value: latestGeometry ? Math.min(latestGeometry.clusters.length, 6) / 6 : 0 },
            { label: 'var', value: latestGeometry?.projectionMetadata.explainedVariance ?? 0 },
        ] as const,
        [epochCount, latestConfidence, latestGeometry, workflowState.completedPasses, workflowState.totalPasses],
    );

    return (
        <div style={{ display: 'flex', flexDirection: 'column', gap: 10, padding: 10, overflow: 'auto', height: '100%' }}>

            {/* ── Session badge ─────────────────────────────────────────── */}
            <div style={CARD}>
                <div style={LABEL}>Bulk Training Monitor</div>
                {!session ? (
                    <div style={{ display: 'grid', gap: 8 }}>
                        <div style={MUTED}>
                            No active session. Start one to begin live bulk polling.
                        </div>
                        <button
                            type="button"
                            disabled={isCreating}
                            onClick={() => orchestrateRealtimeTraining('power', 'bootstrap')}
                            style={{
                                borderRadius: 8,
                                border: 'none',
                                background: 'rgba(45, 212, 191, 0.14)',
                                color: '#a7f3d0',
                                padding: '6px 14px',
                                fontSize: 11,
                                cursor: isCreating ? 'not-allowed' : 'pointer',
                                opacity: isCreating ? 0.5 : 1,
                                letterSpacing: '0.04em',
                            }}
                        >
                            {isCreating ? 'Creating…' : 'Start Realtime Training (power)'}
                        </button>
                    </div>
                ) : (
                    <div style={{ display: 'flex', gap: 6, flexWrap: 'wrap', alignItems: 'center' }}>
                        <Pill color="#a7f3d0" borderColor="rgba(16, 185, 129, 0.28)">
                            {session.sessionId}
                        </Pill>
                        <Pill color="#bae6fd" borderColor="rgba(56, 189, 248, 0.3)">
                            {session.mode}
                        </Pill>
                        {sessionStatus ? (
                            <Pill
                                color={sessionStatus === 'completed' ? '#86efac' : sessionStatus === 'active' ? '#a7f3d0' : '#e9d5ff'}
                                borderColor="rgba(168, 85, 247, 0.26)"
                            >
                                {sessionStatus}
                            </Pill>
                        ) : null}
                        <Pill color="#bae6fd" borderColor="rgba(56, 189, 248, 0.3)">
                            {epochCount} epoch{epochCount !== 1 ? 's' : ''}
                        </Pill>
                        <Pill color={workflowState.status === 'running' ? '#a7f3d0' : '#bae6fd'} borderColor="rgba(16, 185, 129, 0.28)">
                            workflow {workflowState.status}
                        </Pill>
                    </div>
                )}
                {errorMessage ? (
                    <div style={{ fontSize: 11, color: '#fda4af' }}>{errorMessage}</div>
                ) : null}
            </div>

            {session ? (
                <div style={CARD}>
                    <div style={ROW}>
                        <div style={LABEL}>Realtime orchestration</div>
                    </div>
                    <div style={{ display: 'flex', gap: 6, flexWrap: 'wrap' }}>
                        <button
                            type="button"
                            disabled={workflowState.status === 'running'}
                            onClick={() => runWorkflow('bootstrap')}
                            style={{
                                borderRadius: 8,
                                border: '1px solid rgba(45, 212, 191, 0.4)',
                                background: 'rgba(3, 14, 25, 0.55)',
                                color: '#a7f3d0',
                                padding: '6px 10px',
                                fontSize: 11,
                                cursor: workflowState.status === 'running' ? 'not-allowed' : 'pointer',
                                opacity: workflowState.status === 'running' ? 0.5 : 1,
                            }}
                        >
                            Bootstrap (12)
                        </button>
                        <button
                            type="button"
                            disabled={workflowState.status === 'running'}
                            onClick={() => runWorkflow('relations')}
                            style={{
                                borderRadius: 8,
                                border: '1px solid rgba(56, 189, 248, 0.4)',
                                background: 'rgba(3, 14, 25, 0.55)',
                                color: '#bae6fd',
                                padding: '6px 10px',
                                fontSize: 11,
                                cursor: workflowState.status === 'running' ? 'not-allowed' : 'pointer',
                                opacity: workflowState.status === 'running' ? 0.5 : 1,
                            }}
                        >
                            Relations (36)
                        </button>
                    </div>
                    <div style={MUTED}>
                        Shared session orchestration.
                    </div>
                    <div style={{ display: 'flex', justifyContent: 'center' }}>
                        <RadarMini axes={radarBulkAxes} size={140} stroke="#22d3ee" fill="rgba(34, 211, 238, 0.2)" />
                    </div>
                </div>
            ) : null}

            {/* ── Confidence sparkline ───────────────────────────────────── */}
            {session ? (
                <div style={CARD}>
                    <div style={ROW}>
                        <div style={LABEL}>Confidence time-series</div>
                        <div style={{ display: 'flex', gap: 6 }}>
                            {peakEpoch !== null && peakEpoch !== undefined ? (
                                <Pill color="#3fb950" borderColor="rgba(63, 185, 80, 0.3)">
                                    peak e{peakEpoch}
                                </Pill>
                            ) : null}
                            {rollingAvg !== null && rollingAvg !== undefined ? (
                                <Pill color="#bae6fd" borderColor="rgba(56, 189, 248, 0.3)">
                                    avg3 {rollingAvg.toFixed(4)}
                                </Pill>
                            ) : null}
                        </div>
                    </div>
                    {confidence === null ? (
                        <div style={MUTED}>Polling…</div>
                    ) : confidence.epochs.length === 0 ? (
                        <div style={MUTED}>Pending epochs</div>
                    ) : (
                        <div style={{ overflowX: 'auto' }}>
                            <ConfidenceSparkline values={confidenceValues} />
                        </div>
                    )}
                </div>
            ) : null}

            {/* ── Epoch table ────────────────────────────────────────────── */}
            {session && confidence && confidence.epochs.length > 0 ? (
                <div style={CARD}>
                    <div style={LABEL}>Recent epochs (last 6)</div>
                    <EpochTable epochs={confidence.epochs} mode={session.mode} />
                </div>
            ) : null}

            {/* ── Latest geometry ────────────────────────────────────────── */}
            {latestGeometry ? (
                <div style={CARD}>
                    <div style={LABEL}>Latest epoch geometry</div>
                    <GeometryCard geometry={latestGeometry} />
                    <div style={{ display: 'grid', gap: 3 }}>
                        {latestGeometry.clusters.map((c) => (
                            <div key={c.clusterId} style={{
                                display: 'flex',
                                justifyContent: 'space-between',
                                fontSize: 10,
                                padding: '3px 8px',
                                borderRadius: 5,
                                background: 'rgba(45, 212, 191, 0.05)',
                                border: '1px solid rgba(45, 212, 191, 0.1)',
                            }}>
                                <span style={{ color: '#94a3b8' }}>c{c.clusterId}</span>
                                <span style={{ color: '#a7f3d0', fontFamily: 'ui-monospace, monospace' }}>
                                    r={c.inscribedRadius.toFixed(4)}
                                    {' '}({c.centroid.x.toFixed(3)}, {c.centroid.y.toFixed(3)})
                                </span>
                            </div>
                        ))}
                    </div>
                </div>
            ) : null}

            <div style={{ ...MUTED, fontSize: 10, marginTop: 'auto', paddingTop: 4, borderTop: '1px solid rgba(148, 163, 184, 0.12)', lineHeight: 1.6 }}>
                bulk confidence + geometry polling
            </div>
        </div>
    );
}
