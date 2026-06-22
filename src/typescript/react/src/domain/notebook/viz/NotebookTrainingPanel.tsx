import { RoutePill as Pill } from '@banana/ui';
import { useEffect, useMemo, useRef, useState } from 'react';

import type { K3h4TrainingMode } from '../../../lib/api';
import type { K3h4ClusterGeometry2d } from '../../../lib/k3h4GeometryTypes';
import { useK3h4TrainingSessionContext } from './K3h4TrainingSessionContext';
import { RadarMini } from './RadarMini';

// ---------------------------------------------------------------------------
// Small inline primitives (no new dependency)
// ---------------------------------------------------------------------------

const CARD_STYLE: React.CSSProperties = {
    borderRadius: 10,
    border: '1px solid rgba(45, 212, 191, 0.24)',
    background: 'rgba(3, 14, 25, 0.32)',
    padding: 12,
    display: 'grid',
    gap: 8,
};

const SECTION_LABEL: React.CSSProperties = {
    fontSize: 11,
    letterSpacing: '0.06em',
    textTransform: 'uppercase',
    color: '#67e8f9',
};

const MUTED: React.CSSProperties = { fontSize: 12, color: '#94a3b8' };

const ERROR_TEXT: React.CSSProperties = { fontSize: 12, color: '#fda4af' };

function Btn({
    onClick,
    disabled,
    children,
    variant = 'default',
}: {
    onClick: () => void;
    disabled?: boolean;
    children: React.ReactNode;
    variant?: 'default' | 'danger';
}) {
    return (
        <button
            type="button"
            onClick={onClick}
            disabled={disabled}
            style={{
                borderRadius: 8,
                border: `1px solid ${variant === 'danger' ? 'rgba(244, 63, 94, 0.4)' : 'rgba(45, 212, 191, 0.4)'}`,
                background: variant === 'danger' ? 'rgba(15, 23, 42, 0.55)' : 'rgba(3, 14, 25, 0.55)',
                color: variant === 'danger' ? '#fda4af' : '#a7f3d0',
                padding: '6px 14px',
                cursor: disabled ? 'not-allowed' : 'pointer',
                fontSize: 11,
                letterSpacing: '0.04em',
                opacity: disabled ? 0.5 : 1,
                transition: 'opacity 0.15s',
            }}
        >
            {children}
        </button>
    );
}

function ModeSelector({
    value,
    onChange,
    disabled,
}: {
    value: K3h4TrainingMode;
    onChange: (m: K3h4TrainingMode) => void;
    disabled?: boolean;
}) {
    return (
        <div style={{ display: 'flex', gap: 6, alignItems: 'center' }}>
            {(['power', 'multiplicative'] as K3h4TrainingMode[]).map((m) => (
                <button
                    key={m}
                    type="button"
                    disabled={disabled}
                    onClick={() => onChange(m)}
                    style={{
                        borderRadius: 8,
                        border: `1px solid ${value === m ? 'rgba(45, 212, 191, 0.55)' : 'rgba(148, 163, 184, 0.25)'}`,
                        background: value === m ? 'rgba(45, 212, 191, 0.12)' : 'rgba(15, 23, 42, 0.4)',
                        color: value === m ? '#a7f3d0' : '#64748b',
                        padding: '4px 10px',
                        cursor: disabled ? 'not-allowed' : 'pointer',
                        fontSize: 11,
                        opacity: disabled ? 0.5 : 1,
                    }}
                >
                    {m}
                </button>
            ))}
        </div>
    );
}

// ---------------------------------------------------------------------------
// Confidence chart (sparkline via SVG — no charting library)
// ---------------------------------------------------------------------------

function ConfidenceSparkline({
    values,
    width = 340,
    height = 80,
}: {
    values: readonly number[];
    width?: number;
    height?: number;
}) {
    const points = useMemo(() => {
        if (values.length < 2) return null;
        const minV = Math.min(...values);
        const maxV = Math.max(...values);
        const range = maxV - minV || 1;
        return values.map((v, i) => {
            const x = (i / (values.length - 1)) * (width - 20) + 10;
            const y = height - 10 - ((v - minV) / range) * (height - 20);
            return { x, y, v };
        });
    }, [values, width, height]);

    if (!points) {
        return (
            <div style={{ ...MUTED, textAlign: 'center', paddingTop: 8 }}>
                Waiting for first epoch…
            </div>
        );
    }

    const pathD = points
        .map((p, i) => `${i === 0 ? 'M' : 'L'} ${p.x.toFixed(1)} ${p.y.toFixed(1)}`)
        .join(' ');

    const peakPt = points.reduce((best, p) => (p.v > best.v ? p : best));
    const lastPt = points[points.length - 1]!;

    return (
        <svg width={width} height={height} style={{ overflow: 'visible' }}>
            <defs>
                <linearGradient id="conf-grad" x1="0" y1="0" x2="0" y2="1">
                    <stop offset="0%" stopColor="#2dd4bf" stopOpacity="0.3" />
                    <stop offset="100%" stopColor="#2dd4bf" stopOpacity="0" />
                </linearGradient>
            </defs>
            {/* area fill */}
            <path
                d={`${pathD} L ${lastPt.x.toFixed(1)} ${height - 10} L ${points[0]!.x.toFixed(1)} ${height - 10} Z`}
                fill="url(#conf-grad)"
            />
            {/* line */}
            <path d={pathD} fill="none" stroke="#2dd4bf" strokeWidth={1.5} strokeLinejoin="round" />
            {/* peak dot */}
            <circle cx={peakPt.x} cy={peakPt.y} r={3} fill="#a7f3d0" />
            <text x={peakPt.x + 5} y={peakPt.y - 4} fill="#a7f3d0" fontSize={9}>
                peak {peakPt.v.toFixed(3)}
            </text>
            {/* latest dot */}
            <circle cx={lastPt.x} cy={lastPt.y} r={3} fill="#67e8f9" />
        </svg>
    );
}

// ---------------------------------------------------------------------------
// Main panel
// ---------------------------------------------------------------------------

export function NotebookTrainingPanel() {
    const {
        phase,
        session,
        confidence,
        latestGeometry,
        errorMessage,
        workflowState,
        workflowLogs,
        startSession,
        runWorkflow,
        stopWorkflow,
        clearWorkflowLogs,
        clearSession,
    } = useK3h4TrainingSessionContext();

    const [pendingMode, setPendingMode] = useState<K3h4TrainingMode>('power');

    const isActive = phase === 'active';
    const isCreating = phase === 'creating';

    const confidenceValues = confidence?.epochs.map((e) => e.confidence) ?? [];
    const rollingAvg = confidence?.metadata.rollingAverage3;
    const peakEpoch = confidence?.metadata.peakEpoch;
    const sessionStatus = confidence?.status ?? (session ? 'pending' : null);
    const workflowBusy = workflowState.status === 'running';
    const workflowProgressPct =
        workflowState.totalPasses > 0 ?
            Math.round((workflowState.completedPasses / workflowState.totalPasses) * 100) :
            0;
    const geometryClusters = (latestGeometry?.clusters ?? []) as readonly K3h4ClusterGeometry2d[];
    const workflowLogViewportRef = useRef<HTMLDivElement | null>(null);

    const latestConfidence = confidenceValues.length > 0 ? confidenceValues[confidenceValues.length - 1]! : 0;
    const radarSessionAxes = useMemo(
        () => [
            { label: 'conf', value: latestConfidence },
            { label: 'epochs', value: Math.min(epochCountFromConfidence(confidence), 24) / 24 },
            { label: 'flow', value: workflowProgressPct / 100 },
            { label: 'cluster', value: latestGeometry ? Math.min(latestGeometry.clusters.length, 6) / 6 : 0 },
            { label: 'var', value: latestGeometry?.projectionMetadata.explainedVariance ?? 0 },
        ] as const,
        [confidence, latestConfidence, latestGeometry, workflowProgressPct],
    );

    const radarWorkflowAxes = useMemo(
        () => [
            { label: 'done', value: workflowProgressPct / 100 },
            { label: 'pace', value: workflowBusy ? 1 : 0.25 },
            { label: 'epochs', value: Math.min(epochCountFromConfidence(confidence), 18) / 18 },
            { label: 'mode', value: session?.mode === 'power' ? 0.75 : 1 },
            { label: 'state', value: workflowState.status === 'completed' ? 1 : workflowState.status === 'running' ? 0.8 : workflowState.status === 'error' ? 0.2 : 0.35 },
        ] as const,
        [confidence, session?.mode, workflowBusy, workflowProgressPct, workflowState.status],
    );

    useEffect(() => {
        if (!workflowLogViewportRef.current) {
            return;
        }
        workflowLogViewportRef.current.scrollTop = workflowLogViewportRef.current.scrollHeight;
    }, [workflowLogs]);

    return (
        <div style={{ display: 'flex', flexDirection: 'column', gap: 10, padding: 10, overflow: 'auto' }}>

            {/* ── Session header ─────────────────────────────────────────────── */}
            <div style={CARD_STYLE}>
                <div style={SECTION_LABEL}>Training Session</div>

                {!session ? (
                    <>
                        <div style={MUTED}>
                            Start a shared training session. Sessions persist in the browser
                            and are visible to all researchers (no auth required).
                        </div>
                        <div style={{ display: 'flex', flexDirection: 'column', gap: 8 }}>
                            <div style={{ ...MUTED, fontSize: 11 }}>Scoring mode</div>
                            <ModeSelector
                                value={pendingMode}
                                onChange={setPendingMode}
                                disabled={isCreating}
                            />
                            <Btn
                                onClick={() => startSession(pendingMode)}
                                disabled={isCreating}
                            >
                                {isCreating ? 'Creating…' : 'Start Training Session'}
                            </Btn>
                        </div>
                    </>
                ) : (
                    <div style={{ display: 'grid', gap: 8 }}>
                        <div style={{ display: 'flex', gap: 6, flexWrap: 'wrap' }}>
                            <Pill color="#a7f3d0" borderColor="rgba(16, 185, 129, 0.28)">
                                Session {session.sessionId}
                            </Pill>
                            <Pill color="#bae6fd" borderColor="rgba(56, 189, 248, 0.3)">
                                Mode: {session.mode}
                            </Pill>
                            {sessionStatus ? (
                                <Pill
                                    color={sessionStatus === 'completed' ? '#86efac' : sessionStatus === 'active' ? '#a7f3d0' : '#e9d5ff'}
                                    borderColor={sessionStatus === 'completed' ? 'rgba(34, 197, 94, 0.28)' : 'rgba(168, 85, 247, 0.26)'}
                                >
                                    {sessionStatus}
                                </Pill>
                            ) : null}
                            {confidence ? (
                                <Pill color="#bae6fd" borderColor="rgba(56, 189, 248, 0.3)">
                                    {confidence.epochs.length} epoch{confidence.epochs.length !== 1 ? 's' : ''}
                                </Pill>
                            ) : null}
                        </div>
                        <div style={{ ...MUTED, fontSize: 10 }}>
                            Created {new Date(session.createdAtUtc).toLocaleString()}
                        </div>
                        <Btn onClick={clearSession} variant="danger">
                            Clear Session
                        </Btn>
                        <div style={{ display: 'flex', justifyContent: 'center' }}>
                            <RadarMini axes={radarSessionAxes} size={148} stroke="#2dd4bf" fill="rgba(45, 212, 191, 0.2)" />
                        </div>
                    </div>
                )}

                {errorMessage ? (
                    <div style={ERROR_TEXT}>{errorMessage}</div>
                ) : null}
            </div>

            {/* ── Confidence time-series ─────────────────────────────────────── */}
            {isActive && session ? (
                <div style={CARD_STYLE}>
                    <div style={SECTION_LABEL}>Confidence Time-Series</div>

                    {confidence === null ? (
                        <div style={MUTED}>Polling for epoch data…</div>
                    ) : null}

                    {confidence !== null && confidence.epochs.length === 0 ? (
                        <div style={MUTED}>Pending epochs</div>
                    ) : null}

                    {confidenceValues.length > 0 ? (
                        <div style={{ display: 'grid', gap: 6 }}>
                            <div style={{ overflowX: 'auto' }}>
                                <ConfidenceSparkline values={confidenceValues} />
                            </div>
                            <div style={{ display: 'flex', gap: 6, flexWrap: 'wrap' }}>
                                {peakEpoch !== null && peakEpoch !== undefined ? (
                                    <Pill color="#a7f3d0" borderColor="rgba(16, 185, 129, 0.28)">
                                        Peak epoch: {peakEpoch}
                                    </Pill>
                                ) : null}
                                {rollingAvg !== null && rollingAvg !== undefined ? (
                                    <Pill color="#bae6fd" borderColor="rgba(56, 189, 248, 0.3)">
                                        3-epoch avg: {rollingAvg.toFixed(4)}
                                    </Pill>
                                ) : null}
                                <Pill
                                    color={confidenceValues[confidenceValues.length - 1]! > 0 ? '#86efac' : '#e9d5ff'}
                                    borderColor="rgba(34, 197, 94, 0.28)"
                                >
                                    Latest: {confidenceValues[confidenceValues.length - 1]!.toFixed(4)}
                                </Pill>
                            </div>
                        </div>
                    ) : null}
                </div>
            ) : null}

            {/* ── Latest epoch geometry summary ─────────────────────────────── */}
            {isActive && latestGeometry ? (
                <div style={CARD_STYLE}>
                    <div style={SECTION_LABEL}>
                        Epoch {latestGeometry.epoch} Geometry — {latestGeometry.mode}
                    </div>
                    <div style={{ display: 'flex', gap: 6, flexWrap: 'wrap' }}>
                        <Pill color="#a7f3d0" borderColor="rgba(16, 185, 129, 0.28)">
                            {latestGeometry.clusters.length} cluster{latestGeometry.clusters.length !== 1 ? 's' : ''}
                        </Pill>
                        <Pill color="#bae6fd" borderColor="rgba(56, 189, 248, 0.3)">
                            PCA {latestGeometry.projectionMetadata.components}D ·{' '}
                            {(latestGeometry.projectionMetadata.explainedVariance * 100).toFixed(1)}% var
                        </Pill>
                        <Pill color="#e9d5ff" borderColor="rgba(168, 85, 247, 0.26)">
                            Tokens: {latestGeometry.tokens.length}
                        </Pill>
                    </div>
                    <div style={{ display: 'grid', gap: 4 }}>
                        {geometryClusters.map((cluster: K3h4ClusterGeometry2d) => (
                            <div
                                key={cluster.clusterId}
                                style={{
                                    display: 'flex',
                                    justifyContent: 'space-between',
                                    fontSize: 11,
                                    padding: '4px 8px',
                                    borderRadius: 6,
                                    background: 'rgba(45, 212, 191, 0.06)',
                                    border: '1px solid rgba(45, 212, 191, 0.12)',
                                }}
                            >
                                <span style={{ color: '#94a3b8' }}>Cluster {cluster.clusterId}</span>
                                <span style={{ color: '#a7f3d0' }}>
                                    r={cluster.inscribedRadius.toFixed(4)} · centroid=({cluster.centroid.x.toFixed(3)}, {cluster.centroid.y.toFixed(3)})
                                </span>
                            </div>
                        ))}
                    </div>
                    <div style={{ ...MUTED, fontSize: 10 }}>
                        Centroid ({latestGeometry.clusters[0]?.centroid.x.toFixed(3) ?? '—'},
                        {' '}{latestGeometry.clusters[0]?.centroid.y.toFixed(3) ?? '—'}) ·
                        Projection: {latestGeometry.projectionMetadata.method.toUpperCase()},
                        dim={latestGeometry.projectionMetadata.components}
                    </div>
                </div>
            ) : null}

            {/* ── Workflow controls ─────────────────────────────────────────── */}
            {isActive && session ? (
                <div style={{
                    ...CARD_STYLE,
                    border: '1px solid rgba(148, 163, 184, 0.18)',
                    background: 'rgba(15, 23, 42, 0.32)',
                }}>
                    <div style={SECTION_LABEL}>Training Workflows</div>

                    <div style={{ display: 'flex', flexWrap: 'wrap', gap: 8 }}>
                        <Btn
                            onClick={() => runWorkflow('bootstrap')}
                            disabled={workflowBusy}
                        >
                            Bootstrap Data (12)
                        </Btn>
                        <Btn
                            onClick={() => runWorkflow('relations')}
                            disabled={workflowBusy}
                        >
                            Relation Sweep (36)
                        </Btn>
                        <Btn
                            onClick={() => runWorkflow('churn')}
                            disabled={workflowBusy}
                        >
                            Full Churn (96)
                        </Btn>
                        <Btn
                            onClick={stopWorkflow}
                            disabled={!workflowBusy}
                            variant="danger"
                        >
                            Stop Run
                        </Btn>
                    </div>

                    {workflowState.workflow ? (
                        <div style={{ display: 'grid', gap: 6 }}>
                            <div style={{ display: 'flex', gap: 6, flexWrap: 'wrap' }}>
                                <Pill color="#bae6fd" borderColor="rgba(56, 189, 248, 0.3)">
                                    Workflow: {workflowState.workflow}
                                </Pill>
                                <Pill color="#a7f3d0" borderColor="rgba(16, 185, 129, 0.28)">
                                    Passes: {workflowState.completedPasses}/{workflowState.totalPasses}
                                </Pill>
                                <Pill color="#e9d5ff" borderColor="rgba(168, 85, 247, 0.26)">
                                    Progress: {workflowProgressPct}%
                                </Pill>
                                <Pill
                                    color={workflowState.status === 'error' ? '#fda4af' : workflowState.status === 'completed' ? '#86efac' : '#bae6fd'}
                                    borderColor={workflowState.status === 'error' ? 'rgba(244, 63, 94, 0.3)' : 'rgba(56, 189, 248, 0.3)'}
                                >
                                    {workflowState.status}
                                </Pill>
                            </div>
                            <div style={{
                                height: 8,
                                borderRadius: 999,
                                background: 'rgba(15, 23, 42, 0.7)',
                                border: '1px solid rgba(45, 212, 191, 0.2)',
                                overflow: 'hidden',
                            }}>
                                <div style={{
                                    width: `${workflowProgressPct}%`,
                                    height: '100%',
                                    background: 'linear-gradient(90deg, rgba(20, 184, 166, 0.8), rgba(14, 165, 233, 0.85))',
                                    transition: 'width 0.2s ease',
                                }} />
                            </div>
                            <div style={{ display: 'flex', justifyContent: 'center' }}>
                                <RadarMini axes={radarWorkflowAxes} size={136} stroke="#38bdf8" fill="rgba(56, 189, 248, 0.18)" />
                            </div>
                        </div>
                    ) : null}

                    {workflowState.errorMessage ? (
                        <div style={ERROR_TEXT}>{workflowState.errorMessage}</div>
                    ) : null}

                    <div style={{ ...MUTED, fontSize: 10 }}>Live poll sync active</div>
                </div>
            ) : null}

            {/* ── Live workflow logs ───────────────────────────────────────── */}
            {isActive && session ? (
                <div style={{
                    ...CARD_STYLE,
                    border: '1px solid rgba(56, 189, 248, 0.2)',
                    background: 'rgba(7, 19, 34, 0.45)',
                }}>
                    <div style={{ display: 'flex', justifyContent: 'space-between', gap: 8, alignItems: 'center' }}>
                        <div style={SECTION_LABEL}>Live Training Logs</div>
                        <Btn
                            onClick={clearWorkflowLogs}
                            disabled={workflowLogs.length === 0 || workflowBusy}
                            variant="danger"
                        >
                            Clear Logs
                        </Btn>
                    </div>

                    <div style={{ ...MUTED, fontSize: 11 }}>Pass stream</div>

                    <div
                        ref={workflowLogViewportRef}
                        style={{
                            maxHeight: 220,
                            overflowY: 'auto',
                            borderRadius: 8,
                            border: '1px solid rgba(148, 163, 184, 0.2)',
                            background: 'rgba(2, 6, 23, 0.75)',
                            padding: 8,
                            display: 'grid',
                            gap: 6,
                        }}
                    >
                        {workflowLogs.length === 0 ? (
                            <div style={{ ...MUTED, fontSize: 11 }}>
                                No training logs yet. Start a workflow to stream pass-level output.
                            </div>
                        ) : workflowLogs.map((entry) => (
                            <div
                                key={entry.id}
                                style={{
                                    borderRadius: 6,
                                    border: `1px solid ${entry.level === 'error' ? 'rgba(244, 63, 94, 0.35)' : entry.level === 'warn' ? 'rgba(251, 191, 36, 0.35)' : entry.level === 'success' ? 'rgba(34, 197, 94, 0.35)' : 'rgba(56, 189, 248, 0.26)'}`,
                                    background: 'rgba(15, 23, 42, 0.55)',
                                    padding: '6px 8px',
                                    display: 'grid',
                                    gap: 2,
                                    fontFamily: 'ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace',
                                    fontSize: 11,
                                }}
                            >
                                <div style={{ color: '#cbd5e1' }}>
                                    [{new Date(entry.timestamp).toLocaleTimeString()}] {entry.message}
                                </div>
                                {entry.details ? (
                                    <div style={{ color: '#94a3b8', whiteSpace: 'pre-wrap', wordBreak: 'break-word' }}>
                                        {entry.details}
                                    </div>
                                ) : null}
                            </div>
                        ))}
                    </div>
                </div>
            ) : null}

        </div>
    );
}

function epochCountFromConfidence(confidence: {
    readonly epochs: readonly unknown[];
} | null): number {
    return confidence?.epochs.length ?? 0;
}
