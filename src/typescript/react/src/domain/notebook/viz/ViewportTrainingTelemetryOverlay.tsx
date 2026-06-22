import { useEffect, useMemo, useState } from 'react';

import { TelemetryHealthCard } from './TelemetryHealthCard';
import { InfoLine, MetricPill, SectionLabel } from './TelemetryPrimitives';
import { markerColorForLogLevel, TelemetrySparkline, type SparklineMarker } from './TelemetrySparkline';
import { useK3h4TrainingSessionContext } from './K3h4TrainingSessionContext';
import { useInteractionTracking } from './useInteractionTracking';
import { GamifiedActionPanel } from './GamifiedActionPanel';

type ViewportTrainingTelemetryOverlayProps = {
    readonly analyticsAvailable?: boolean;
    readonly analyticsCohort?: string;
    readonly lineCount?: number;
    readonly includeCount?: number;
    readonly callDensity?: number;
    readonly riskLabel?: string;
    readonly k3h4Dimensions?: number;
    readonly k3h4Alignment?: number;
    readonly cFileCount?: number;
    readonly modelConfidence?: number;
    readonly k3h4Clusters?: number;
    readonly k3h4Convergence?: string;
    readonly abiCoverageSummary?: string;
    readonly abiCoveragePercent?: number;
    readonly k3h4HashPreview?: string;
};

export function ViewportTrainingTelemetryOverlay({
    analyticsAvailable = true,
    analyticsCohort = 'default',
    lineCount = 0,
    includeCount = 0,
    callDensity = 0,
    riskLabel = 'Stable',
    k3h4Dimensions = 0,
    k3h4Alignment = 0,
    cFileCount = 0,
    modelConfidence = 0,
    k3h4Clusters = 0,
    k3h4Convergence = 'unknown',
    abiCoverageSummary = '0/5',
    abiCoveragePercent = 0,
    k3h4HashPreview = 'n/a',
}: ViewportTrainingTelemetryOverlayProps) {
    const {
        session,
        confidence,
        latestGeometry,
        workflowState,
        workflowLogs,
        orchestrateRealtimeTraining,
        runWorkflow,
    } = useK3h4TrainingSessionContext();

    const confidenceValues = confidence?.epochs.map((epoch) => epoch.confidence) ?? [];
    const latestConfidence =
        confidenceValues.length > 0 ? confidenceValues[confidenceValues.length - 1]! : 0;
    const baselineConfidence = confidenceValues.length > 0 ? confidenceValues[0]! : 0;
    const epochCount = confidence?.epochs.length ?? 0;
    const flow = workflowState.totalPasses > 0 ?
        workflowState.completedPasses / workflowState.totalPasses :
        0;

    const [viewportWidth, setViewportWidth] = useState<number>(
        typeof window === 'undefined' ? 1440 : window.innerWidth,
    );
    const [showTelemetryPanel, setShowTelemetryPanel] = useState(
        typeof window === 'undefined' ? true : window.innerWidth >= 900,
    );
    const [showMobileDeepDetails, setShowMobileDeepDetails] = useState(false);

    // Track user interactions and persist them to K3H4 model
    const { metrics, recentInteractions, trackInteraction } = useInteractionTracking({
        sessionId: session?.sessionId ?? '',
        mode: session?.mode ?? 'power',
        k3h4Payload: latestGeometry ? {
            centers: latestGeometry.clusters.map((c, i) => ({
                clusterId: i,
                centerQ16: [Math.round(c.centroid.x * 65536), Math.round(c.centroid.y * 65536)],
                memberVectorIds: [],
                memberCount: 0,
            })),
            radii: latestGeometry.clusters.map((c, i) => ({
                clusterId: i,
                nearestNeighborDistanceQ16: Math.round(c.inscribedRadius * 2 * 65536),
                inscribedRadiusQ16: Math.round(c.inscribedRadius * 65536),
                radiusState: 'ok' as const,
            })),
            weightedVoronoiScores: [],
            spectralProxy: [],
            observability: {
                convergenceStatus: 'converged' as const,
                iterationCount: epochCount,
                assignmentChangesLastIteration: 0,
                scoringValidity: 'valid' as const,
                deterministicHash: k3h4HashPreview,
            },
        } : undefined,
        confidence: latestConfidence,
        alignment: k3h4Alignment,
        radialStability: latestGeometry?.projectionMetadata.explainedVariance ?? 0,
    });

    useEffect(() => {
        if (typeof window === 'undefined') {
            return;
        }

        const onResize = () => {
            setViewportWidth(window.innerWidth);
        };

        window.addEventListener('resize', onResize);
        return () => {
            window.removeEventListener('resize', onResize);
        };
    }, []);

    const compact = viewportWidth < 1280;
    const mobile = viewportWidth < 900;
    const overlayTop = mobile ? 170 : compact ? 124 : 84;
    const sparklineBottom = mobile ? 88 : compact ? 82 : 72;
    const showRadarVisual = !mobile;
    const spectralActivation = latestGeometry ? 'affinity-graph' : 'disabled';

    const recentConfidenceValues = confidenceValues.slice(-10);
    const visibleConfidenceValues = confidenceValues.slice(-32);
    const recentBaseline = recentConfidenceValues.length > 0 ? recentConfidenceValues[0]! : latestConfidence;
    const globalDelta = latestConfidence - baselineConfidence;
    const recentDelta = latestConfidence - recentBaseline;
    const interactionCount = workflowLogs.length;
    const interactionYield = interactionCount > 0 ? globalDelta / interactionCount : 0;
    const lastInteraction = workflowLogs.length > 0 ? workflowLogs[workflowLogs.length - 1] : null;
    const varianceSignal = latestGeometry?.projectionMetadata.explainedVariance ?? 0;
    const confidenceTrendColor = globalDelta >= 0 ? '#34d399' : '#fb7185';

    const interactionMarkers = useMemo(() => {
        if (visibleConfidenceValues.length < 2 || workflowLogs.length === 0) {
            return [] as SparklineMarker[];
        }

        const markerCount = Math.min(6, workflowLogs.length, visibleConfidenceValues.length - 1);
        const recentLogs = workflowLogs.slice(-markerCount);
        return recentLogs.map((entry, index) => {
            const pointIndex = Math.round(((index + 1) / (markerCount + 1)) * (visibleConfidenceValues.length - 1));
            return {
                pointIndex,
                color: markerColorForLogLevel(entry.level),
                title: `${entry.level.toUpperCase()} - ${entry.message}`,
            };
        });
    }, [visibleConfidenceValues.length, workflowLogs]);

    const sessionAxes = useMemo(
        () => [
            { label: 'c', value: latestConfidence },
            { label: 'e', value: Math.min(epochCount, 36) / 36 },
            { label: 'f', value: flow },
            { label: 'g', value: latestGeometry ? Math.min(latestGeometry.clusters.length, 6) / 6 : 0 },
            { label: 'v', value: latestGeometry?.projectionMetadata.explainedVariance ?? 0 },
        ] as const,
        [epochCount, flow, latestConfidence, latestGeometry],
    );

    const geomAxes = useMemo(
        () => [
            { label: 'x', value: latestGeometry ? Math.min(Math.abs(latestGeometry.clusters[0]?.centroid.x ?? 0), 1) : 0 },
            { label: 'y', value: latestGeometry ? Math.min(Math.abs(latestGeometry.clusters[0]?.centroid.y ?? 0), 1) : 0 },
            { label: 'r', value: latestGeometry ? Math.min(latestGeometry.clusters[0]?.inscribedRadius ?? 0, 1) : 0 },
            { label: 's', value: spectralActivation === 'disabled' ? 0.25 : 1 },
            { label: 'v', value: latestGeometry?.projectionMetadata.explainedVariance ?? 0 },
        ] as const,
        [latestGeometry, spectralActivation],
    );

    const sessionBars = useMemo(
        () => [
            { label: 'Confidence', value: latestConfidence, color: '#2dd4bf' },
            { label: 'Epoch Density', value: Math.min(epochCount, 36) / 36, color: '#5eead4' },
            { label: 'Workflow Flow', value: flow, color: '#22d3ee' },
            { label: 'Clusters', value: latestGeometry ? Math.min(latestGeometry.clusters.length, 6) / 6 : 0, color: '#38bdf8' },
            { label: 'Variance', value: latestGeometry?.projectionMetadata.explainedVariance ?? 0, color: '#60a5fa' },
        ] as const,
        [epochCount, flow, latestConfidence, latestGeometry],
    );

    const geometryBars = useMemo(
        () => [
            { label: 'Centroid X', value: latestGeometry ? Math.min(Math.abs(latestGeometry.clusters[0]?.centroid.x ?? 0), 1) : 0, color: '#38bdf8' },
            { label: 'Centroid Y', value: latestGeometry ? Math.min(Math.abs(latestGeometry.clusters[0]?.centroid.y ?? 0), 1) : 0, color: '#60a5fa' },
            { label: 'Radius Fit', value: latestGeometry ? Math.min(latestGeometry.clusters[0]?.inscribedRadius ?? 0, 1) : 0, color: '#22d3ee' },
            { label: 'Spectral', value: spectralActivation === 'disabled' ? 0.25 : 1, color: '#0ea5e9' },
            { label: 'Variance', value: latestGeometry?.projectionMetadata.explainedVariance ?? 0, color: '#7dd3fc' },
        ] as const,
        [latestGeometry, spectralActivation],
    );

    const sessionHealthScore = Math.round(
        (sessionAxes.reduce((sum, axis) => sum + axis.value, 0) / sessionAxes.length) * 100,
    );
    const geometryHealthScore = Math.round(
        (geomAxes.reduce((sum, axis) => sum + axis.value, 0) / geomAxes.length) * 100,
    );

    const healthCards = [
        {
            key: 'session',
            title: 'Session health',
            titleColor: '#99f6e4',
            score: sessionHealthScore,
            scoreTone: sessionHealthScore >= 70 ? 'good' : sessionHealthScore < 45 ? 'bad' : 'default',
            radarAxes: sessionAxes,
            radarStroke: '#2dd4bf',
            radarFill: 'rgba(45, 212, 191, 0.2)',
            bars: sessionBars,
            summary: `confidence ${Math.round(latestConfidence * 100)}% • flow ${Math.round(flow * 100)}% • epochs ${epochCount}`,
            helper: 'fuller shape means stronger confidence, progress, and stability.',
            border: '1px solid rgba(45, 212, 191, 0.24)',
        },
        {
            key: 'geometry',
            title: 'Geometry health',
            titleColor: '#7dd3fc',
            score: geometryHealthScore,
            scoreTone: geometryHealthScore >= 70 ? 'good' : geometryHealthScore < 45 ? 'bad' : 'default',
            radarAxes: geomAxes,
            radarStroke: '#38bdf8',
            radarFill: 'rgba(56, 189, 248, 0.2)',
            bars: geometryBars,
            summary: `clusters ${latestGeometry?.clusters.length ?? 0} • spectral ${spectralActivation === 'disabled' ? 'off' : 'on'} • variance ${varianceSignal.toFixed(3)}`,
            helper: 'fuller shape means tighter geometry and more stable projection.',
            border: '1px solid rgba(56, 189, 248, 0.24)',
        },
    ] as const;

    return (
        <div
            style={{
                position: 'absolute',
                inset: 0,
                pointerEvents: 'none',
                zIndex: 2,
            }}
        >
            {/* Global Observability Strip */}
            <div
                style={{
                    position: 'absolute',
                    left: '50%',
                    transform: 'translateX(-50%)',
                    top: mobile ? 64 : compact ? 84 : 66,
                    width: mobile ? 'calc(100vw - 20px)' : compact ? 'min(94vw, 560px)' : 'min(76vw, 980px)',
                    borderRadius: 12,
                    border: '1px solid rgba(45, 212, 191, 0.24)',
                    background: mobile ? 'rgba(2, 10, 20, 0.58)' : 'linear-gradient(90deg, rgba(2, 10, 20, 0.82), rgba(9, 18, 36, 0.72))',
                    backdropFilter: 'blur(5px)',
                    padding: mobile ? '6px 8px' : compact ? '7px 8px' : '8px 10px',
                    display: 'grid',
                    gap: 6,
                    pointerEvents: mobile ? 'auto' : 'none',
                }}
            >
                {mobile ? (
                    <>
                        <div style={{ display: 'flex', gap: 7, alignItems: 'center', justifyContent: 'space-between' }}>
                            <SectionLabel color="#67e8f9">Observability</SectionLabel>
                            <button
                                type="button"
                                onClick={() => {
                                    if (!showTelemetryPanel) {
                                        trackInteraction('expand-details', 0.3, { flow });
                                    }
                                    setShowTelemetryPanel((prev) => !prev);
                                }}
                                style={{
                                    borderRadius: 999,
                                    border: '1px solid rgba(125, 211, 252, 0.32)',
                                    background: 'rgba(15, 23, 42, 0.55)',
                                    color: '#bae6fd',
                                    fontSize: 10,
                                    padding: '2px 8px',
                                    cursor: 'pointer',
                                }}
                            >
                                {showTelemetryPanel ? 'Hide Panel' : 'Show Panel'}
                            </button>
                        </div>
                        <div style={{ display: 'flex', gap: 6, alignItems: 'center', flexWrap: 'wrap', fontSize: 10, color: '#cbd5e1' }}>
                            <span style={{ padding: '1px 6px', borderRadius: 999, border: '1px solid rgba(125, 211, 252, 0.2)', background: 'rgba(15, 23, 42, 0.44)' }}>
                                conf {Math.round(latestConfidence * 100)}%
                            </span>
                            <span style={{ padding: '1px 6px', borderRadius: 999, border: '1px solid rgba(45, 212, 191, 0.2)', background: 'rgba(15, 23, 42, 0.44)' }}>
                                flow {Math.round(flow * 100)}%
                            </span>
                            <span style={{ padding: '1px 6px', borderRadius: 999, border: '1px solid rgba(251, 191, 36, 0.2)', background: 'rgba(15, 23, 42, 0.44)' }}>
                                xp {metrics.xpEarned}
                            </span>
                        </div>
                        <div style={{ fontSize: 9, color: '#94a3b8' }}>
                            {lastInteraction ? `last: ${lastInteraction.message}` : 'last: waiting for interaction'}
                        </div>
                    </>
                ) : (
                    <>
                        <div style={{ display: 'flex', gap: 7, alignItems: 'center', justifyContent: 'space-between', flexWrap: 'wrap' }}>
                            <SectionLabel color="#67e8f9">Global observability</SectionLabel>
                            <span style={{ fontSize: 10, color: '#94a3b8' }}>
                                interactions {interactionCount} • epochs {epochCount}
                            </span>
                        </div>

                        <div style={{ display: 'flex', gap: 6, alignItems: 'center', flexWrap: 'wrap' }}>
                            <MetricPill label="impact" value={formatSigned(globalDelta)} tone={globalDelta >= 0 ? 'good' : 'bad'} />
                            <MetricPill label="momentum" value={formatSigned(recentDelta)} tone={recentDelta >= 0 ? 'good' : 'bad'} />
                            <MetricPill label="yield" value={formatSigned(interactionYield)} tone={interactionYield >= 0 ? 'good' : 'bad'} />
                            <MetricPill label="alignment" value={`${k3h4Alignment}%`} />
                            <MetricPill label="variance" value={varianceSignal.toFixed(3)} />
                            <MetricPill label="flow" value={`${Math.round(flow * 100)}%`} />
                        </div>

                        <div style={{ display: 'grid', gridTemplateColumns: compact ? '1fr' : '2.2fr 1fr', gap: 8, alignItems: 'center' }}>
                            <div style={{ borderRadius: 999, border: '1px solid rgba(148, 163, 184, 0.22)', background: 'rgba(15, 23, 42, 0.45)', padding: '2px 8px' }}>
                                <TelemetrySparkline values={visibleConfidenceValues} width={compact ? 300 : 430} height={24} stroke={confidenceTrendColor} markers={interactionMarkers} />
                            </div>
                            <div style={{ fontSize: 10, color: '#94a3b8', textAlign: compact ? 'left' : 'right' }}>
                                {lastInteraction ? `last: ${lastInteraction.message}` : 'last: waiting for interaction'}
                            </div>
                        </div>
                        <div style={{ fontSize: 9, color: '#64748b' }}>
                            event ticks mark recent interaction logs (info/success/warn/error)
                        </div>
                    </>
                )}
            </div>

            {!mobile ? (
            <div
                style={{
                    position: 'absolute',
                    left: mobile ? 8 : 16,
                    right: mobile ? 8 : 'auto',
                    top: overlayTop,
                    display: 'grid',
                    gap: 8,
                    pointerEvents: 'none',
                }}
            >
                {healthCards.map((card) => (
                    <TelemetryHealthCard
                        key={card.key}
                        title={card.title}
                        titleColor={card.titleColor}
                        score={card.score}
                        scoreTone={card.scoreTone}
                        radarAxes={card.radarAxes}
                        radarStroke={card.radarStroke}
                        radarFill={card.radarFill}
                        bars={card.bars}
                        summary={card.summary}
                        helper={card.helper}
                        showRadar={showRadarVisual}
                        mobile={mobile || compact}
                        border={card.border}
                    />
                ))}
            </div>
            ) : null}

            {/* Gamified Action Panel */}
            <GamifiedActionPanel
                metrics={metrics}
                recentInteractions={recentInteractions}
                isBootstrapActive={workflowState.status === 'running'}
                isRelationsDisabled={!session || workflowState.status === 'running'}
                onBootstrap={() => {
                    trackInteraction('bootstrap', 0.8, { confidence: latestConfidence, alignment: k3h4Alignment });
                    orchestrateRealtimeTraining('power', 'bootstrap');
                }}
                onRelations={() => {
                    trackInteraction('relations', 0.9, { confidence: latestConfidence, alignment: k3h4Alignment });
                    runWorkflow('relations');
                }}
                mobile={mobile}
                overlayTop={overlayTop}
                mobileDrawerOpen={showTelemetryPanel}
            />

            {/* Bottom Sheet Detail Panel */}
            {showTelemetryPanel ? (
                <div
                    style={{
                        position: 'absolute',
                        right: mobile ? 8 : compact ? 16 : 60,
                        left: mobile ? 8 : 'auto',
                        top: mobile ? 'auto' : compact ? overlayTop + 122 : overlayTop,
                        bottom: mobile ? 12 : 'auto',
                        width: mobile ? 'auto' : 'min(340px, calc(100vw - 40px))',
                        maxHeight: mobile ? '34vh' : 'none',
                        overflowY: mobile ? 'auto' : 'visible',
                        borderRadius: 12,
                        border: '1px solid rgba(45, 212, 191, 0.24)',
                        background: mobile ? 'rgba(2, 10, 20, 0.54)' : 'rgba(2, 10, 20, 0.72)',
                        backdropFilter: 'blur(5px)',
                        padding: 8,
                        display: 'grid',
                        gap: 8,
                        pointerEvents: 'auto',
                    }}
                >
                    {mobile ? (
                        <div
                            style={{
                                display: 'flex',
                                justifyContent: 'space-between',
                                alignItems: 'center',
                                gap: 8,
                                paddingBottom: 4,
                                borderBottom: '1px solid rgba(148, 163, 184, 0.1)',
                            }}
                        >
                            <span style={{ fontSize: 10, color: '#94a3b8', textTransform: 'uppercase', letterSpacing: '0.06em' }}>
                                Details
                            </span>
                            <div style={{ display: 'flex', alignItems: 'center', gap: 4 }}>
                                <button
                                    type="button"
                                    onClick={() => setShowMobileDeepDetails((prev) => !prev)}
                                    style={{
                                        borderRadius: 999,
                                        border: '1px solid rgba(125, 211, 252, 0.24)',
                                        background: 'rgba(15, 23, 42, 0.45)',
                                        color: '#bae6fd',
                                        cursor: 'pointer',
                                        fontSize: 10,
                                        padding: '1px 8px',
                                    }}
                                >
                                    {showMobileDeepDetails ? 'Less' : 'More'}
                                </button>
                                <button
                                    type="button"
                                    onClick={() => {
                                        trackInteraction('collapse-details', 0.3, { flow });
                                        setShowTelemetryPanel(false);
                                    }}
                                    style={{
                                        background: 'none',
                                        border: 'none',
                                        color: '#94a3b8',
                                        cursor: 'pointer',
                                        fontSize: 14,
                                        padding: '0 4px',
                                    }}
                                    aria-label="Close"
                                >
                                    ×
                                </button>
                            </div>
                        </div>
                    ) : null}
                    {mobile && !showMobileDeepDetails ? (
                        <div
                            style={{
                                borderRadius: 8,
                                border: '1px solid rgba(56, 189, 248, 0.2)',
                                background: 'rgba(6, 18, 30, 0.54)',
                                padding: 7,
                                display: 'grid',
                                gap: 6,
                            }}
                        >
                            <div style={{ display: 'flex', gap: 6, flexWrap: 'wrap' }}>
                                <MetricPill label="state" value={workflowState.status} />
                                <MetricPill label="conf" value={latestConfidence.toFixed(3)} />
                                <MetricPill label="epochs" value={String(epochCount)} />
                            </div>
                            <div style={{ fontSize: 10, color: '#94a3b8' }}>
                                k3h4 {k3h4Clusters} clusters • align {k3h4Alignment}% • variance {varianceSignal.toFixed(3)}
                            </div>
                            <div style={{ display: 'grid', gap: 5, maxHeight: 72, overflowY: 'auto', borderRadius: 8, border: '1px solid rgba(148, 163, 184, 0.2)', padding: 6 }}>
                                {workflowLogs.length === 0 ? (
                                    <div style={{ fontSize: 11, color: '#64748b' }}>No logs yet</div>
                                ) : workflowLogs.slice(-2).map((entry) => (
                                    <div key={entry.id} style={{ fontSize: 10, color: '#cbd5e1', lineHeight: 1.35 }}>
                                        <span style={{ color: '#94a3b8' }}>[{new Date(entry.timestamp).toLocaleTimeString()}]</span>{' '}
                                        {entry.message}
                                    </div>
                                ))}
                            </div>
                        </div>
                    ) : null}
                    {!mobile || showMobileDeepDetails ? (
                    <div
                        style={{
                            borderRadius: 8,
                            border: '1px solid rgba(56, 189, 248, 0.22)',
                            background: 'rgba(6, 18, 30, 0.72)',
                            padding: 7,
                            display: 'grid',
                            gap: 6,
                        }}
                    >
                        <div>
                            <SectionLabel color="#7dd3fc">Rule Mining source of truth</SectionLabel>
                        </div>
                        <div style={{ display: 'flex', gap: 6, flexWrap: 'wrap' }}>
                            <MetricPill label="signal" value={`${lineCount} lines · ${includeCount} deps`} />
                            <MetricPill label="density" value={`${callDensity}%`} />
                            <MetricPill label="risk" value={riskLabel} />
                            <MetricPill label="cohort" value={analyticsCohort} />
                        </div>
                        <div style={{ display: 'flex', gap: 6, flexWrap: 'wrap' }}>
                            <MetricPill label="k3h4" value={`${k3h4Clusters} clusters`} />
                            <MetricPill label="shape" value={`D${k3h4Dimensions} · Align ${k3h4Alignment}%`} />
                            <MetricPill label="abi" value={`${abiCoverageSummary} (${abiCoveragePercent}%)`} />
                            <MetricPill label="hash" value={k3h4HashPreview} />
                        </div>
                        <div style={{ fontSize: 10, color: '#94a3b8', display: 'flex', gap: 8, flexWrap: 'wrap' }}>
                            <span>Convergence {k3h4Convergence}</span>
                            <span>C files {cFileCount}</span>
                            <span>ML conf {modelConfidence}%</span>
                            <span>{analyticsAvailable ? 'Analytics online' : 'Analytics offline'}</span>
                        </div>
                    </div>
                    ) : null}

                    {!mobile || showMobileDeepDetails ? (
                    <div style={{ display: 'grid', gap: 6 }}>
                        <SectionLabel>Telemetry legend</SectionLabel>
                        <div style={{ display: 'grid', gap: 4 }}>
                            <InfoLine code="c/e/f/g/v" text="confidence, epoch density, flow, clusters, variance" />
                            <InfoLine code="x/y/r/s/v" text="centroid X/Y, radius, spectral state, variance" />
                            <InfoLine code="line" text="confidence trend over recent epochs" />
                        </div>
                    </div>
                    ) : null}

                    {!mobile || showMobileDeepDetails ? (
                    <div style={{ display: 'grid', gap: 6 }}>
                        <div style={{ display: 'flex', gap: 6, flexWrap: 'wrap' }}>
                            <MetricPill label="state" value={workflowState.status} />
                            <MetricPill label="epochs" value={String(epochCount)} />
                            <MetricPill label="conf" value={latestConfidence.toFixed(3)} />
                        </div>
                    </div>
                    ) : null}

                    {!mobile || showMobileDeepDetails ? (
                    <div style={{ display: 'grid', gap: 5, maxHeight: mobile ? 72 : 142, overflowY: 'auto', borderRadius: 8, border: '1px solid rgba(148, 163, 184, 0.2)', padding: 6 }}>
                        {workflowLogs.length === 0 ? (
                            <div style={{ fontSize: 11, color: '#64748b' }}>No logs yet</div>
                        ) : workflowLogs.slice(mobile ? -2 : -7).map((entry) => (
                            <div key={entry.id} style={{ fontSize: 10, color: '#cbd5e1', lineHeight: 1.35 }}>
                                <span style={{ color: '#94a3b8' }}>[{new Date(entry.timestamp).toLocaleTimeString()}]</span>{' '}
                                {entry.message}
                            </div>
                        ))}
                    </div>
                    ) : null}
                </div>
            ) : null}

            {!mobile ? (
            <div
                style={{
                    position: 'absolute',
                    left: '50%',
                    transform: 'translateX(-50%)',
                    bottom: sparklineBottom,
                    borderRadius: 999,
                    border: '1px solid rgba(45, 212, 191, 0.24)',
                    background: 'rgba(2, 10, 20, 0.34)',
                    padding: '4px 8px',
                    maxWidth: 'min(560px, calc(100vw - 120px))',
                    pointerEvents: 'none',
                }}
                title="confidence trend"
            >
                <TelemetrySparkline values={visibleConfidenceValues} width={compact ? 180 : 220} stroke={confidenceTrendColor} markers={interactionMarkers} />
            </div>
            ) : null}
        </div>
    );
}

function formatSigned(value: number): string {
    const sign = value >= 0 ? '+' : '';
    return `${sign}${value.toFixed(3)}`;
}
