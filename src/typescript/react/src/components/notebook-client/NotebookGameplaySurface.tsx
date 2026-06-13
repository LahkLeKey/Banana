import { useEffect, useMemo, useState, type CSSProperties, type Dispatch, type SetStateAction } from 'react';
import { RoutePill as Pill, RouteSurfaceCard as SurfaceCard } from '@banana/ui';

import type { QuestProgress } from '../../lib/notebook-client/gamification';
import { buildIntelSignals, buildMissionIntelEvents } from '../../domain/notebook/intel-domain';
import { deriveMissionMeta } from '../../domain/notebook/mission-domain';
import {
    buildCFileContractNodeModel,
    type ContractNodeId,
    type NodeInteractionAction,
} from '../../domain/notebook/network-domain';
import { useNetcodeSession } from '../../domain/notebook/useNetcodeSession';
import type { NotebookRouteMode } from '../../lib/notebook-client/routeModeConfig';
import { buildWorkflowStepMap, type WorkflowStepId } from '../../domain/notebook/workflow-domain';
import { useNotebookLayoutStore } from '../../lib/notebook-client/layoutStore';
import { IntelDockPanel, ObjectivesDockPanel, PlayerDockPanel } from './NotebookDockPanels';
import {
    ActiveDockShell,
    GameplayCoreNode,
    GameplayGalaxyField,
    GameplayTopHud,
    NodeOpsConsole,
    NodeOrbCluster,
    QuestLogRail,
    TelemetryRail,
    type OrbAnchorSet,
    type OrbitLayerSpec,
    type NodeOpsActionSpec,
    type QuestLoopItem,
} from './NotebookGameplaySharedPanels';
type NotebookGameplaySurfaceProps = {
    readonly selectedFile: string;
    readonly selectedContent: string;
    readonly fullBleed?: boolean;
    readonly routeMode?: NotebookRouteMode;
    readonly allIndexedFiles?: readonly string[];
    readonly contentByFile?: ReadonlyMap<string, string>;
    readonly indexedFileCount?: number;
    readonly notebookCellCount?: number;
    readonly selectedDomain?: string;
    readonly rarityLabel?: string;
    readonly rarityAccent?: string;
    readonly playerLevel?: number;
    readonly totalXp?: number;
    readonly comboStreak?: number;
    readonly lastXpGain?: number;
    readonly questProgress?: QuestProgress[];
    readonly questToast?: string;
    readonly nextDomainTarget?: string;
    readonly taskbarOwnsIntelAndPlayerWindows?: boolean;
    readonly taskbarOwnsObjectiveAndQuestWindows?: boolean;
    readonly taskbarOwnsNodeOpsWindow?: boolean;
    readonly onToggleIntelNodeWindow?: () => void;
    readonly onToggleObjectiveNodeWindow?: () => void;
    readonly onTogglePlayerNodeWindow?: () => void;
    readonly onToggleNodeOpsWindow?: () => void;
    readonly onAnalyticsTelemetryChange?: (telemetry: {
        analyticsAvailable: boolean;
        analyticsCohort: string;
        k3h4Clusters: number;
        k3h4Convergence: string;
        abiCoveragePresent: number;
        abiCoverageExpected: number;
        abiCoveragePercent: number;
        abiCoverageComplete: boolean;
        abiCoverageMissing: readonly string[];
        abiLayerLedger: readonly {
            layer: 'learning' | 'reward' | 'link' | 'vector' | 'k3h4';
            present: boolean;
            contractVersion: number;
            status: 'ok' | 'unsupported-version' | 'invalid-payload' | 'nonfinite-value' | 'crc-mismatch' | 'missing';
            payloadBytes: number;
            byteOrderTag: number;
            deterministicHash: number | string | null;
        }[];
    }) => void;
};

// Scanline overlay for the fullBleed stage
const scanlineUri = `url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='2' height='4'%3E%3Crect width='2' height='2' fill='rgba(0,0,0,0.08)'/%3E%3C/svg%3E")`;

const domainTheme: Record<string, { aura: string; accent: string; panel: string; radar: string; }> = {
    ai: {
        aura: 'rgba(16, 185, 129, 0.24)',
        accent: '#6ee7b7',
        panel: 'rgba(6, 33, 25, 0.86)',
        radar: '#34d399',
    },
    physics: {
        aura: 'rgba(59, 130, 246, 0.24)',
        accent: '#93c5fd',
        panel: 'rgba(8, 22, 42, 0.86)',
        radar: '#60a5fa',
    },
    render: {
        aura: 'rgba(168, 85, 247, 0.24)',
        accent: '#d8b4fe',
        panel: 'rgba(28, 18, 45, 0.86)',
        radar: '#c084fc',
    },
    world: {
        aura: 'rgba(245, 158, 11, 0.22)',
        accent: '#fcd34d',
        panel: 'rgba(42, 24, 9, 0.86)',
        radar: '#f59e0b',
    },
    runtime: {
        aura: 'rgba(20, 184, 166, 0.22)',
        accent: '#5eead4',
        panel: 'rgba(6, 29, 33, 0.86)',
        radar: '#2dd4bf',
    },
    other: {
        aura: 'rgba(14, 165, 233, 0.18)',
        accent: '#67e8f9',
        panel: 'rgba(5, 22, 34, 0.86)',
        radar: '#22d3ee',
    },
};

const workflowOrder: WorkflowStepId[] = ['scan', 'shape', 'commit'];

const routeModeOrbitProfile: Record<NotebookRouteMode, { radiusScale: number; squashBias: number; rotationBias: number; opacityScale: number; }> = {
    runtime: { radiusScale: 1, squashBias: 0, rotationBias: 0, opacityScale: 1 },
    auth: { radiusScale: 0.82, squashBias: -0.06, rotationBias: -6, opacityScale: 0.9 },
    marketing: { radiusScale: 1.14, squashBias: 0.05, rotationBias: 10, opacityScale: 0.94 },
    portal: { radiusScale: 0.92, squashBias: -0.02, rotationBias: -4, opacityScale: 0.92 },
    session: { radiusScale: 1.06, squashBias: 0.04, rotationBias: 7, opacityScale: 1.02 },
    engine: { radiusScale: 1.12, squashBias: 0.02, rotationBias: 12, opacityScale: 1.04 },
    menu: { radiusScale: 0.88, squashBias: -0.03, rotationBias: -8, opacityScale: 0.9 },
    character: { radiusScale: 0.96, squashBias: 0.03, rotationBias: 5, opacityScale: 0.96 },
};

const routeModeOrbitAccent: Record<NotebookRouteMode, string> = {
    runtime: '#67e8f9',
    auth: '#93c5fd',
    marketing: '#f9a8d4',
    portal: '#a5b4fc',
    session: '#86efac',
    engine: '#fcd34d',
    menu: '#c4b5fd',
    character: '#fca5a5',
};

const nodeActionLabels: Record<NodeInteractionAction, string> = {
    snapshot: 'Snapshot',
    inspect: 'Inspect',
    train: 'Train',
    route: 'Route',
};

const nodeTitles: Record<ContractNodeId, string> = {
    intel: 'Intel Node',
    objectives: 'Objective Node',
    player: 'Player Node',
    ops: 'Action Node',
};

export function NotebookGameplaySurface({
    selectedFile,
    selectedContent,
    fullBleed = false,
    routeMode = 'runtime',
    allIndexedFiles = [],
    contentByFile,
    indexedFileCount = 0,
    notebookCellCount = 0,
    selectedDomain = 'other',
    rarityLabel = 'Common',
    rarityAccent = '#86efac',
    playerLevel = 1,
    totalXp = 0,
    comboStreak = 0,
    lastXpGain = 0,
    questProgress = [],
    questToast = '',
    nextDomainTarget = 'runtime',
    taskbarOwnsIntelAndPlayerWindows = false,
    taskbarOwnsObjectiveAndQuestWindows = false,
    taskbarOwnsNodeOpsWindow = false,
    onToggleIntelNodeWindow,
    onToggleObjectiveNodeWindow,
    onTogglePlayerNodeWindow,
    onToggleNodeOpsWindow,
    onAnalyticsTelemetryChange,
}: NotebookGameplaySurfaceProps) {
    const [intelMode, setIntelMode] = useState<'summary' | 'raw'>('summary');
    const [developerIntelUnlocked, setDeveloperIntelUnlocked] = useState(false);
    const [isCompactViewport, setIsCompactViewport] = useState(false);
    const [expandedQuestRail, setExpandedQuestRail] = useState(false);
    const [expandedDockPanel, setExpandedDockPanel] = useState(false);
    const [focusedNode, setFocusedNode] = useState<ContractNodeId>('intel');
    const [sessionXpGain, setSessionXpGain] = useState(0);
    const [interactionToast, setInteractionToast] = useState('');
    const activeDock = useNotebookLayoutStore((state) => state.activeDock);
    const setActiveDock = useNotebookLayoutStore((state) => state.setActiveDock);
    const intelPanel = useNotebookLayoutStore((state) => state.intelPanel);
    const setIntelPanel = useNotebookLayoutStore((state) => state.setIntelPanel);
    const objectivePanel = useNotebookLayoutStore((state) => state.objectivePanel);
    const setObjectivePanel = useNotebookLayoutStore((state) => state.setObjectivePanel);
    const playerPanel = useNotebookLayoutStore((state) => state.playerPanel);
    const setPlayerPanel = useNotebookLayoutStore((state) => state.setPlayerPanel);
    const showFullSourceDump = useNotebookLayoutStore((state) => state.showFullSourceDump);
    const setShowFullSourceDump = useNotebookLayoutStore((state) => state.setShowFullSourceDump);
    const activeWorkflowStep = useNotebookLayoutStore((state) => state.activeWorkflowStep);
    const setActiveWorkflowStep = useNotebookLayoutStore((state) => state.setActiveWorkflowStep);
    const workflowDepth = useNotebookLayoutStore((state) => state.workflowDepth);
    const setWorkflowDepth = useNotebookLayoutStore((state) => state.setWorkflowDepth);
    const cycleWorkflowStep = useNotebookLayoutStore((state) => state.cycleWorkflowStep);
    const adjustWorkflowDepth = useNotebookLayoutStore((state) => state.adjustWorkflowDepth);
    const resetForSector = useNotebookLayoutStore((state) => state.resetForSector);
    const showExplorer = useNotebookLayoutStore((state) => state.showExplorer);
    const showMenu = useNotebookLayoutStore((state) => state.showMenu);
    const showStatus = useNotebookLayoutStore((state) => state.showStatus);
    const showOperations = useNotebookLayoutStore((state) => state.showOperations);
    const showObjectiveNodeWindow = useNotebookLayoutStore((state) => state.showObjectiveNodeWindow);
    const showQuestLogWindow = useNotebookLayoutStore((state) => state.showQuestLogWindow);
    const setGameplayWindowVisibility = useNotebookLayoutStore((state) => state.setGameplayWindowVisibility);
    const intelSignals = useMemo(
        () => buildIntelSignals(selectedContent),
        [selectedContent],
    );
    const {
        lineCount,
        functionCount,
        functionNames,
        includeCount,
        primaryDependencyLabel,
        callDensity,
        branchPressure,
        dependencyPulse,
        hotspotScore,
        riskLabel,
        sourcePreviewWithLineNumbers,
    } = intelSignals;
    const activeTheme = useMemo(
        () => domainTheme[selectedDomain] ?? domainTheme.other,
        [selectedDomain],
    );
    const completedQuestCount = useMemo(
        () => questProgress.filter((quest) => quest.completed).length,
        [questProgress],
    );
    const effectiveTotalXp = totalXp + sessionXpGain;
    const effectivePlayerLevel = playerLevel + Math.floor(effectiveTotalXp / 260) - Math.floor(totalXp / 260);
    const questPercent = useMemo(() => {
        if (questProgress.length === 0) {
            return 0;
        }
        return Math.round((completedQuestCount / questProgress.length) * 100);
    }, [completedQuestCount, questProgress.length]);
    const missionSector = useMemo(
        () => deriveMissionMeta(selectedFile),
        [selectedFile],
    );
    const intelEvents = useMemo(
        () => buildMissionIntelEvents(intelSignals),
        [intelSignals],
    );
    const workflowSteps = useMemo(() => buildWorkflowStepMap({
        missionTitle: missionSector.title,
        hotspotScore,
        riskLabel,
        callDensity,
        primaryDependencyLabel,
        branchPressure,
        dependencyPulse,
        nextDomainTarget,
        completedQuestCount,
        totalQuestCount: questProgress.length,
        comboStreak,
        lastXpGain,
    }), [
        missionSector.title,
        hotspotScore,
        riskLabel,
        callDensity,
        primaryDependencyLabel,
        branchPressure,
        dependencyPulse,
        nextDomainTarget,
        completedQuestCount,
        questProgress.length,
        comboStreak,
        lastXpGain,
    ]);
    const cFileCount = useMemo(
        () => allIndexedFiles.filter((filePath) => /\.c$/i.test(filePath)).length,
        [allIndexedFiles],
    );
    const networkDimensions = useMemo(() => {
        if (cFileCount <= 0) {
            return 6;
        }

        // Expand K3H4 projection dimensionality as C-file evidence increases.
        return Math.min(16, Math.max(8, 6 + Math.floor(Math.log2(cFileCount + 1))));
    }, [cFileCount]);
    const {
        learningModel,
        ledger: interactionLedger,
        recordNodeTap,
        recordAction,
        rewardSignal,
        linkConfidence,
        k3h4Projection,
        k3h4,
        abiLayerLedger,
        abiLayerCoverage,
        analyticsAvailability,
    } = useNetcodeSession({
        callDensity,
        questPercent,
        comboStreak,
        branchPressure,
        workflowDepth,
        playerLevel: effectivePlayerLevel,
        dependencyPulse,
        networkDimensions,
    });
    const subsystemLoops = useMemo(() => ([
        {
            id: 'intel' as const,
            label: 'Intel',
            value: callDensity,
            accent: activeTheme.accent,
            detail: `${riskLabel} risk`,
        },
        {
            id: 'objectives' as const,
            label: 'Quests',
            value: questPercent,
            accent: '#38bdf8',
            detail: `${completedQuestCount}/${questProgress.length} complete`,
        },
        {
            id: 'player' as const,
            label: 'Player',
            value: Math.min(100, Math.round(effectivePlayerLevel * 8 + comboStreak * 4)),
            accent: '#a78bfa',
            detail: `Lv${effectivePlayerLevel} · x${Math.max(1, comboStreak)}`,
        },
        {
            id: null,
            label: 'Ops',
            value: Math.min(100, Math.round((branchPressure + dependencyPulse) / 2)),
            accent: '#f59e0b',
            detail: `${branchPressure}% pressure`,
        },
    ]), [
        activeTheme.accent,
        branchPressure,
        callDensity,
        comboStreak,
        completedQuestCount,
        dependencyPulse,
        effectivePlayerLevel,
        questPercent,
        questProgress.length,
        riskLabel,
    ]);
    const { neuralRelevanceScore, projectedRewardXp, rewardTier, rewardBand } = rewardSignal;
    const k3h4ClusterCount = k3h4.centers.length;
    const k3h4Convergence = k3h4.observability.convergenceStatus;
    const k3h4HashPreview = k3h4.observability.deterministicHash.length > 0
        ? k3h4.observability.deterministicHash.slice(0, 8)
        : 'n/a';
    const abiCoveragePresent = abiLayerCoverage.presentLayers.length;
    const abiCoverageExpected = abiLayerCoverage.expectedLayers.length;
    const abiCoveragePercent = Math.round(abiLayerCoverage.completeness * 100);
    const abiCoverageSummary = `${abiCoveragePresent}/${abiCoverageExpected}`;
    const abiCoverageState = abiLayerCoverage.complete ? 'complete' : 'partial';
    const abiCoverageMissing = abiLayerCoverage.missingLayers.length > 0
        ? abiLayerCoverage.missingLayers.join(', ')
        : 'none';

    useEffect(() => {
        if (!onAnalyticsTelemetryChange) {
            return;
        }

        onAnalyticsTelemetryChange({
            analyticsAvailable: analyticsAvailability.available,
            analyticsCohort: analyticsAvailability.rollout.cohort,
            k3h4Clusters: k3h4ClusterCount,
            k3h4Convergence,
            abiCoveragePresent,
            abiCoverageExpected,
            abiCoveragePercent,
            abiCoverageComplete: abiLayerCoverage.complete,
            abiCoverageMissing: [...abiLayerCoverage.missingLayers],
            abiLayerLedger: abiLayerLedger.map((entry) => ({ ...entry })),
        });
    }, [
        abiLayerLedger,
        abiCoverageExpected,
        abiCoveragePercent,
        abiCoveragePresent,
        analyticsAvailability.available,
        analyticsAvailability.rollout.cohort,
        k3h4ClusterCount,
        k3h4Convergence,
        onAnalyticsTelemetryChange,
        abiLayerCoverage.complete,
        abiLayerCoverage.missingLayers,
    ]);
    const laneContractCoherence = useMemo(() => {
        const coherenceMap = new Map<ContractNodeId, number>();
        k3h4Projection.nodes.forEach((node) => {
            coherenceMap.set(node.id, node.coherence);
        });

        return {
            intel: coherenceMap.get('intel') ?? 50,
            objectives: coherenceMap.get('objectives') ?? 50,
            player: coherenceMap.get('player') ?? 50,
            ops: coherenceMap.get('ops') ?? 50,
        };
    }, [k3h4Projection.nodes]);
    const hasOpenDock = activeDock !== null;
    const orbitLayerCount = useMemo(
        () => Math.max(2, Math.ceil(k3h4Projection.dimensions / 2)),
        [k3h4Projection.dimensions],
    );
    const cFileContractNodes = useMemo(() => {
        if (!contentByFile || allIndexedFiles.length === 0) {
            return [];
        }

        return buildCFileContractNodeModel({
            files: allIndexedFiles,
            contentByFile,
            orbitLayerCount,
        });
    }, [allIndexedFiles, contentByFile, orbitLayerCount]);
    const k3h4OrbitLayers: OrbitLayerSpec[] = useMemo(() => {
        const layerCount = orbitLayerCount;
        const alignmentWeight = 0.5 + k3h4Projection.alignment / 250;
        const stabilityWeight = 0.5 + k3h4Projection.radialStability / 250;
        const orbitProfile = routeModeOrbitProfile[routeMode];
        const hudHorizontalLoad = isCompactViewport ? 0.08 : 0.36;
        const hudVerticalLoad = (hasOpenDock ? 0.18 : 0) + 0.14;
        const maxRadius = Math.max(11, 19 - hudHorizontalLoad * 24 - hudVerticalLoad * 18);
        const minRadius = Math.max(5.5, maxRadius * 0.34);
        const orbitSpread = Math.max(4.5, maxRadius - minRadius);
        const squashBase = Math.max(0.7, 0.82 - hudVerticalLoad * 0.12);
        const squashSpread = 0.14 + hudVerticalLoad * 0.06;

        return Array.from({ length: layerCount }, (_, index) => {
            const progression = (index + 1) / layerCount;
            return {
                key: `orbit-${index}`,
                radius: (minRadius + progression * orbitSpread) * orbitProfile.radiusScale,
                squash: Math.min(0.98, Math.max(0.66, squashBase + progression * squashSpread + orbitProfile.squashBias)),
                rotate: -18 + progression * 36 + orbitProfile.rotationBias,
                opacity: Math.max(0.1, 0.28 * (1 - index / (layerCount + 1)) * alignmentWeight * orbitProfile.opacityScale),
                strokeWidth: Math.max(0.12, 0.42 * (1 - index / (layerCount + 1)) * stabilityWeight),
            };
        });
    }, [
        hasOpenDock,
        k3h4Projection.alignment,
        k3h4Projection.dimensions,
        k3h4Projection.radialStability,
        isCompactViewport,
        routeMode,
        orbitLayerCount,
    ]);
    const peripheralBranchMesh = useMemo(() => {
        const linkByLane: Record<ContractNodeId, number> = {
            intel: linkConfidence.intel,
            objectives: linkConfidence.objectives,
            player: linkConfidence.player,
            ops: linkConfidence.ops,
        };
        const coherenceByLane: Record<ContractNodeId, number> = {
            intel: laneContractCoherence.intel,
            objectives: laneContractCoherence.objectives,
            player: laneContractCoherence.player,
            ops: laneContractCoherence.ops,
        };
        const pairs: readonly [ContractNodeId, ContractNodeId][] = [
            ['intel', 'objectives'],
            ['intel', 'player'],
            ['intel', 'ops'],
            ['objectives', 'player'],
            ['objectives', 'ops'],
            ['player', 'ops'],
        ];

        return pairs.map(([from, to], index) => {
            const branchStrength = Math.round((linkByLane[from] + linkByLane[to] + coherenceByLane[from] + coherenceByLane[to]) / 4);
            return {
                key: `${from}-${to}`,
                from,
                to,
                branchStrength,
                phase: 0.14 * index,
                curvature: (index % 2 === 0 ? 1 : -1) * (4 + (branchStrength / 100) * 3),
            };
        });
    }, [laneContractCoherence, linkConfidence.intel, linkConfidence.objectives, linkConfidence.ops, linkConfidence.player]);
    const visibleLoops = useMemo(() => subsystemLoops, [subsystemLoops]);
    const nodeAnchors: OrbAnchorSet = useMemo(() => {
        const center = { x: 50, y: hasOpenDock ? 47 : 50 };
        const topLeftX = 30;
        const topRightX = 70;
        const bottomLeftX = 24;
        const bottomRightX = 76;
        const topY = 34;
        const nominalBottomY = 86;
        const bottomY = Math.min(nominalBottomY, hasOpenDock ? 82 : 88);
        const topSpan = Math.max(6, center.y - topY);
        const bottomSpan = Math.max(8, bottomY - center.y);
        const leftTopSpan = Math.max(8, center.x - topLeftX);
        const rightTopSpan = Math.max(8, topRightX - center.x);
        const leftBottomSpan = Math.max(8, center.x - bottomLeftX);
        const rightBottomSpan = Math.max(8, bottomRightX - center.x);

        const projectionById = new Map<ContractNodeId, { x: number; y: number; z: number; }>();
        k3h4Projection.nodes.forEach((node) => {
            projectionById.set(node.id, { x: node.x, y: node.y, z: node.z });
        });

        const projectAnchor = (id: ContractNodeId, horizontal: 'left' | 'right', vertical: 'top' | 'bottom') => {
            const projection = projectionById.get(id);
            const px = Math.min(1, Math.abs(projection?.x ?? 0.58));
            const py = Math.min(1, Math.abs(projection?.y ?? 0.62));
            const pz = Math.min(1, Math.max(0, projection?.z ?? 0.5));
            const radialBias = 0.54 + px * 0.28 + (1 - pz) * 0.12;
            const verticalBias = 0.56 + py * 0.26 + (1 - pz) * 0.12;
            const horizontalSpan = horizontal === 'left'
                ? (vertical === 'top' ? leftTopSpan : leftBottomSpan)
                : (vertical === 'top' ? rightTopSpan : rightBottomSpan);

            return {
                x: horizontal === 'left'
                    ? center.x - horizontalSpan * radialBias
                    : center.x + horizontalSpan * radialBias,
                y: vertical === 'top'
                    ? center.y - topSpan * verticalBias
                    : center.y + bottomSpan * verticalBias,
            };
        };

        return {
            center,
            intel: projectAnchor('intel', 'left', 'top'),
            objectives: projectAnchor('objectives', 'left', 'bottom'),
            player: projectAnchor('player', 'right', 'top'),
            ops: projectAnchor('ops', 'right', 'bottom'),
        };
    }, [hasOpenDock, k3h4Projection.nodes]);
    const cFileVisualNodes = useMemo(() => {
        if (cFileContractNodes.length === 0 || k3h4OrbitLayers.length === 0) {
            return [] as Array<{
                key: string;
                x: number;
                y: number;
                radius: number;
                opacity: number;
                accent: string;
                glowRadius: number;
                glowOpacity: number;
                twinkleDur: number;
                twinkleDelay: number;
                depthBand: 'near' | 'mid' | 'far';
            }>;
        }

        const armCount = Math.max(3, Math.min(6, Math.ceil(orbitLayerCount / 2)));
        const spiralTightness = 0.78;

        return cFileContractNodes.map((node, index) => {
            const layer = k3h4OrbitLayers[node.layerHint % k3h4OrbitLayers.length];
            const armIndex = index % armCount;
            const armAngle = (Math.PI * 2 * armIndex) / armCount;
            const radialRatio = (node.layerHint + 1) / Math.max(1, orbitLayerCount);
            const radiusNudge = 0.74 + radialRatio * 0.28 + (node.signal / 100) * 0.14;
            const spiralTurn = radialRatio * Math.PI * 2.2 * spiralTightness;
            const jitter = (((index * 31) % 19) - 9) * 0.012;
            const theta = armAngle + spiralTurn + jitter;
            const radiusVector = layer.radius * radiusNudge;
            const x = nodeAnchors.center.x + Math.cos(theta) * radiusVector;
            const y = nodeAnchors.center.y + Math.sin(theta) * radiusVector * layer.squash;
            const radius = 0.12 + (node.signal / 100) * 0.28;
            const opacity = 0.2 + (node.coupling / 100) * 0.3;
            const accent = node.signal >= 70
                ? '#7dd3fc'
                : node.signal >= 45
                    ? '#a5b4fc'
                    : '#64748b';
            const depthBias = 0.52 + (node.signal / 100) * 0.48;
            const glowRadius = radius * (2.4 + depthBias);
            const glowOpacity = Math.max(0.08, opacity * 0.28 * depthBias);
            const twinkleDur = 2.1 + ((index % 7) * 0.27);
            const twinkleDelay = (index % 11) * 0.18;
            const depthBand: 'near' | 'mid' | 'far' = node.signal >= 72 ? 'near' : node.signal >= 42 ? 'mid' : 'far';

            return {
                key: `${node.filePath}-${index}`,
                x,
                y,
                radius,
                opacity,
                accent,
                glowRadius,
                glowOpacity,
                twinkleDur,
                twinkleDelay,
                depthBand,
            };
        });
    }, [cFileContractNodes, k3h4OrbitLayers, nodeAnchors.center.x, nodeAnchors.center.y, orbitLayerCount]);
    const ambientGalaxyNodes = useMemo(() => {
        if (k3h4OrbitLayers.length === 0) {
            return [] as Array<{
                key: string;
                x: number;
                y: number;
                radius: number;
                opacity: number;
                accent: string;
                glowRadius: number;
                glowOpacity: number;
                twinkleDur: number;
                twinkleDelay: number;
                depthBand: 'near' | 'mid' | 'far';
            }>;
        }

        const count = 140;
        return Array.from({ length: count }, (_, index) => {
            const layer = k3h4OrbitLayers[index % k3h4OrbitLayers.length];
            const seed = Math.sin(index * 12.9898) * 43758.5453;
            const frac = seed - Math.floor(seed);
            const theta = frac * Math.PI * 2 + index * 0.17;
            const radial = 0.62 + ((index % 29) / 29) * 0.42;
            const x = nodeAnchors.center.x + Math.cos(theta) * layer.radius * radial;
            const y = nodeAnchors.center.y + Math.sin(theta) * layer.radius * layer.squash * radial;
            const depthBand: 'near' | 'mid' | 'far' = index % 7 === 0 ? 'near' : index % 3 === 0 ? 'mid' : 'far';
            const radius = depthBand === 'near' ? 0.24 : depthBand === 'mid' ? 0.18 : 0.12;
            const opacity = depthBand === 'near' ? 0.44 : depthBand === 'mid' ? 0.3 : 0.2;
            const accent = depthBand === 'near' ? '#bfdbfe' : depthBand === 'mid' ? '#93c5fd' : '#64748b';

            return {
                key: `ambient-${index}`,
                x,
                y,
                radius,
                opacity,
                accent,
                glowRadius: radius * 2.4,
                glowOpacity: opacity * 0.26,
                twinkleDur: 1.9 + (index % 9) * 0.19,
                twinkleDelay: (index % 13) * 0.12,
                depthBand,
            };
        });
    }, [k3h4OrbitLayers, nodeAnchors.center.x, nodeAnchors.center.y]);
    const renderedGalaxyNodes = cFileVisualNodes.length > 0 ? cFileVisualNodes : ambientGalaxyNodes;
    const lanePulsePhase = useMemo(() => ({
        intel: { forward: 0, reverse: 0.66 },
        objectives: { forward: 0.18, reverse: 0.84 },
        player: { forward: 0.34, reverse: 1.0 },
        ops: { forward: 0.5, reverse: 1.16 },
    }), []);

    useEffect(() => {
        setIntelMode('summary');
        resetForSector(Boolean(selectedFile));
    }, [resetForSector, selectedFile]);

    useEffect(() => {
        if (typeof window === 'undefined') {
            return;
        }

        const stored = window.localStorage.getItem('banana.notebooks.devIntelUnlocked');
        setDeveloperIntelUnlocked(stored === '1');
    }, []);

    useEffect(() => {
        if (typeof window === 'undefined') {
            return;
        }

        const applyViewportMode = () => {
            setIsCompactViewport(window.innerWidth <= 1100);
        };

        applyViewportMode();
        window.addEventListener('resize', applyViewportMode);

        return () => {
            window.removeEventListener('resize', applyViewportMode);
        };
    }, []);

    useEffect(() => {
        if (!developerIntelUnlocked && intelMode === 'raw') {
            setIntelMode('summary');
        }
    }, [developerIntelUnlocked, intelMode]);

    useEffect(() => {
        if (intelMode !== 'raw') {
            setShowFullSourceDump(false);
        }
    }, [intelMode, setShowFullSourceDump]);

    useEffect(() => {
        if (interactionToast.length === 0) {
            return;
        }

        const timeoutId = window.setTimeout(() => {
            setInteractionToast('');
        }, 2100);

        return () => {
            window.clearTimeout(timeoutId);
        };
    }, [interactionToast]);

    const toggleDeveloperUnlock = () => {
        setDeveloperIntelUnlocked((previous) => {
            const next = !previous;
            if (typeof window !== 'undefined') {
                window.localStorage.setItem(
                    'banana.notebooks.devIntelUnlocked',
                    next ? '1' : '0',
                );
            }
            return next;
        });
    };
    const setIntelPanelDispatch: Dispatch<SetStateAction<'threat' | 'signal' | 'route'>> = (value) => {
        if (typeof value === 'function') {
            setIntelPanel(value(intelPanel));
            return;
        }
        setIntelPanel(value);
    };
    const setObjectivePanelDispatch: Dispatch<SetStateAction<'board' | 'timeline'>> = (value) => {
        if (typeof value === 'function') {
            setObjectivePanel(value(objectivePanel));
            return;
        }
        setObjectivePanel(value);
    };
    const setPlayerPanelDispatch: Dispatch<SetStateAction<'rank' | 'economy' | 'streak'>> = (value) => {
        if (typeof value === 'function') {
            setPlayerPanel(value(playerPanel));
            return;
        }
        setPlayerPanel(value);
    };
    const setShowFullSourceDumpDispatch: Dispatch<SetStateAction<boolean>> = (value) => {
        if (typeof value === 'function') {
            setShowFullSourceDump(value(showFullSourceDump));
            return;
        }
        setShowFullSourceDump(value);
    };
    const cycleNodeFocus = () => {
        const nodeOrder: ContractNodeId[] = ['intel', 'objectives', 'player', 'ops'];
        const currentIndex = nodeOrder.indexOf(focusedNode);
        const nextNode = nodeOrder[(currentIndex + 1 + nodeOrder.length) % nodeOrder.length];
        setFocusedNode(nextNode);
        recordNodeTap(nextNode);
        const tapXp = Math.max(1, Math.round(learningModel.policyMomentum / 28));
        setSessionXpGain((previous) => previous + tapXp);
        setInteractionToast(`Node lock ${nodeTitles[nextNode]} +${tapXp} XP`);
    };
    const focusNode = (node: ContractNodeId) => {
        setFocusedNode(node);
        recordNodeTap(node);
    };
    const applyNodeAction = (action: NodeInteractionAction) => {
        const weightedBonus = Math.max(1, Math.round((learningModel.nodeWeights[focusedNode] ?? 0) / 26));
        const actionXp = learningModel.xpByAction[action] + weightedBonus;

        recordAction(action);
        setSessionXpGain((previous) => previous + actionXp);

        if (action === 'inspect') {
            setActiveDock(focusedNode === 'ops' ? 'player' : focusedNode);
        }
        if (action === 'train') {
            setActiveWorkflowStep('shape');
            setWorkflowDepth(3);
        }
        if (action === 'route') {
            setActiveWorkflowStep('commit');
            setActiveDock('player');
            cycleWorkflowStep();
        }

        setInteractionToast(`${nodeActionLabels[action]} ${nodeTitles[focusedNode]} +${actionXp} XP · model ${learningModel.modelConfidence}%`);
    };
    const viewportTopInset = 40;
    const viewportBottomInset = 78;
    const dockPanelBottom = viewportBottomInset + (hasOpenDock ? 12 : 4);
    const loopRailBottom: number | string = hasOpenDock ? `calc(26dvh + ${viewportBottomInset + 44}px)` : viewportBottomInset + 72;
    const dockPanelMaxHeight = expandedDockPanel ? '48dvh' : (hasOpenDock ? '26dvh' : '34dvh');
    const leftRailInset = isCompactViewport ? 12 : 408;
    const rightRailInset = isCompactViewport ? 12 : 408;
    const leftTelemetryBottom = isCompactViewport ? 78 : 364;
    const questRailWidth = isCompactViewport
        ? 'min(360px, calc(100% - 24px))'
        : expandedQuestRail
            ? 'min(360px, calc(100% - 24px))'
            : 'min(320px, calc(100% - 24px))';
    const dockPanelWidth = isCompactViewport
        ? 'calc(100% - 24px)'
        : expandedDockPanel
            ? 'min(390px, calc(100% - 24px))'
            : 'min(340px, calc(100% - 24px))';
    const showQuestRail = !isCompactViewport;
    const showTelemetryRail = !isCompactViewport;
    const questRailBottom: number | string = expandedQuestRail
        ? viewportBottomInset + 20
        : (hasOpenDock ? `calc(24dvh + ${viewportBottomInset + 54}px)` : viewportBottomInset + 62);
    const questLoopItems: QuestLoopItem[] = visibleLoops.flatMap((loop) => {
        if (loop.id === null) {
            return [];
        }
        return [{
            id: loop.id,
            label: loop.label,
            value: loop.value,
            detail: loop.detail,
            accent: loop.accent,
        }];
    });
    const nodeOpsActions: NodeOpsActionSpec[] = (['snapshot', 'inspect', 'train', 'route'] as const).map((action) => ({
        id: action,
        label: nodeActionLabels[action],
        xp: learningModel.xpByAction[action],
        recommended: learningModel.recommendedAction === action,
    }));

    if (fullBleed) {
        return (
            <div style={{
                position: 'absolute',
                inset: 0,
                borderRadius: 0,
                border: 'none',
                background: `${scanlineUri}, radial-gradient(ellipse at 12% 8%, ${activeTheme.aura}, transparent 38%), radial-gradient(ellipse at 88% 92%, rgba(14, 165, 233, 0.12), transparent 40%), linear-gradient(160deg, #020c18 0%, #071525 55%, #050f1e 100%)`,
                overflow: 'hidden',
                zIndex: 1,
            }}>
                {(questToast.length > 0 || interactionToast.length > 0) ? (
                    <div style={{
                        position: 'absolute',
                        top: 16,
                        left: 16,
                        zIndex: 5,
                        borderRadius: 12,
                        border: '1px solid rgba(251, 191, 36, 0.6)',
                        background: 'linear-gradient(135deg, rgba(120, 53, 15, 0.9), rgba(146, 64, 14, 0.82))',
                        color: '#fef3c7',
                        padding: '10px 12px',
                        fontSize: 12,
                        letterSpacing: '0.05em',
                        textTransform: 'uppercase',
                        fontWeight: 700,
                        boxShadow: '0 14px 28px rgba(120, 53, 15, 0.38)',
                    }}>
                        {interactionToast.length > 0 ? interactionToast : questToast}
                    </div>
                ) : null}

                {!analyticsAvailability.available ? (
                    <div style={{
                        position: 'absolute',
                        top: 16,
                        right: 16,
                        zIndex: 5,
                        borderRadius: 12,
                        border: '1px solid rgba(244, 63, 94, 0.55)',
                        background: 'linear-gradient(140deg, rgba(76, 5, 25, 0.92), rgba(67, 20, 7, 0.85))',
                        color: '#fecdd3',
                        padding: '10px 12px',
                        fontSize: 11,
                        letterSpacing: '0.05em',
                        textTransform: 'uppercase',
                        fontWeight: 700,
                        boxShadow: '0 12px 24px rgba(76, 5, 25, 0.35)',
                    }}>
                        Analytics unavailable · {analyticsAvailability.reason} · cohort {analyticsAvailability.rollout.cohort}
                    </div>
                ) : null}

                {analyticsAvailability.available ? (
                    <div style={{
                        position: 'absolute',
                        top: 16,
                        right: 16,
                        zIndex: 5,
                        borderRadius: 12,
                        border: `1px solid ${abiLayerCoverage.complete ? 'rgba(16, 185, 129, 0.52)' : 'rgba(251, 191, 36, 0.55)'}`,
                        background: abiLayerCoverage.complete
                            ? 'linear-gradient(140deg, rgba(6, 78, 59, 0.9), rgba(6, 95, 70, 0.82))'
                            : 'linear-gradient(140deg, rgba(120, 53, 15, 0.9), rgba(146, 64, 14, 0.82))',
                        color: abiLayerCoverage.complete ? '#d1fae5' : '#fef3c7',
                        padding: '10px 12px',
                        fontSize: 11,
                        letterSpacing: '0.05em',
                        textTransform: 'uppercase',
                        fontWeight: 700,
                        boxShadow: abiLayerCoverage.complete
                            ? '0 12px 24px rgba(6, 78, 59, 0.35)'
                            : '0 12px 24px rgba(120, 53, 15, 0.35)',
                        display: 'grid',
                        gap: 4,
                        maxWidth: 340,
                    }}>
                        <span>ABI coverage {abiCoverageSummary} · {abiCoveragePercent}% · {abiCoverageState}</span>
                        <span style={{ fontSize: 10, color: abiLayerCoverage.complete ? 'rgba(187, 247, 208, 0.92)' : 'rgba(254, 240, 138, 0.92)' }}>
                            missing: {abiCoverageMissing}
                        </span>
                    </div>
                ) : null}

                <GameplayTopHud
                    selectedFile={selectedFile}
                    missionCode={missionSector.code}
                    missionTitle={missionSector.title}
                    routeMode={routeMode}
                    isCompactViewport={isCompactViewport}
                    k3h4Dimensions={k3h4Projection.dimensions}
                />

                <div style={{
                    position: 'absolute',
                    inset: `${viewportTopInset}px 0 ${viewportBottomInset}px`,
                    borderRadius: 0,
                    border: `1px solid ${activeTheme.accent}44`,
                    background: 'linear-gradient(180deg, rgba(2, 10, 22, 0.72), rgba(1, 7, 16, 0.68))',
                    overflow: 'hidden',
                    zIndex: 1,
                }}>
                    {/* Keyframe animations injected inline */}
                    <style>{`
                        @keyframes bng-pulse-ring { 0%,100%{transform:scale(1);opacity:.7} 50%{transform:scale(1.18);opacity:.25} }
                        @keyframes bng-flow { 0%{offset-distance:0%} 100%{offset-distance:100%} }
                        @keyframes bng-glow { 0%,100%{opacity:.35} 50%{opacity:.85} }
                        @keyframes bng-ticker { 0%{transform:translateY(0)} 100%{transform:translateY(-50%)} }
                        @keyframes bng-galaxy-spin { 0%{transform:rotate(0deg)} 100%{transform:rotate(360deg)} }
                        @keyframes bng-galaxy-near { 0%,100%{transform:translate(0px,0px) scale(1.02)} 50%{transform:translate(-2px,1px) scale(1.04)} }
                        @keyframes bng-galaxy-mid { 0%,100%{transform:translate(0px,0px) scale(1)} 50%{transform:translate(1px,-1px) scale(1.01)} }
                        @keyframes bng-galaxy-far { 0%,100%{transform:translate(0px,0px) scale(0.98)} 50%{transform:translate(2px,-1px) scale(0.97)} }
                    `}</style>

                    <GameplayGalaxyField
                        nodeAnchors={nodeAnchors}
                        k3h4OrbitLayers={k3h4OrbitLayers}
                        routeModeOrbitAccent={routeModeOrbitAccent[routeMode]}
                        renderedGalaxyNodes={renderedGalaxyNodes}
                        peripheralBranchMesh={peripheralBranchMesh}
                        accent={activeTheme.accent}
                        rewardBand={rewardBand}
                        activeDock={activeDock}
                        linkConfidence={linkConfidence}
                        laneContractCoherence={laneContractCoherence}
                        lanePulsePhase={lanePulsePhase}
                        hotspotScore={hotspotScore}
                    />

                    <GameplayCoreNode
                        selectedFile={selectedFile}
                        accent={activeTheme.accent}
                        aura={activeTheme.aura}
                        hotspotScore={hotspotScore}
                        riskLabel={riskLabel}
                        missionTitle={missionSector.title}
                        missionDomain={missionSector.domain}
                        rarityLabel={rarityLabel}
                        workflowOrder={workflowOrder}
                        activeWorkflowStep={activeWorkflowStep}
                        onSetWorkflowStep={setActiveWorkflowStep}
                        workflowDepth={workflowDepth}
                        onAdjustWorkflowDepth={(delta) => adjustWorkflowDepth(delta)}
                        workflowSummary={workflowSteps[activeWorkflowStep].summary}
                        indexedFileCount={indexedFileCount}
                        notebookCellCount={notebookCellCount}
                        onCycleCore={() => {
                            cycleWorkflowStep();
                            cycleNodeFocus();
                        }}
                    />

                    <NodeOrbCluster
                        nodeAnchors={nodeAnchors}
                        activeDock={activeDock}
                        accent={activeTheme.accent}
                        aura={activeTheme.aura}
                        callDensity={callDensity}
                        questPercent={questPercent}
                        effectivePlayerLevel={effectivePlayerLevel}
                        effectiveTotalXp={effectiveTotalXp}
                        functionCount={functionCount}
                        onIntelToggle={() => {
                            focusNode('intel');
                            if (taskbarOwnsIntelAndPlayerWindows) {
                                onToggleIntelNodeWindow?.();
                                return;
                            }
                            setActiveDock(activeDock === 'intel' ? null : 'intel');
                        }}
                        onObjectivesToggle={() => {
                            focusNode('objectives');
                            if (taskbarOwnsObjectiveAndQuestWindows) {
                                onToggleObjectiveNodeWindow?.();
                                return;
                            }
                            setGameplayWindowVisibility('objectiveNode', true);
                            setActiveDock(activeDock === 'objectives' ? null : 'objectives');
                        }}
                        onPlayerToggle={() => {
                            focusNode('player');
                            if (taskbarOwnsIntelAndPlayerWindows) {
                                onTogglePlayerNodeWindow?.();
                                return;
                            }
                            setActiveDock(activeDock === 'player' ? null : 'player');
                        }}
                        onActionNode={() => {
                            if (taskbarOwnsNodeOpsWindow) {
                                onToggleNodeOpsWindow?.();
                                return;
                            }
                            focusNode('ops');
                            setActiveWorkflowStep('commit');
                            setWorkflowDepth(3);
                            setActiveDock('player');
                            applyNodeAction('route');
                        }}
                    />

                    {!taskbarOwnsNodeOpsWindow ? (
                        <NodeOpsConsole
                            viewportBottomInset={viewportBottomInset}
                            hasOpenDock={hasOpenDock}
                            isCompactViewport={isCompactViewport}
                            focusedNodeTitle={nodeTitles[focusedNode]}
                            modelConfidence={learningModel.modelConfidence}
                            trainingAccuracy={learningModel.trainingAccuracy}
                            policyMomentum={learningModel.policyMomentum}
                            actions={nodeOpsActions}
                            onAction={applyNodeAction}
                            snapshots={interactionLedger.snapshots}
                            inspects={interactionLedger.inspections}
                            trainings={interactionLedger.trainings}
                            routes={interactionLedger.routes}
                            sessionXpGain={sessionXpGain}
                        />
                    ) : null}

                    {/* ─── Side telemetry rail (left) ───────────────────────── */}
                    {showTelemetryRail ? (
                        <TelemetryRail
                            leftRailInset={leftRailInset}
                            leftTelemetryBottom={leftTelemetryBottom}
                            lineCount={lineCount}
                            includeCount={includeCount}
                            callDensity={callDensity}
                            riskLabel={riskLabel}
                            k3h4Dimensions={k3h4Projection.dimensions}
                            k3h4Alignment={k3h4Projection.alignment}
                            cFileCount={cFileCount}
                            modelConfidence={learningModel.modelConfidence}
                            extraLines={[
                                `K3H4: ${k3h4ClusterCount} clusters`,
                                `Convergence: ${k3h4Convergence}`,
                                `ABI: ${abiCoverageSummary} (${abiCoveragePercent}%)`,
                                `Hash: ${k3h4HashPreview}`,
                            ]}
                        />
                    ) : null}

                    {/* ─── Quest log rail (right) ───────────────────────────── */}
                    {!taskbarOwnsObjectiveAndQuestWindows && showQuestRail && showQuestLogWindow ? (
                        <QuestLogRail
                            rightRailInset={rightRailInset}
                            questRailBottom={questRailBottom}
                            questRailWidth={questRailWidth}
                            expandedQuestRail={expandedQuestRail}
                            onToggleExpanded={() => setExpandedQuestRail((current) => !current)}
                            workflowStepLabel={`Step ${activeWorkflowStep} · Depth ${workflowDepth} · x${Math.max(1, comboStreak)}`}
                            loopItems={questLoopItems}
                            activeDock={activeDock}
                            onToggleLoopDock={(dockId) => {
                                focusNode(dockId);
                                setActiveDock(activeDock === dockId ? null : dockId);
                            }}
                            rewardBand={rewardBand}
                            questListMaxHeight={expandedQuestRail ? '34dvh' : (hasOpenDock ? '18dvh' : '24dvh')}
                            projectedRewardXp={projectedRewardXp}
                            statusLine={`K3H4 ${k3h4ClusterCount} · ${k3h4Convergence} · ABI ${abiCoverageSummary} · ${analyticsAvailability.rollout.cohort}`}
                            onMainAction={() => {
                                setActiveWorkflowStep('commit');
                                setWorkflowDepth(3);
                                setActiveDock('player');
                                focusNode('ops');
                                applyNodeAction('route');
                            }}
                            onClose={() => {
                                setGameplayWindowVisibility('questLog', false);
                            }}
                        />
                    ) : null}
                </div>

                {activeDock &&
                    (!taskbarOwnsIntelAndPlayerWindows || (activeDock !== 'intel' && activeDock !== 'player')) &&
                    (!taskbarOwnsObjectiveAndQuestWindows || activeDock !== 'objectives') &&
                    (activeDock !== 'objectives' || showObjectiveNodeWindow) ? (
                    <ActiveDockShell
                        rightRailInset={rightRailInset}
                        dockPanelBottom={dockPanelBottom}
                        dockPanelWidth={dockPanelWidth}
                        dockPanelMaxHeight={dockPanelMaxHeight}
                        dockTitle={activeDock === 'intel' ? 'Intel Node' : activeDock === 'objectives' ? 'Objective Node' : 'Player Node'}
                        expandedDockPanel={expandedDockPanel}
                        onToggleExpanded={() => setExpandedDockPanel((current) => !current)}
                        onClose={() => {
                            if (activeDock === 'objectives') {
                                setGameplayWindowVisibility('objectiveNode', false);
                            }
                            setActiveDock(null);
                        }}
                        summaryColor={rewardBand.line}
                        summaryLine={`Neural relevance ${neuralRelevanceScore}% · ${rewardTier} · Reward +${projectedRewardXp} XP · Session +${sessionXpGain} XP`}
                    >
                        {activeDock === 'intel' ? (
                            <IntelDockPanel
                                missionTitle={missionSector.title}
                                missionCode={missionSector.code}
                                sectorReadiness={Math.min(100, 42 + callDensity)}
                                intelMode={intelMode}
                                setIntelMode={setIntelMode}
                                developerIntelUnlocked={developerIntelUnlocked}
                                toggleDeveloperUnlock={toggleDeveloperUnlock}
                                intelPanel={intelPanel}
                                setIntelPanel={setIntelPanelDispatch}
                                hotspotScore={hotspotScore}
                                riskLabel={riskLabel}
                                branchPressure={branchPressure}
                                dependencyPulse={dependencyPulse}
                                callDensity={callDensity}
                                functionName={functionNames[0] ?? 'Unknown'}
                                functionCount={functionCount}
                                primaryDependencyLabel={primaryDependencyLabel}
                                includeCount={includeCount}
                                intelEvents={intelEvents}
                                showFullSourceDump={showFullSourceDump}
                                setShowFullSourceDump={setShowFullSourceDumpDispatch}
                                sourcePreviewWithLineNumbers={sourcePreviewWithLineNumbers}
                            />
                        ) : null}

                        {activeDock === 'objectives' ? (
                            <ObjectivesDockPanel
                                completedQuestCount={completedQuestCount}
                                questProgress={questProgress}
                                questPercent={questPercent}
                                objectivePanel={objectivePanel}
                                setObjectivePanel={setObjectivePanelDispatch}
                                nextDomainTarget={nextDomainTarget}
                            />
                        ) : null}

                        {activeDock === 'player' ? (
                            <PlayerDockPanel
                                playerPanel={playerPanel}
                                setPlayerPanel={setPlayerPanelDispatch}
                                playerLevel={effectivePlayerLevel}
                                rarityLabel={rarityLabel}
                                rarityAccent={rarityAccent}
                                totalXp={effectiveTotalXp}
                                lastXpGain={lastXpGain}
                                comboStreak={comboStreak}
                            />
                        ) : null}
                    </ActiveDockShell>
                ) : null}
                {/* Corner vignette borders */}
                <div style={{ position: 'absolute', inset: 0, borderRadius: 0, boxShadow: 'inset 0 0 60px rgba(2, 6, 23, 0.55)', pointerEvents: 'none', zIndex: 2 }} />
            </div>
        );
    }

    return (
        <SurfaceCard
            title="Mission Viewport"
            description="Source-driven runtime feed for the current sector."
        >
            <div style={{ display: 'flex', flexWrap: 'wrap', gap: 8 }}>
                <Pill color="#5eead4" borderColor="rgba(45, 212, 191, 0.4)">Sector: {selectedFile || 'none'}</Pill>
                <Pill color="#a5b4fc" borderColor="rgba(148, 163, 184, 0.35)">Lines: {lineCount}</Pill>
                <Pill color="#86efac" borderColor="rgba(34, 197, 94, 0.35)">Renderer: notebook</Pill>
                <Pill color={rarityAccent} borderColor="rgba(148, 163, 184, 0.35)">Rarity: {rarityLabel}</Pill>
                <Pill color="#fcd34d" borderColor="rgba(251, 191, 36, 0.35)">XP +{lastXpGain}</Pill>
            </div>
            <div style={{ marginTop: 10, width: '100%', borderRadius: 12, border: '1px solid rgba(45,212,191,0.35)', background: 'rgba(2,6,23,0.95)', fontFamily: '"IBM Plex Mono",monospace', fontSize: 13, lineHeight: 1.55, color: '#d1fae5', overflow: 'auto', whiteSpace: 'pre-wrap', overflowWrap: 'anywhere', padding: 14 } as CSSProperties}>
                {selectedContent}
            </div>
        </SurfaceCard>
    );
}
