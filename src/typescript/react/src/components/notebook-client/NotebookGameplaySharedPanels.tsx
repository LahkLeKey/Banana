import type { ReactNode } from 'react';

import type { ContractNodeId, NodeInteractionAction } from '../../domain/notebook/network-domain';
import type { WorkflowStepId } from '../../domain/notebook/workflow-domain';

export type DockLoopId = 'intel' | 'objectives' | 'player';

export type RewardBandContract = {
    readonly glow: string;
    readonly line: string;
};

export type QuestLoopItem = {
    readonly id: DockLoopId;
    readonly label: string;
    readonly value: number;
    readonly detail: string;
    readonly accent: string;
};

type TelemetryRailProps = {
    readonly leftRailInset: number;
    readonly leftTelemetryBottom: number;
    readonly lineCount: number;
    readonly includeCount: number;
    readonly callDensity: number;
    readonly riskLabel: string;
    readonly hypersphereDimensions: number;
    readonly hypersphereAlignment: number;
    readonly cFileCount: number;
    readonly modelConfidence: number;
    readonly extraLines?: readonly string[];
};

export function TelemetryRail(props: TelemetryRailProps) {
    const {
        leftRailInset,
        leftTelemetryBottom,
        lineCount,
        includeCount,
        callDensity,
        riskLabel,
        hypersphereDimensions,
        hypersphereAlignment,
        cFileCount,
        modelConfidence,
        extraLines = [],
    } = props;

    return (
        <div style={{
            position: 'absolute',
            left: leftRailInset,
            bottom: leftTelemetryBottom,
            width: 160,
            display: 'grid',
            gap: 8,
            zIndex: 3,
            pointerEvents: 'none',
        }}>
            <div style={{
                borderRadius: 10,
                border: '1px solid rgba(148, 163, 184, 0.24)',
                background: 'linear-gradient(180deg, rgba(5, 12, 24, 0.82), rgba(3, 9, 19, 0.82))',
                padding: '8px 9px',
                display: 'grid',
                gap: 6,
            }}>
                <div style={{ fontSize: 9, color: '#93c5fd', letterSpacing: '0.06em', textTransform: 'uppercase', fontWeight: 700 }}>Signal</div>
                <div style={{ fontSize: 10, color: '#e2e8f0' }}>{lineCount} lines · {includeCount} deps</div>
                <div style={{ fontSize: 10, color: '#e2e8f0' }}>{callDensity}% density</div>
                <div style={{ fontSize: 10, color: riskLabel === 'Critical' ? '#fda4af' : riskLabel === 'Elevated' ? '#fcd34d' : '#86efac' }}>{riskLabel}</div>
            </div>

            <div style={{
                borderRadius: 10,
                border: '1px solid rgba(148, 163, 184, 0.24)',
                background: 'linear-gradient(180deg, rgba(5, 12, 24, 0.82), rgba(3, 9, 19, 0.82))',
                padding: '8px 9px',
                display: 'grid',
                gap: 6,
            }}>
                <div style={{ fontSize: 9, color: '#a5b4fc', letterSpacing: '0.06em', textTransform: 'uppercase', fontWeight: 700 }}>Hypersphere</div>
                <div style={{ fontSize: 10, color: '#e2e8f0' }}>D{hypersphereDimensions} · N/2 {Math.max(2, Math.ceil(hypersphereDimensions / 2))}</div>
                <div style={{ fontSize: 10, color: '#e2e8f0' }}>Align {hypersphereAlignment}%</div>
                <div style={{ fontSize: 10, color: '#7dd3fc' }}>C files {cFileCount}</div>
                <div style={{ fontSize: 10, color: '#a7f3d0' }}>ML conf {modelConfidence}%</div>
                {extraLines.map((line) => (
                    <div key={line} style={{ fontSize: 10, color: '#cbd5e1' }}>{line}</div>
                ))}
            </div>
        </div>
    );
}

type QuestLogRailProps = {
    readonly rightRailInset: number;
    readonly questRailBottom: number | string;
    readonly questRailWidth: string;
    readonly expandedQuestRail: boolean;
    readonly onToggleExpanded: () => void;
    readonly workflowStepLabel: string;
    readonly loopItems: readonly QuestLoopItem[];
    readonly activeDock: DockLoopId | null;
    readonly onToggleLoopDock: (dockId: DockLoopId) => void;
    readonly rewardBand: RewardBandContract;
    readonly questListMaxHeight: string;
    readonly projectedRewardXp: number;
    readonly statusLine?: string;
    readonly onMainAction: () => void;
    readonly onClose?: () => void;
};

export function QuestLogRail(props: QuestLogRailProps) {
    const {
        rightRailInset,
        questRailBottom,
        questRailWidth,
        expandedQuestRail,
        onToggleExpanded,
        workflowStepLabel,
        loopItems,
        activeDock,
        onToggleLoopDock,
        rewardBand,
        questListMaxHeight,
        projectedRewardXp,
        statusLine,
        onMainAction,
        onClose,
    } = props;

    return (
        <div style={{
            position: 'absolute',
            right: rightRailInset,
            bottom: questRailBottom,
            width: questRailWidth,
            borderRadius: 12,
            border: '1px solid rgba(148, 163, 184, 0.14)',
            background: 'linear-gradient(180deg, rgba(4, 13, 26, 0.68), rgba(3, 10, 20, 0.64))',
            boxShadow: '0 8px 16px rgba(2, 6, 23, 0.2)',
            backdropFilter: 'blur(2px)',
            padding: 7,
            display: 'grid',
            gap: 6,
            zIndex: 4,
            maxHeight: expandedQuestRail ? '48dvh' : undefined,
            overflow: 'hidden',
        }}>
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', gap: 8, position: 'relative', zIndex: 2 }}>
                <div style={{ fontSize: 9, color: '#99f6e4', letterSpacing: '0.07em', textTransform: 'uppercase', fontWeight: 700 }}>
                    Quest Log
                </div>
                <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
                    {onClose ? (
                        <button
                            type="button"
                            onClick={onClose}
                            style={{
                                border: '1px solid rgba(148, 163, 184, 0.34)',
                                borderRadius: 999,
                                background: 'rgba(8, 13, 28, 0.55)',
                                color: '#cbd5e1',
                                fontSize: 8,
                                fontWeight: 700,
                                letterSpacing: '0.04em',
                                padding: '2px 8px',
                                cursor: 'pointer',
                                textTransform: 'uppercase',
                                whiteSpace: 'nowrap',
                            }}
                            title="Close quest log"
                        >
                            Close
                        </button>
                    ) : null}
                    <button
                        type="button"
                        onClick={onToggleExpanded}
                        style={{
                            borderRadius: 6,
                            border: '1px solid rgba(45, 212, 191, 0.3)',
                            background: expandedQuestRail ? 'rgba(45, 212, 191, 0.14)' : 'rgba(8, 47, 73, 0.4)',
                            color: '#5eead4',
                            padding: '2px 7px',
                            fontSize: 8,
                            fontWeight: 700,
                            textTransform: 'uppercase',
                            cursor: 'pointer',
                        }}
                        title={expandedQuestRail ? 'Compact quest rail' : 'Expand quest rail'}
                    >
                        {expandedQuestRail ? 'Compact' : 'Expand'}
                    </button>
                    <div style={{ fontSize: 8, color: '#94a3b8', letterSpacing: '0.05em', textTransform: 'uppercase' }}>
                        {workflowStepLabel}
                    </div>
                    {statusLine ? (
                        <div style={{ fontSize: 9, color: '#cbd5e1', letterSpacing: '0.04em', textTransform: 'uppercase' }}>
                            {statusLine}
                        </div>
                    ) : null}
                </div>
            </div>

            <div style={{ display: 'grid', gap: 5, maxHeight: questListMaxHeight, overflowY: 'auto', paddingRight: 2, position: 'relative', zIndex: 1 }}>
                {loopItems.map((loop, index) => {
                    const active = activeDock === loop.id;
                    return (
                        <button
                            key={loop.label}
                            type="button"
                            onClick={() => onToggleLoopDock(loop.id)}
                            style={{
                                borderRadius: 8,
                                border: active ? `1px solid ${loop.accent}66` : '1px solid rgba(148, 163, 184, 0.16)',
                                background: active ? 'rgba(8, 47, 73, 0.34)' : 'rgba(2, 10, 22, 0.42)',
                                color: '#e2e8f0',
                                padding: '6px 8px',
                                textAlign: 'left',
                                cursor: 'pointer',
                                boxShadow: active ? `0 0 8px ${rewardBand.glow}` : 'none',
                                display: 'grid',
                                gap: 3,
                            }}
                        >
                            <div style={{ display: 'flex', justifyContent: 'space-between', gap: 8, alignItems: 'center' }}>
                                <div style={{ display: 'flex', alignItems: 'center', gap: 8, minWidth: 0 }}>
                                    <span style={{
                                        width: 18,
                                        height: 18,
                                        borderRadius: 999,
                                        border: `1px solid ${loop.accent}`,
                                        color: loop.accent,
                                        display: 'inline-flex',
                                        alignItems: 'center',
                                        justifyContent: 'center',
                                        fontSize: 9,
                                        fontWeight: 700,
                                        flexShrink: 0,
                                    }}>
                                        {index + 1}
                                    </span>
                                    <span style={{ fontSize: 9, textTransform: 'uppercase', letterSpacing: '0.06em', color: loop.accent, fontWeight: 700 }}>
                                        {loop.label}
                                    </span>
                                </div>
                                <span style={{ fontSize: 9, fontWeight: 700, color: '#cbd5e1' }}>{loop.value}%</span>
                            </div>
                            <div style={{ fontSize: 8, color: '#94a3b8', whiteSpace: 'nowrap', overflow: 'hidden', textOverflow: 'ellipsis' }}>
                                {loop.detail}
                            </div>
                            <div style={{ height: 3, borderRadius: 999, background: 'rgba(15, 23, 42, 0.9)', overflow: 'hidden' }}>
                                <div style={{ width: `${loop.value}%`, height: '100%', background: loop.accent, transition: 'width 220ms ease' }} />
                            </div>
                        </button>
                    );
                })}
            </div>

            <button
                type="button"
                onClick={onMainAction}
                style={{
                    borderRadius: 9,
                    border: '1px solid rgba(16, 185, 129, 0.28)',
                    background: 'linear-gradient(180deg, rgba(5, 46, 22, 0.58), rgba(6, 34, 18, 0.54))',
                    color: '#d1fae5',
                    padding: '6px 8px',
                    textAlign: 'left',
                    cursor: 'pointer',
                    display: 'grid',
                    gap: 3,
                }}
            >
                <div style={{ display: 'flex', justifyContent: 'space-between', gap: 8, alignItems: 'baseline' }}>
                    <span style={{ fontSize: 9, textTransform: 'uppercase', letterSpacing: '0.06em', color: '#86efac', fontWeight: 700 }}>Main Action</span>
                    <span style={{ fontSize: 10, fontWeight: 700 }}>+{projectedRewardXp} XP</span>
                </div>
                <div style={{ fontSize: 8, color: '#a7f3d0', whiteSpace: 'nowrap', overflow: 'hidden', textOverflow: 'ellipsis' }}>
                    Commit workflow and route to player lane
                </div>
            </button>
        </div>
    );
}

type ActiveDockShellProps = {
    readonly rightRailInset: number;
    readonly dockPanelBottom: number;
    readonly dockPanelWidth: string;
    readonly dockPanelMaxHeight: string;
    readonly dockTitle: string;
    readonly expandedDockPanel: boolean;
    readonly onToggleExpanded: () => void;
    readonly onClose: () => void;
    readonly summaryLine: string;
    readonly summaryColor: string;
    readonly children: ReactNode;
};

export function ActiveDockShell(props: ActiveDockShellProps) {
    const {
        rightRailInset,
        dockPanelBottom,
        dockPanelWidth,
        dockPanelMaxHeight,
        dockTitle,
        expandedDockPanel,
        onToggleExpanded,
        onClose,
        summaryLine,
        summaryColor,
        children,
    } = props;

    return (
        <div style={{
            position: 'absolute',
            right: rightRailInset,
            bottom: dockPanelBottom,
            width: dockPanelWidth,
            zIndex: 4,
            display: 'grid',
            gap: 8,
        }}>
            <div style={{
                borderRadius: 14,
                border: '1px solid rgba(148, 163, 184, 0.14)',
                background: 'linear-gradient(180deg, rgba(2, 10, 22, 0.7), rgba(2, 8, 20, 0.66))',
                boxShadow: '0 8px 16px rgba(2, 6, 23, 0.2)',
                backdropFilter: 'blur(2px)',
                padding: 8,
                maxHeight: dockPanelMaxHeight,
                overflow: 'hidden',
                display: 'grid',
                gap: 8,
            }}>
                <div style={{
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'space-between',
                    gap: 10,
                    borderBottom: '1px solid rgba(148, 163, 184, 0.2)',
                    paddingBottom: 6,
                }}>
                    <div style={{ display: 'grid', gap: 2 }}>
                        <div style={{ fontSize: 10, color: '#99f6e4', letterSpacing: '0.06em', textTransform: 'uppercase', fontWeight: 700 }}>
                            {dockTitle}
                        </div>
                        <div style={{ fontSize: 9, color: '#94a3b8' }}>
                            Switch nodes from the workflow command lane
                        </div>
                    </div>
                    <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
                        <button
                            type="button"
                            onClick={onToggleExpanded}
                            style={{
                                borderRadius: 6,
                                border: '1px solid rgba(45, 212, 191, 0.3)',
                                background: expandedDockPanel ? 'rgba(45, 212, 191, 0.14)' : 'rgba(8, 47, 73, 0.4)',
                                color: '#5eead4',
                                padding: '3px 8px',
                                fontSize: 9,
                                fontWeight: 700,
                                letterSpacing: '0.04em',
                                cursor: 'pointer',
                                textTransform: 'uppercase',
                                whiteSpace: 'nowrap',
                            }}
                            title={expandedDockPanel ? 'Compact dock panel' : 'Expand dock panel'}
                        >
                            {expandedDockPanel ? 'Compact' : 'Expand'}
                        </button>
                        <button
                            type="button"
                            onClick={onClose}
                            style={{
                                border: '1px solid rgba(148, 163, 184, 0.34)',
                                borderRadius: 999,
                                background: 'rgba(8, 13, 28, 0.55)',
                                color: '#cbd5e1',
                                fontSize: 10,
                                fontWeight: 700,
                                letterSpacing: '0.04em',
                                padding: '4px 10px',
                                cursor: 'pointer',
                                textTransform: 'uppercase',
                                whiteSpace: 'nowrap',
                            }}
                        >
                            Close
                        </button>
                    </div>
                </div>

                <div style={{
                    fontSize: 10,
                    color: summaryColor,
                    letterSpacing: '0.05em',
                    textTransform: 'uppercase',
                }}>
                    {summaryLine}
                </div>

                <div style={{ maxHeight: '24dvh', overflowY: 'auto', paddingRight: 4 }}>
                    {children}
                </div>
            </div>
        </div>
    );
}

type GameplayTopHudProps = {
    readonly selectedFile: string;
    readonly missionCode: string;
    readonly missionTitle: string;
    readonly routeMode: string;
    readonly isCompactViewport: boolean;
    readonly hypersphereDimensions: number;
};

export function GameplayTopHud(props: GameplayTopHudProps) {
    const {
        selectedFile,
        missionCode,
        missionTitle,
        routeMode,
        isCompactViewport,
        hypersphereDimensions,
    } = props;

    return (
        <div style={{
            position: 'absolute',
            top: 6,
            left: 12,
            right: 12,
            height: 22,
            display: 'grid',
            gridTemplateColumns: '1fr auto 1fr',
            alignItems: 'center',
            zIndex: 3,
            pointerEvents: 'none',
        }}>
            <div style={{
                justifySelf: 'start',
                maxWidth: '100%',
                overflow: 'hidden',
                textOverflow: 'ellipsis',
                whiteSpace: 'nowrap',
                fontSize: 9,
                color: 'rgba(148, 163, 184, 0.68)',
                letterSpacing: '0.06em',
                textTransform: 'uppercase',
                visibility: isCompactViewport ? 'hidden' : 'visible',
            }}>
                {selectedFile ? missionCode : `mode ${routeMode}`}
            </div>
            <div style={{
                fontSize: 10,
                color: 'rgba(103, 232, 249, 0.78)',
                textTransform: 'uppercase',
                letterSpacing: '0.1em',
                whiteSpace: 'nowrap',
            }}>
                {selectedFile ? `Sector Live Feed - ${missionTitle}` : 'Rule Mining Constellation'}
            </div>
            <div style={{
                justifySelf: 'end',
                maxWidth: '100%',
                overflow: 'hidden',
                textOverflow: 'ellipsis',
                whiteSpace: 'nowrap',
                fontSize: 9,
                color: 'rgba(148, 163, 184, 0.68)',
                letterSpacing: '0.06em',
                textTransform: 'uppercase',
                visibility: isCompactViewport ? 'hidden' : 'visible',
            }}>
                Hyper D{hypersphereDimensions}
            </div>
        </div>
    );
}

type GameplayCoreNodeProps = {
    readonly selectedFile: string;
    readonly accent: string;
    readonly aura: string;
    readonly hotspotScore: number;
    readonly riskLabel: string;
    readonly missionTitle: string;
    readonly missionDomain: string;
    readonly rarityLabel: string;
    readonly workflowOrder: readonly WorkflowStepId[];
    readonly activeWorkflowStep: WorkflowStepId;
    readonly onSetWorkflowStep: (step: WorkflowStepId) => void;
    readonly workflowDepth: number;
    readonly onAdjustWorkflowDepth: (delta: -1 | 1) => void;
    readonly workflowSummary: string;
    readonly indexedFileCount: number;
    readonly notebookCellCount: number;
    readonly onCycleCore: () => void;
};

export function GameplayCoreNode(props: GameplayCoreNodeProps) {
    const {
        selectedFile,
        accent,
        aura,
        hotspotScore,
        riskLabel,
        missionTitle,
        missionDomain,
        rarityLabel,
        workflowOrder,
        activeWorkflowStep,
        onSetWorkflowStep,
        workflowDepth,
        onAdjustWorkflowDepth,
        workflowSummary,
        indexedFileCount,
        notebookCellCount,
        onCycleCore,
    } = props;

    return (
        <div style={{
            position: 'absolute',
            left: '50%',
            top: '48%',
            transform: 'translate(-50%, -50%)',
            textAlign: 'center',
            pointerEvents: 'auto',
            zIndex: 3,
        }}>
            <button
                type="button"
                onClick={onCycleCore}
                aria-label="Cycle workflow step"
                style={{ border: 'none', background: 'transparent', padding: 0, cursor: 'pointer' }}
            >
                <div style={{
                    width: 72,
                    height: 72,
                    borderRadius: '50%',
                    border: `2px solid ${accent}`,
                    background: `radial-gradient(circle, ${aura}, rgba(2,6,23,0.85) 70%)`,
                    boxShadow: `0 0 28px ${aura}, 0 0 8px ${accent}44`,
                    display: 'flex',
                    flexDirection: 'column',
                    alignItems: 'center',
                    justifyContent: 'center',
                    margin: '0 auto',
                    animation: 'bng-pulse-ring 3s ease-in-out infinite',
                }}>
                    {selectedFile ? (
                        <>
                            <div style={{ fontSize: 18, fontWeight: 800, color: accent, lineHeight: 1 }}>{hotspotScore}</div>
                            <div style={{ fontSize: 9, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#94a3b8', marginTop: 2 }}>{riskLabel}</div>
                        </>
                    ) : (
                        <div style={{ fontSize: 9, color: '#64748b', textTransform: 'uppercase', letterSpacing: '0.07em', textAlign: 'center', padding: '0 6px' }}>
                            All Sectors
                        </div>
                    )}
                </div>
            </button>
            {selectedFile ? (
                <div style={{ marginTop: 6, display: 'grid', gap: 6, placeItems: 'center' }}>
                    <div style={{ fontSize: 11, fontWeight: 700, color: '#e2e8f0', whiteSpace: 'nowrap' }}>{missionTitle}</div>
                    <div style={{ fontSize: 10, color: accent, textTransform: 'uppercase', letterSpacing: '0.06em' }}>{missionDomain} · {rarityLabel}</div>
                    <div style={{ display: 'flex', gap: 4 }}>
                        {workflowOrder.map((step) => (
                            <button
                                key={step}
                                type="button"
                                onClick={() => onSetWorkflowStep(step)}
                                style={{
                                    borderRadius: 999,
                                    border: activeWorkflowStep === step ? `1px solid ${accent}` : '1px solid rgba(148, 163, 184, 0.32)',
                                    background: activeWorkflowStep === step ? 'rgba(8, 47, 73, 0.7)' : 'rgba(2, 10, 22, 0.64)',
                                    color: activeWorkflowStep === step ? accent : '#94a3b8',
                                    fontSize: 9,
                                    textTransform: 'uppercase',
                                    letterSpacing: '0.06em',
                                    padding: '3px 7px',
                                    cursor: 'pointer',
                                    fontWeight: 700,
                                }}
                            >
                                {step}
                            </button>
                        ))}
                    </div>
                    <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
                        <button
                            type="button"
                            onClick={() => onAdjustWorkflowDepth(-1)}
                            aria-label="Decrease workflow depth"
                            style={{
                                width: 22,
                                height: 22,
                                borderRadius: 999,
                                border: '1px solid rgba(148, 163, 184, 0.35)',
                                background: 'rgba(2, 10, 22, 0.64)',
                                color: '#cbd5e1',
                                fontWeight: 700,
                                cursor: 'pointer',
                                lineHeight: 1,
                            }}
                        >
                            -
                        </button>
                        <span style={{ fontSize: 10, color: '#94a3b8', textTransform: 'uppercase', letterSpacing: '0.06em' }}>
                            Depth {workflowDepth}
                        </span>
                        <button
                            type="button"
                            onClick={() => onAdjustWorkflowDepth(1)}
                            aria-label="Increase workflow depth"
                            style={{
                                width: 22,
                                height: 22,
                                borderRadius: 999,
                                border: '1px solid rgba(148, 163, 184, 0.35)',
                                background: 'rgba(2, 10, 22, 0.64)',
                                color: '#cbd5e1',
                                fontWeight: 700,
                                cursor: 'pointer',
                                lineHeight: 1,
                            }}
                        >
                            +
                        </button>
                    </div>
                    <div style={{ fontSize: 9, color: '#64748b', maxWidth: 240, whiteSpace: 'nowrap', overflow: 'hidden', textOverflow: 'ellipsis' }}>
                        {workflowSummary}
                    </div>
                </div>
            ) : (
                <div style={{ marginTop: 6, fontSize: 11, color: '#475569' }}>
                    Mining {indexedFileCount} sectors · {notebookCellCount} cells
                </div>
            )}
        </div>
    );
}

type NodeOrbAnchors = {
    readonly intel: { x: number; y: number; };
    readonly objectives: { x: number; y: number; };
    readonly player: { x: number; y: number; };
    readonly ops: { x: number; y: number; };
};

export type OrbAnchorSet = {
    readonly center: { x: number; y: number; };
    readonly intel: { x: number; y: number; };
    readonly objectives: { x: number; y: number; };
    readonly player: { x: number; y: number; };
    readonly ops: { x: number; y: number; };
};

export type OrbitLayerSpec = {
    readonly key: string;
    readonly radius: number;
    readonly squash: number;
    readonly rotate: number;
    readonly opacity: number;
    readonly strokeWidth: number;
};

type GalaxyNodeSpec = {
    readonly key: string;
    readonly x: number;
    readonly y: number;
    readonly radius: number;
    readonly opacity: number;
    readonly accent: string;
    readonly glowRadius: number;
    readonly glowOpacity: number;
    readonly twinkleDur: number;
    readonly twinkleDelay: number;
    readonly depthBand: 'near' | 'mid' | 'far';
};

type BranchLinkSpec = {
    readonly key: string;
    readonly from: ContractNodeId;
    readonly to: ContractNodeId;
    readonly branchStrength: number;
    readonly phase: number;
    readonly curvature: number;
};

type LanePulsePhaseSpec = {
    readonly intel: { forward: number; reverse: number; };
    readonly objectives: { forward: number; reverse: number; };
    readonly player: { forward: number; reverse: number; };
    readonly ops: { forward: number; reverse: number; };
};

type GameplayGalaxyFieldProps = {
    readonly nodeAnchors: OrbAnchorSet;
    readonly hypersphereOrbitLayers: readonly OrbitLayerSpec[];
    readonly routeModeOrbitAccent: string;
    readonly renderedGalaxyNodes: readonly GalaxyNodeSpec[];
    readonly peripheralBranchMesh: readonly BranchLinkSpec[];
    readonly accent: string;
    readonly rewardBand: { line: string; soft: string; };
    readonly activeDock: DockLoopId | null;
    readonly linkConfidence: {
        readonly intel: number;
        readonly objectives: number;
        readonly player: number;
        readonly ops: number;
    };
    readonly laneContractCoherence: {
        readonly intel: number;
        readonly objectives: number;
        readonly player: number;
        readonly ops: number;
    };
    readonly lanePulsePhase: LanePulsePhaseSpec;
    readonly hotspotScore: number;
};

export function GameplayGalaxyField(props: GameplayGalaxyFieldProps) {
    const {
        nodeAnchors,
        hypersphereOrbitLayers,
        routeModeOrbitAccent,
        renderedGalaxyNodes,
        peripheralBranchMesh,
        accent,
        rewardBand,
        activeDock,
        linkConfidence,
        laneContractCoherence,
        lanePulsePhase,
        hotspotScore,
    } = props;

    return (
        <svg aria-hidden="true" viewBox="0 0 100 100" preserveAspectRatio="none"
            style={{ position: 'absolute', inset: 0, width: '100%', height: '100%', pointerEvents: 'none' }}>
            <defs>
                <pattern id="bng-grid" width="8" height="8" patternUnits="userSpaceOnUse">
                    <path d="M 8 0 L 0 0 0 8" fill="none" stroke="rgba(148,163,184,0.07)" strokeWidth="0.3" />
                </pattern>
                <filter id="bng-galaxy-near-focus" x="-50%" y="-50%" width="200%" height="200%">
                    <feGaussianBlur stdDeviation="0.05" />
                </filter>
                <filter id="bng-galaxy-far-blur" x="-50%" y="-50%" width="200%" height="200%">
                    <feGaussianBlur stdDeviation="0.18" />
                </filter>
                <filter id="bng-galaxy-dust-blur" x="-50%" y="-50%" width="200%" height="200%">
                    <feGaussianBlur stdDeviation="0.34" />
                </filter>
                <linearGradient id="bng-galaxy-dust-gradient" x1="0%" y1="0%" x2="100%" y2="0%">
                    <stop offset="0%" stopColor="rgba(2,6,23,0)" />
                    <stop offset="24%" stopColor="rgba(2,6,23,0.48)" />
                    <stop offset="50%" stopColor="rgba(2,6,23,0.7)" />
                    <stop offset="76%" stopColor="rgba(2,6,23,0.48)" />
                    <stop offset="100%" stopColor="rgba(2,6,23,0)" />
                </linearGradient>
            </defs>
            <rect width="100" height="100" fill="url(#bng-grid)" />

            {hypersphereOrbitLayers.map((layer) => (
                <ellipse
                    key={layer.key}
                    cx={nodeAnchors.center.x}
                    cy={nodeAnchors.center.y}
                    rx={layer.radius}
                    ry={layer.radius * layer.squash}
                    fill="none"
                    stroke={routeModeOrbitAccent}
                    strokeOpacity={layer.opacity}
                    strokeWidth={layer.strokeWidth}
                    transform={`rotate(${layer.rotate} ${nodeAnchors.center.x} ${nodeAnchors.center.y})`}
                />
            ))}

            <ellipse
                cx={nodeAnchors.center.x}
                cy={nodeAnchors.center.y + 1.6}
                rx={(hypersphereOrbitLayers[hypersphereOrbitLayers.length - 1]?.radius ?? 18) * 1.12}
                ry={(hypersphereOrbitLayers[hypersphereOrbitLayers.length - 1]?.radius ?? 18) * 0.58}
                fill="rgba(10, 25, 49, 0.18)"
            />

            <ellipse
                cx={nodeAnchors.center.x}
                cy={nodeAnchors.center.y}
                rx={hypersphereOrbitLayers[hypersphereOrbitLayers.length - 1]?.radius ?? 18}
                ry={(hypersphereOrbitLayers[hypersphereOrbitLayers.length - 1]?.radius ?? 18) * 0.52}
                fill="none"
                stroke="rgba(59, 130, 246, 0.14)"
                strokeWidth="1.2"
            />

            <g style={{ transformOrigin: `${nodeAnchors.center.x}% ${nodeAnchors.center.y}%`, animation: 'bng-galaxy-spin 140s linear infinite' }}>
                <g style={{ animation: 'bng-galaxy-far 31s ease-in-out infinite' }} filter="url(#bng-galaxy-far-blur)">
                    {renderedGalaxyNodes.filter((node) => node.depthBand === 'far').map((node) => (
                        <g key={node.key}>
                            <circle
                                cx={node.x}
                                cy={node.y}
                                r={node.glowRadius * 1.16}
                                fill={node.accent}
                                fillOpacity={node.glowOpacity * 0.72}
                            />
                            <circle
                                cx={node.x}
                                cy={node.y}
                                r={node.radius * 0.92}
                                fill={node.accent}
                                fillOpacity={node.opacity * 0.72}
                            />
                        </g>
                    ))}
                </g>
                <g style={{ animation: 'bng-galaxy-mid 23s ease-in-out infinite' }}>
                    {renderedGalaxyNodes.filter((node) => node.depthBand === 'mid').map((node) => (
                        <g key={node.key}>
                            <circle
                                cx={node.x}
                                cy={node.y}
                                r={node.glowRadius}
                                fill={node.accent}
                                fillOpacity={node.glowOpacity}
                            />
                            <circle
                                cx={node.x}
                                cy={node.y}
                                r={node.radius}
                                fill={node.accent}
                                fillOpacity={node.opacity}
                            >
                                <animate
                                    attributeName="opacity"
                                    values={`${Math.max(0.16, node.opacity * 0.65)};${Math.min(0.98, node.opacity * 1.24)};${Math.max(0.16, node.opacity * 0.65)}`}
                                    dur={`${node.twinkleDur}s`}
                                    begin={`${node.twinkleDelay}s`}
                                    repeatCount="indefinite"
                                />
                            </circle>
                        </g>
                    ))}
                </g>
                <g style={{ animation: 'bng-galaxy-near 17s ease-in-out infinite' }} filter="url(#bng-galaxy-near-focus)">
                    {renderedGalaxyNodes.filter((node) => node.depthBand === 'near').map((node) => (
                        <g key={node.key}>
                            <circle
                                cx={node.x}
                                cy={node.y}
                                r={node.glowRadius * 1.22}
                                fill={node.accent}
                                fillOpacity={Math.min(0.82, node.glowOpacity * 1.3)}
                            />
                            <circle
                                cx={node.x}
                                cy={node.y}
                                r={node.radius * 1.12}
                                fill={node.accent}
                                fillOpacity={Math.min(0.96, node.opacity * 1.18)}
                            >
                                <animate
                                    attributeName="opacity"
                                    values={`${Math.max(0.24, node.opacity * 0.78)};${Math.min(0.99, node.opacity * 1.34)};${Math.max(0.24, node.opacity * 0.78)}`}
                                    dur={`${Math.max(1.6, node.twinkleDur - 0.35)}s`}
                                    begin={`${node.twinkleDelay}s`}
                                    repeatCount="indefinite"
                                />
                            </circle>
                        </g>
                    ))}
                </g>
            </g>

            <g filter="url(#bng-galaxy-dust-blur)" opacity="0.8">
                <path
                    d={`M ${nodeAnchors.center.x - 20} ${nodeAnchors.center.y - 2.8} C ${nodeAnchors.center.x - 8} ${nodeAnchors.center.y - 7.8}, ${nodeAnchors.center.x + 8} ${nodeAnchors.center.y + 3.1}, ${nodeAnchors.center.x + 22} ${nodeAnchors.center.y - 1.9}`}
                    stroke="url(#bng-galaxy-dust-gradient)"
                    strokeWidth="2.4"
                    fill="none"
                    strokeLinecap="round"
                />
                <path
                    d={`M ${nodeAnchors.center.x - 16} ${nodeAnchors.center.y + 4.8} C ${nodeAnchors.center.x - 4} ${nodeAnchors.center.y + 1.4}, ${nodeAnchors.center.x + 10} ${nodeAnchors.center.y + 8.5}, ${nodeAnchors.center.x + 20} ${nodeAnchors.center.y + 4.1}`}
                    stroke="url(#bng-galaxy-dust-gradient)"
                    strokeWidth="1.9"
                    fill="none"
                    strokeLinecap="round"
                />
                <path
                    d={`M ${nodeAnchors.center.x - 12} ${nodeAnchors.center.y - 8.6} C ${nodeAnchors.center.x - 2} ${nodeAnchors.center.y - 11.6}, ${nodeAnchors.center.x + 5} ${nodeAnchors.center.y - 4.6}, ${nodeAnchors.center.x + 15} ${nodeAnchors.center.y - 7.9}`}
                    stroke="url(#bng-galaxy-dust-gradient)"
                    strokeWidth="1.4"
                    fill="none"
                    strokeLinecap="round"
                />
            </g>

            <ellipse
                cx={nodeAnchors.center.x}
                cy={nodeAnchors.center.y + 2.4}
                rx={(hypersphereOrbitLayers[Math.max(0, hypersphereOrbitLayers.length - 2)]?.radius ?? 14) * 0.9}
                ry={(hypersphereOrbitLayers[Math.max(0, hypersphereOrbitLayers.length - 2)]?.radius ?? 14) * 0.24}
                fill="rgba(2, 6, 23, 0.32)"
            />

            {peripheralBranchMesh.map((branch) => {
                const fromAnchor = nodeAnchors[branch.from];
                const toAnchor = nodeAnchors[branch.to];
                const midX = (fromAnchor.x + toAnchor.x) / 2;
                const midY = (fromAnchor.y + toAnchor.y) / 2;
                const controlX = midX + (nodeAnchors.center.y - midY) * 0.2;
                const controlY = midY - (nodeAnchors.center.x - midX) * 0.2 + branch.curvature;
                const path = `M${fromAnchor.x},${fromAnchor.y} Q${controlX},${controlY} ${toAnchor.x},${toAnchor.y}`;
                const reversePath = `M${toAnchor.x},${toAnchor.y} Q${controlX},${controlY} ${fromAnchor.x},${fromAnchor.y}`;
                const strokeOpacity = Math.max(0.1, branch.branchStrength / 300);
                const pulseOpacity = Math.max(0.18, branch.branchStrength / 220);
                const dur = Math.max(1.7, 3.8 - branch.branchStrength / 75);

                return (
                    <g key={branch.key}>
                        <path
                            d={path}
                            fill="none"
                            stroke={accent}
                            strokeOpacity={strokeOpacity}
                            strokeWidth={Math.max(0.1, branch.branchStrength / 620)}
                            strokeDasharray="0.9 2"
                        />
                        <circle r="0.52" fill={accent} opacity={pulseOpacity}>
                            <animateMotion
                                begin={`${branch.phase}s`}
                                dur={`${dur}s`}
                                repeatCount="indefinite"
                                path={path}
                            />
                        </circle>
                        <circle r="0.48" fill={rewardBand.line} opacity={Math.max(0.16, pulseOpacity * 0.82)}>
                            <animateMotion
                                begin={`${branch.phase + 0.52}s`}
                                dur={`${Math.max(1.5, dur + 0.22)}s`}
                                repeatCount="indefinite"
                                path={reversePath}
                            />
                        </circle>
                    </g>
                );
            })}

            <line x1={nodeAnchors.center.x} y1={nodeAnchors.center.y} x2={nodeAnchors.intel.x} y2={nodeAnchors.intel.y}
                stroke={activeDock === 'intel' ? accent : rewardBand.soft}
                strokeWidth={activeDock === 'intel' ? '0.7' : `${Math.max(0.3, (linkConfidence.intel / 180) * (0.7 + laneContractCoherence.intel / 160))}`} />
            <circle r="1.1" fill={accent} opacity={Math.max(0.35, linkConfidence.intel / 120)}>
                <animateMotion
                    begin={`${lanePulsePhase.intel.forward}s`}
                    dur={`${Math.max(1, 2.4 - linkConfidence.intel / 80)}s`}
                    repeatCount="indefinite"
                    path={`M${nodeAnchors.center.x},${nodeAnchors.center.y} L${nodeAnchors.intel.x},${nodeAnchors.intel.y}`}
                />
            </circle>
            <circle r="0.9" fill={accent} opacity={Math.max(0.28, linkConfidence.intel / 150)}>
                <animateMotion
                    begin={`${lanePulsePhase.intel.reverse}s`}
                    dur={`${Math.max(1.1, 2.8 - linkConfidence.intel / 85)}s`}
                    repeatCount="indefinite"
                    path={`M${nodeAnchors.intel.x},${nodeAnchors.intel.y} L${nodeAnchors.center.x},${nodeAnchors.center.y}`}
                />
            </circle>

            <line x1={nodeAnchors.center.x} y1={nodeAnchors.center.y} x2={nodeAnchors.objectives.x} y2={nodeAnchors.objectives.y}
                stroke={activeDock === 'objectives' ? '#38bdf8' : rewardBand.soft}
                strokeWidth={activeDock === 'objectives' ? '0.7' : `${Math.max(0.3, (linkConfidence.objectives / 180) * (0.7 + laneContractCoherence.objectives / 160))}`} />
            <circle r="1.1" fill="#38bdf8" opacity={Math.max(0.35, linkConfidence.objectives / 120)}>
                <animateMotion
                    begin={`${lanePulsePhase.objectives.forward}s`}
                    dur={`${Math.max(1, 2.5 - linkConfidence.objectives / 80)}s`}
                    repeatCount="indefinite"
                    path={`M${nodeAnchors.center.x},${nodeAnchors.center.y} L${nodeAnchors.objectives.x},${nodeAnchors.objectives.y}`}
                />
            </circle>
            <circle r="0.9" fill="#38bdf8" opacity={Math.max(0.28, linkConfidence.objectives / 150)}>
                <animateMotion
                    begin={`${lanePulsePhase.objectives.reverse}s`}
                    dur={`${Math.max(1.1, 2.9 - linkConfidence.objectives / 85)}s`}
                    repeatCount="indefinite"
                    path={`M${nodeAnchors.objectives.x},${nodeAnchors.objectives.y} L${nodeAnchors.center.x},${nodeAnchors.center.y}`}
                />
            </circle>

            <line x1={nodeAnchors.center.x} y1={nodeAnchors.center.y} x2={nodeAnchors.player.x} y2={nodeAnchors.player.y}
                stroke={activeDock === 'player' ? '#a78bfa' : rewardBand.soft}
                strokeWidth={activeDock === 'player' ? '0.7' : `${Math.max(0.3, (linkConfidence.player / 180) * (0.7 + laneContractCoherence.player / 160))}`} />
            <circle r="1.1" fill="#a78bfa" opacity={Math.max(0.35, linkConfidence.player / 120)}>
                <animateMotion
                    begin={`${lanePulsePhase.player.forward}s`}
                    dur={`${Math.max(1, 2.4 - linkConfidence.player / 80)}s`}
                    repeatCount="indefinite"
                    path={`M${nodeAnchors.center.x},${nodeAnchors.center.y} L${nodeAnchors.player.x},${nodeAnchors.player.y}`}
                />
            </circle>
            <circle r="0.9" fill="#a78bfa" opacity={Math.max(0.28, linkConfidence.player / 150)}>
                <animateMotion
                    begin={`${lanePulsePhase.player.reverse}s`}
                    dur={`${Math.max(1.1, 2.8 - linkConfidence.player / 85)}s`}
                    repeatCount="indefinite"
                    path={`M${nodeAnchors.player.x},${nodeAnchors.player.y} L${nodeAnchors.center.x},${nodeAnchors.center.y}`}
                />
            </circle>

            <line x1={nodeAnchors.center.x} y1={nodeAnchors.center.y} x2={nodeAnchors.ops.x} y2={nodeAnchors.ops.y}
                stroke={rewardBand.soft} strokeWidth={Math.max(0.35, (linkConfidence.ops / 180) * (0.7 + laneContractCoherence.ops / 160))} />
            <circle r="1.1" fill={rewardBand.line} opacity={Math.max(0.35, linkConfidence.ops / 120)}>
                <animateMotion
                    begin={`${lanePulsePhase.ops.forward}s`}
                    dur={`${Math.max(1, 2.6 - linkConfidence.ops / 80)}s`}
                    repeatCount="indefinite"
                    path={`M${nodeAnchors.center.x},${nodeAnchors.center.y} L${nodeAnchors.ops.x},${nodeAnchors.ops.y}`}
                />
            </circle>
            <circle r="0.9" fill={rewardBand.line} opacity={Math.max(0.28, linkConfidence.ops / 150)}>
                <animateMotion
                    begin={`${lanePulsePhase.ops.reverse}s`}
                    dur={`${Math.max(1.1, 3 - linkConfidence.ops / 85)}s`}
                    repeatCount="indefinite"
                    path={`M${nodeAnchors.ops.x},${nodeAnchors.ops.y} L${nodeAnchors.center.x},${nodeAnchors.center.y}`}
                />
            </circle>

            <circle cx="50" cy="48" r="5" fill="none"
                stroke={rewardBand.line} strokeWidth="0.4" opacity="0.44"
                style={{ animation: 'bng-glow 2.2s ease-in-out infinite' }} />
            <circle cx="50" cy="48" r="8.5" fill="none"
                stroke={rewardBand.line} strokeWidth="0.25" opacity="0.2" />

            {[0, 1, 2, 3, 4, 5, 6, 7].map((i) => (
                <rect key={i} x={47 + i * 1.2} y={10} width="0.7"
                    height={Math.max(1, (hotspotScore / 100) * 8 * (0.4 + 0.6 * Math.sin(i * 0.9 + hotspotScore * 0.05)))}
                    fill={accent} opacity="0.5" />
            ))}
        </svg>
    );
}

type NodeOrbClusterProps = {
    readonly nodeAnchors: NodeOrbAnchors;
    readonly activeDock: DockLoopId | null;
    readonly accent: string;
    readonly aura: string;
    readonly callDensity: number;
    readonly questPercent: number;
    readonly effectivePlayerLevel: number;
    readonly effectiveTotalXp: number;
    readonly functionCount: number;
    readonly onIntelToggle: () => void;
    readonly onObjectivesToggle: () => void;
    readonly onPlayerToggle: () => void;
    readonly onActionNode: () => void;
};

export function NodeOrbCluster(props: NodeOrbClusterProps) {
    const {
        nodeAnchors,
        activeDock,
        accent,
        aura,
        callDensity,
        questPercent,
        effectivePlayerLevel,
        effectiveTotalXp,
        functionCount,
        onIntelToggle,
        onObjectivesToggle,
        onPlayerToggle,
        onActionNode,
    } = props;

    return (
        <>
            <button
                type="button"
                aria-label="Open Intel dock"
                onClick={onIntelToggle}
                style={{
                    position: 'absolute', left: `${nodeAnchors.intel.x}%`, top: `${nodeAnchors.intel.y}%`, transform: 'translate(-50%,-50%)',
                    background: 'transparent', border: 'none', cursor: 'pointer', textAlign: 'center', zIndex: 3,
                }}
            >
                <div style={{
                    width: 52, height: 52, borderRadius: '50%', margin: '0 auto',
                    border: `1.5px solid ${activeDock === 'intel' ? accent : 'rgba(148,163,184,0.35)'}`,
                    background: activeDock === 'intel'
                        ? `radial-gradient(circle, ${aura}, rgba(2,6,23,0.9) 70%)`
                        : 'rgba(2,8,20,0.72)',
                    boxShadow: activeDock === 'intel' ? `0 0 18px ${aura}` : 'none',
                    display: 'flex', flexDirection: 'column', alignItems: 'center', justifyContent: 'center',
                    transition: 'border-color 0.2s, box-shadow 0.2s',
                }}>
                    <div style={{ fontSize: 16, fontWeight: 800, color: activeDock === 'intel' ? accent : '#cbd5e1', lineHeight: 1 }}>{callDensity}%</div>
                    <div style={{ fontSize: 8, textTransform: 'uppercase', color: '#64748b', marginTop: 2 }}>Signal</div>
                </div>
                <div style={{ marginTop: 4, fontSize: 9, fontWeight: 700, color: activeDock === 'intel' ? accent : '#94a3b8', textTransform: 'uppercase', letterSpacing: '0.06em' }}>Intel</div>
            </button>

            <button
                type="button"
                aria-label="Open Objectives dock"
                onClick={onObjectivesToggle}
                style={{
                    position: 'absolute', left: `${nodeAnchors.objectives.x}%`, top: `${nodeAnchors.objectives.y}%`, transform: 'translate(-50%,-50%)',
                    background: 'transparent', border: 'none', cursor: 'pointer', textAlign: 'center', zIndex: 3,
                }}
            >
                <div style={{
                    width: 58, height: 58, borderRadius: '50%', margin: '0 auto',
                    border: `1.5px solid ${activeDock === 'objectives' ? '#38bdf8' : 'rgba(148,163,184,0.35)'}`,
                    background: activeDock === 'objectives'
                        ? 'radial-gradient(circle, rgba(8,47,73,0.6), rgba(2,6,23,0.9) 70%)'
                        : 'rgba(2,8,20,0.72)',
                    boxShadow: activeDock === 'objectives' ? '0 0 18px rgba(56,189,248,0.22)' : 'none',
                    animation: activeDock === 'objectives' ? 'bng-pulse-ring 3.2s ease-in-out infinite' : 'none',
                    display: 'flex', flexDirection: 'column', alignItems: 'center', justifyContent: 'center',
                    transition: 'border-color 0.2s, box-shadow 0.2s',
                }}>
                    <div style={{ fontSize: 16, fontWeight: 800, color: activeDock === 'objectives' ? '#38bdf8' : '#cbd5e1', lineHeight: 1 }}>{questPercent}%</div>
                    <div style={{ fontSize: 8, textTransform: 'uppercase', color: '#64748b', marginTop: 2 }}>Done</div>
                </div>
                <div style={{ marginTop: 4, fontSize: 9, fontWeight: 700, color: activeDock === 'objectives' ? '#38bdf8' : '#94a3b8', textTransform: 'uppercase', letterSpacing: '0.06em' }}>Quests</div>
            </button>

            <button
                type="button"
                aria-label="Open Player dock"
                onClick={onPlayerToggle}
                style={{
                    position: 'absolute', left: `${nodeAnchors.player.x}%`, top: `${nodeAnchors.player.y}%`, transform: 'translate(-50%,-50%)',
                    background: 'transparent', border: 'none', cursor: 'pointer', textAlign: 'center', zIndex: 3,
                }}
            >
                <div style={{
                    width: 52, height: 52, borderRadius: '50%', margin: '0 auto',
                    border: `1.5px solid ${activeDock === 'player' ? '#a78bfa' : 'rgba(148,163,184,0.35)'}`,
                    background: activeDock === 'player'
                        ? 'radial-gradient(circle, rgba(76,29,149,0.4), rgba(2,6,23,0.9) 70%)'
                        : 'rgba(2,8,20,0.72)',
                    boxShadow: activeDock === 'player' ? '0 0 18px rgba(167,139,250,0.22)' : 'none',
                    display: 'flex', flexDirection: 'column', alignItems: 'center', justifyContent: 'center',
                    transition: 'border-color 0.2s, box-shadow 0.2s',
                }}>
                    <div style={{ fontSize: 16, fontWeight: 800, color: activeDock === 'player' ? '#a78bfa' : '#cbd5e1', lineHeight: 1 }}>Lv{effectivePlayerLevel}</div>
                    <div style={{ fontSize: 8, textTransform: 'uppercase', color: '#64748b', marginTop: 2 }}>{effectiveTotalXp} xp</div>
                </div>
                <div style={{ marginTop: 4, fontSize: 9, fontWeight: 700, color: activeDock === 'player' ? '#a78bfa' : '#94a3b8', textTransform: 'uppercase', letterSpacing: '0.06em' }}>Player</div>
            </button>

            <button
                type="button"
                aria-label="Activate main action orb"
                onClick={onActionNode}
                style={{
                    position: 'absolute', left: `${nodeAnchors.ops.x}%`, top: `${nodeAnchors.ops.y}%`, transform: 'translate(-50%,-50%)',
                    textAlign: 'center', zIndex: 3,
                    background: 'transparent',
                    border: 'none',
                    cursor: 'pointer',
                }}>
                <div style={{
                    width: 58, height: 58, borderRadius: '50%', margin: '0 auto',
                    border: '1.5px solid rgba(245, 158, 11, 0.5)',
                    background: 'radial-gradient(circle, rgba(120,53,15,0.45), rgba(2,8,20,0.84) 72%)',
                    boxShadow: '0 0 20px rgba(245,158,11,0.24)',
                    animation: 'bng-pulse-ring 3s ease-in-out infinite',
                    display: 'flex', flexDirection: 'column', alignItems: 'center', justifyContent: 'center',
                }}>
                    <div style={{ fontSize: 14, fontWeight: 800, color: '#fbbf24', lineHeight: 1 }}>{functionCount}</div>
                    <div style={{ fontSize: 8, textTransform: 'uppercase', color: '#64748b', marginTop: 2 }}>Fns</div>
                </div>
                <div style={{ marginTop: 4, fontSize: 9, fontWeight: 700, color: '#fbbf24', textTransform: 'uppercase', letterSpacing: '0.06em' }}>Action</div>
            </button>
        </>
    );
}

export type NodeOpsActionSpec = {
    readonly id: NodeInteractionAction;
    readonly label: string;
    readonly xp: number;
    readonly recommended: boolean;
};

type NodeOpsConsoleProps = {
    readonly viewportBottomInset: number;
    readonly hasOpenDock: boolean;
    readonly isCompactViewport: boolean;
    readonly focusedNodeTitle: string;
    readonly modelConfidence: number;
    readonly trainingAccuracy: number;
    readonly policyMomentum: number;
    readonly actions: readonly NodeOpsActionSpec[];
    readonly onAction: (action: NodeInteractionAction) => void;
    readonly snapshots: number;
    readonly inspects: number;
    readonly trainings: number;
    readonly routes: number;
    readonly sessionXpGain: number;
};

export function NodeOpsConsole(props: NodeOpsConsoleProps) {
    const {
        viewportBottomInset,
        hasOpenDock,
        isCompactViewport,
        focusedNodeTitle,
        modelConfidence,
        trainingAccuracy,
        policyMomentum,
        actions,
        onAction,
        snapshots,
        inspects,
        trainings,
        routes,
        sessionXpGain,
    } = props;

    return (
        <div style={{
            position: 'absolute',
            left: '50%',
            bottom: viewportBottomInset + (hasOpenDock ? 6 : 12),
            transform: 'translateX(-50%)',
            width: isCompactViewport ? 'calc(100% - 24px)' : 'min(620px, calc(100% - 24px))',
            borderRadius: 12,
            border: '1px solid rgba(148, 163, 184, 0.14)',
            background: 'linear-gradient(180deg, rgba(5, 12, 24, 0.64), rgba(3, 9, 19, 0.62))',
            boxShadow: '0 8px 16px rgba(2, 6, 23, 0.22)',
            backdropFilter: 'blur(2px)',
            padding: '8px 9px',
            display: 'grid',
            gap: 7,
            zIndex: 4,
        }}>
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', gap: 8, flexWrap: 'wrap' }}>
                <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#a5b4fc', fontWeight: 700 }}>
                    Node Ops Console · {focusedNodeTitle}
                </div>
                <div style={{ fontSize: 9, color: '#94a3b8', textTransform: 'uppercase', letterSpacing: '0.06em' }}>
                    model {modelConfidence}% · accuracy {trainingAccuracy}% · momentum {policyMomentum}%
                </div>
            </div>
            <div style={{
                display: 'grid',
                gridTemplateColumns: isCompactViewport ? 'repeat(2, minmax(0, 1fr))' : 'repeat(4, minmax(0, 1fr))',
                gap: 6,
            }}>
                {actions.map((action) => (
                    <button
                        key={action.id}
                        type="button"
                        onClick={() => onAction(action.id)}
                        style={{
                            borderRadius: 9,
                            border: action.recommended ? '1px solid rgba(110, 231, 183, 0.62)' : '1px solid rgba(148, 163, 184, 0.32)',
                            background: action.recommended ? 'rgba(6, 78, 59, 0.66)' : 'rgba(2, 10, 22, 0.66)',
                            color: action.recommended ? '#d1fae5' : '#cbd5e1',
                            padding: '6px 8px',
                            textAlign: 'left',
                            cursor: 'pointer',
                            display: 'grid',
                            gap: 3,
                        }}
                    >
                        <span style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.07em', fontWeight: 700 }}>
                            {action.label}
                        </span>
                        <span style={{ fontSize: 9, color: action.recommended ? '#a7f3d0' : '#94a3b8' }}>
                            +{action.xp} XP
                            {action.recommended ? ' · Recommended' : ''}
                        </span>
                    </button>
                ))}
            </div>
            <div style={{ display: 'flex', gap: 8, flexWrap: 'wrap', fontSize: 9, color: '#94a3b8', textTransform: 'uppercase', letterSpacing: '0.06em' }}>
                <span>Snapshots {snapshots}</span>
                <span>Inspects {inspects}</span>
                <span>Training {trainings}</span>
                <span>Routes {routes}</span>
                <span>Session XP +{sessionXpGain}</span>
            </div>
        </div>
    );
}
