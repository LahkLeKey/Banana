import { useEffect, useMemo, useState, type CSSProperties, type Dispatch, type SetStateAction } from 'react';
import { RoutePill as Pill, RouteSurfaceCard as SurfaceCard } from '@banana/ui';

import type { QuestProgress } from '../../lib/notebook-client/gamification';
import { buildIntelSignals, buildMissionIntelEvents } from '../../domain/notebook/intel-domain';
import { deriveMissionMeta } from '../../domain/notebook/mission-domain';
import {
    buildCFileContractNodeModel,
    buildContractHypersphereProjectionModel,
    buildContractNodeVectorModel,
    buildNodeLinkConfidenceModel,
    buildRewardSignalModel,
    type ContractNodeId,
} from '../../domain/notebook/network-domain';
import type { NotebookRouteMode } from '../../lib/notebook-client/routeModeConfig';
import { buildWorkflowStepMap, type WorkflowStepId } from '../../domain/notebook/workflow-domain';
import { useNotebookLayoutStore } from '../../lib/notebook-client/layoutStore';
import { IntelDockPanel, ObjectivesDockPanel, PlayerDockPanel } from './NotebookDockPanels';
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
}: NotebookGameplaySurfaceProps) {
    const [intelMode, setIntelMode] = useState<'summary' | 'raw'>('summary');
    const [developerIntelUnlocked, setDeveloperIntelUnlocked] = useState(false);
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
            value: Math.min(100, Math.round(playerLevel * 8 + comboStreak * 4)),
            accent: '#a78bfa',
            detail: `Lv${playerLevel} · x${Math.max(1, comboStreak)}`,
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
        playerLevel,
        questPercent,
        questProgress.length,
        riskLabel,
    ]);
    const rewardSignal = useMemo(() => buildRewardSignalModel({
        callDensity,
        questPercent,
        comboStreak,
        branchPressure,
        workflowDepth,
    }), [branchPressure, callDensity, comboStreak, questPercent, workflowDepth]);
    const { neuralRelevanceScore, projectedRewardXp, rewardTier, rewardBand } = rewardSignal;
    const cFileCount = useMemo(
        () => allIndexedFiles.filter((filePath) => /\.c$/i.test(filePath)).length,
        [allIndexedFiles],
    );
    const networkDimensions = useMemo(() => {
        if (cFileCount <= 0) {
            return 6;
        }

        // Expand hypersphere dimensionality as C-file evidence increases.
        return Math.min(16, Math.max(8, 6 + Math.floor(Math.log2(cFileCount + 1))));
    }, [cFileCount]);
    const linkConfidence = useMemo(() => buildNodeLinkConfidenceModel({
        callDensity,
        questPercent,
        playerLevel,
        comboStreak,
        branchPressure,
        dependencyPulse,
    }), [branchPressure, callDensity, comboStreak, dependencyPulse, playerLevel, questPercent]);
    const contractVectors = useMemo(() => buildContractNodeVectorModel({
        callDensity,
        questPercent,
        playerLevel,
        comboStreak,
        branchPressure,
        dependencyPulse,
        workflowDepth,
        neuralRelevanceScore,
        networkDimensions,
    }), [
        branchPressure,
        callDensity,
        comboStreak,
        dependencyPulse,
        networkDimensions,
        neuralRelevanceScore,
        playerLevel,
        questPercent,
        workflowDepth,
    ]);
    const hypersphereProjection = useMemo(
        () => buildContractHypersphereProjectionModel({ nodes: contractVectors }),
        [contractVectors],
    );
    const laneContractCoherence = useMemo(() => {
        const coherenceMap = new Map<ContractNodeId, number>();
        hypersphereProjection.nodes.forEach((node) => {
            coherenceMap.set(node.id, node.coherence);
        });

        return {
            intel: coherenceMap.get('intel') ?? 50,
            objectives: coherenceMap.get('objectives') ?? 50,
            player: coherenceMap.get('player') ?? 50,
            ops: coherenceMap.get('ops') ?? 50,
        };
    }, [hypersphereProjection.nodes]);
    const hasOpenDock = activeDock !== null;
    const orbitLayerCount = useMemo(
        () => Math.max(2, Math.ceil(hypersphereProjection.dimensions / 2)),
        [hypersphereProjection.dimensions],
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
    const hypersphereOrbitLayers = useMemo(() => {
        const layerCount = orbitLayerCount;
        const alignmentWeight = 0.5 + hypersphereProjection.alignment / 250;
        const stabilityWeight = 0.5 + hypersphereProjection.radialStability / 250;
        const orbitProfile = routeModeOrbitProfile[routeMode];
        const hudHorizontalLoad =
            (showExplorer ? 0.12 : 0) +
            (showMenu ? 0.12 : 0) +
            (showStatus ? 0.06 : 0) +
            (showOperations ? 0.06 : 0);
        const hudVerticalLoad =
            ((showStatus || showOperations) ? 0.1 : 0) +
            (hasOpenDock ? 0.18 : 0) +
            ((showExplorer || showMenu) ? 0.04 : 0);
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
        hypersphereProjection.alignment,
        hypersphereProjection.dimensions,
        hypersphereProjection.radialStability,
        showExplorer,
        showMenu,
        showOperations,
        showStatus,
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
    const nodeAnchors = useMemo(() => {
        const center = { x: 50, y: hasOpenDock ? 44 : 48 };
        const leftX = showExplorer ? 24 : 18;
        const rightX = showMenu ? 76 : 82;
        const bottomLeftX = showStatus ? 16 : 12;
        const bottomRightX = showOperations ? 84 : 88;
        const topY = (showExplorer || showMenu) ? 28 : 20;
        const nominalBottomY = (showStatus || showOperations) ? 82 : 88;
        const bottomY = Math.min(nominalBottomY, hasOpenDock ? 78 : 86);
        const topSpan = Math.max(6, center.y - topY);
        const bottomSpan = Math.max(8, bottomY - center.y);
        const leftSpan = Math.max(8, center.x - leftX);
        const rightSpan = Math.max(8, rightX - center.x);

        const projectionById = new Map<ContractNodeId, { x: number; y: number; z: number; }>();
        hypersphereProjection.nodes.forEach((node) => {
            projectionById.set(node.id, { x: node.x, y: node.y, z: node.z });
        });

        const projectAnchor = (id: ContractNodeId, horizontal: 'left' | 'right', vertical: 'top' | 'bottom') => {
            const projection = projectionById.get(id);
            const px = Math.min(1, Math.abs(projection?.x ?? 0.58));
            const py = Math.min(1, Math.abs(projection?.y ?? 0.62));
            const pz = Math.min(1, Math.max(0, projection?.z ?? 0.5));
            const radialBias = 0.54 + px * 0.28 + (1 - pz) * 0.12;
            const verticalBias = 0.56 + py * 0.26 + (1 - pz) * 0.12;

            return {
                x: horizontal === 'left'
                    ? center.x - leftSpan * radialBias
                    : center.x + rightSpan * radialBias,
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
    }, [hasOpenDock, hypersphereProjection.nodes, showExplorer, showMenu, showOperations, showStatus]);
    const cFileVisualNodes = useMemo(() => {
        if (cFileContractNodes.length === 0 || hypersphereOrbitLayers.length === 0) {
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
            const layer = hypersphereOrbitLayers[node.layerHint % hypersphereOrbitLayers.length];
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
            const depthBand = node.signal >= 72 ? 'near' : node.signal >= 42 ? 'mid' : 'far';

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
    }, [cFileContractNodes, hypersphereOrbitLayers, nodeAnchors.center.x, nodeAnchors.center.y, orbitLayerCount]);
    const ambientGalaxyNodes = useMemo(() => {
        if (hypersphereOrbitLayers.length === 0) {
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
            const layer = hypersphereOrbitLayers[index % hypersphereOrbitLayers.length];
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
    }, [hypersphereOrbitLayers, nodeAnchors.center.x, nodeAnchors.center.y]);
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
        if (!developerIntelUnlocked && intelMode === 'raw') {
            setIntelMode('summary');
        }
    }, [developerIntelUnlocked, intelMode]);

    useEffect(() => {
        if (intelMode !== 'raw') {
            setShowFullSourceDump(false);
        }
    }, [intelMode, setShowFullSourceDump]);

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
    const viewportBottomInset = 0;
    const loopRailBottom = hasOpenDock ? 266 : 104;
    const dockPanelBottom = hasOpenDock ? 88 : 70;
    const dockPanelMaxHeight = hasOpenDock ? '32dvh' : '40dvh';

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
                {questToast.length > 0 ? (
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
                        {questToast}
                    </div>
                ) : null}

                <div style={{
                    position: 'absolute',
                    top: 14,
                    left: '50%',
                    transform: 'translateX(-50%)',
                    zIndex: 3,
                    fontSize: 11,
                    color: 'rgba(103, 232, 249, 0.78)',
                    textTransform: 'uppercase',
                    letterSpacing: '0.12em',
                    pointerEvents: 'none',
                    whiteSpace: 'nowrap',
                }}>
                    {selectedFile
                        ? `Sector Live Feed - ${missionSector.title} (${missionSector.code})`
                        : 'Rule Mining Constellation'}
                </div>

                <div style={{
                    position: 'absolute',
                    inset: `54px 0 ${viewportBottomInset}px`,
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

                    {/* Connection line SVG — reactive to activeDock */}
                    <svg aria-hidden="true" viewBox="0 0 100 100" preserveAspectRatio="none"
                        style={{ position: 'absolute', inset: 0, width: '100%', height: '100%', pointerEvents: 'none' }}>
                        {/* Grid */}
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

                        {/* Hypersphere n/2 orbit layers */}
                        {hypersphereOrbitLayers.map((layer) => (
                            <ellipse
                                key={layer.key}
                                cx={nodeAnchors.center.x}
                                cy={nodeAnchors.center.y}
                                rx={layer.radius}
                                ry={layer.radius * layer.squash}
                                fill="none"
                                stroke={routeModeOrbitAccent[routeMode]}
                                strokeOpacity={layer.opacity}
                                strokeWidth={layer.strokeWidth}
                                transform={`rotate(${layer.rotate} ${nodeAnchors.center.x} ${nodeAnchors.center.y})`}
                            />
                        ))}

                        {/* Perspective haze to create depth falloff */}
                        <ellipse
                            cx={nodeAnchors.center.x}
                            cy={nodeAnchors.center.y + 1.6}
                            rx={(hypersphereOrbitLayers[hypersphereOrbitLayers.length - 1]?.radius ?? 18) * 1.12}
                            ry={(hypersphereOrbitLayers[hypersphereOrbitLayers.length - 1]?.radius ?? 18) * 0.58}
                            fill="rgba(10, 25, 49, 0.18)"
                        />

                        {/* Subtle nebula shell to reduce flatness perception */}
                        <ellipse
                            cx={nodeAnchors.center.x}
                            cy={nodeAnchors.center.y}
                            rx={hypersphereOrbitLayers[hypersphereOrbitLayers.length - 1]?.radius ?? 18}
                            ry={(hypersphereOrbitLayers[hypersphereOrbitLayers.length - 1]?.radius ?? 18) * 0.52}
                            fill="none"
                            stroke="rgba(59, 130, 246, 0.14)"
                            strokeWidth="1.2"
                        />

                        {/* C-file node cloud (all indexed .c files mapped into n/2 layers) */}
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

                        {/* Dust lanes across the core for depth occlusion */}
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

                        {/* Core shadow ridge to separate foreground and background planes */}
                        <ellipse
                            cx={nodeAnchors.center.x}
                            cy={nodeAnchors.center.y + 2.4}
                            rx={(hypersphereOrbitLayers[Math.max(0, hypersphereOrbitLayers.length - 2)]?.radius ?? 14) * 0.9}
                            ry={(hypersphereOrbitLayers[Math.max(0, hypersphereOrbitLayers.length - 2)]?.radius ?? 14) * 0.24}
                            fill="rgba(2, 6, 23, 0.32)"
                        />

                        {/* Peripheral branch mesh (N-way contract cross-links) */}
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
                                        stroke={activeTheme.accent}
                                        strokeOpacity={strokeOpacity}
                                        strokeWidth={Math.max(0.1, branch.branchStrength / 620)}
                                        strokeDasharray="0.9 2"
                                    />
                                    <circle r="0.52" fill={activeTheme.accent} opacity={pulseOpacity}>
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

                        {/* Center ← → Intel */}
                        <line x1={nodeAnchors.center.x} y1={nodeAnchors.center.y} x2={nodeAnchors.intel.x} y2={nodeAnchors.intel.y}
                            stroke={activeDock === 'intel' ? activeTheme.accent : rewardBand.soft}
                            strokeWidth={activeDock === 'intel' ? '0.7' : `${Math.max(0.3, (linkConfidence.intel / 180) * (0.7 + laneContractCoherence.intel / 160))}`} />
                        <circle r="1.1" fill={activeTheme.accent} opacity={Math.max(0.35, linkConfidence.intel / 120)}>
                            <animateMotion
                                begin={`${lanePulsePhase.intel.forward}s`}
                                dur={`${Math.max(1, 2.4 - linkConfidence.intel / 80)}s`}
                                repeatCount="indefinite"
                                path={`M${nodeAnchors.center.x},${nodeAnchors.center.y} L${nodeAnchors.intel.x},${nodeAnchors.intel.y}`}
                            />
                        </circle>
                        <circle r="0.9" fill={activeTheme.accent} opacity={Math.max(0.28, linkConfidence.intel / 150)}>
                            <animateMotion
                                begin={`${lanePulsePhase.intel.reverse}s`}
                                dur={`${Math.max(1.1, 2.8 - linkConfidence.intel / 85)}s`}
                                repeatCount="indefinite"
                                path={`M${nodeAnchors.intel.x},${nodeAnchors.intel.y} L${nodeAnchors.center.x},${nodeAnchors.center.y}`}
                            />
                        </circle>

                        {/* Center ← → Objectives */}
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

                        {/* Center ← → Player */}
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

                        {/* Center ← → Workflow (bottom-right) */}
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

                        {/* Center node outer ring pulse */}
                        <circle cx="50" cy="48" r="5" fill="none"
                            stroke={rewardBand.line} strokeWidth="0.4" opacity="0.44"
                            style={{ animation: 'bng-glow 2.2s ease-in-out infinite' }} />
                        <circle cx="50" cy="48" r="8.5" fill="none"
                            stroke={rewardBand.line} strokeWidth="0.25" opacity="0.2" />

                        {/* Live signal bars at top (hotspot) */}
                        {[0, 1, 2, 3, 4, 5, 6, 7].map((i) => (
                            <rect key={i} x={47 + i * 1.2} y={10} width="0.7"
                                height={Math.max(1, (hotspotScore / 100) * 8 * (0.4 + 0.6 * Math.sin(i * 0.9 + hotspotScore * 0.05)))}
                                fill={activeTheme.accent} opacity="0.5" />
                        ))}
                    </svg>

                    {/* ─── CENTER NODE ──────────────────────────────────────── */}
                    <div style={{
                        position: 'absolute',
                        left: '50%', top: '48%',
                        transform: 'translate(-50%, -50%)',
                        textAlign: 'center',
                        pointerEvents: 'auto',
                        zIndex: 3,
                    }}>
                        <button
                            type="button"
                            onClick={() => {
                                cycleWorkflowStep();
                            }}
                            aria-label="Cycle workflow step"
                            style={{ border: 'none', background: 'transparent', padding: 0, cursor: 'pointer' }}
                        >
                            <div style={{
                                width: 72, height: 72,
                                borderRadius: '50%',
                                border: `2px solid ${activeTheme.accent}`,
                                background: `radial-gradient(circle, ${activeTheme.aura}, rgba(2,6,23,0.85) 70%)`,
                                boxShadow: `0 0 28px ${activeTheme.aura}, 0 0 8px ${activeTheme.accent}44`,
                                display: 'flex', flexDirection: 'column', alignItems: 'center', justifyContent: 'center',
                                margin: '0 auto',
                                animation: 'bng-pulse-ring 3s ease-in-out infinite',
                            }}>
                                {selectedFile ? (
                                    <>
                                        <div style={{ fontSize: 18, fontWeight: 800, color: activeTheme.accent, lineHeight: 1 }}>{hotspotScore}</div>
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
                                <div style={{ fontSize: 11, fontWeight: 700, color: '#e2e8f0', whiteSpace: 'nowrap' }}>{missionSector.title}</div>
                                <div style={{ fontSize: 10, color: activeTheme.accent, textTransform: 'uppercase', letterSpacing: '0.06em' }}>{missionSector.domain} · {rarityLabel}</div>
                                <div style={{ display: 'flex', gap: 4 }}>
                                    {workflowOrder.map((step) => (
                                        <button
                                            key={step}
                                            type="button"
                                            onClick={() => setActiveWorkflowStep(step)}
                                            style={{
                                                borderRadius: 999,
                                                border: activeWorkflowStep === step ? `1px solid ${activeTheme.accent}` : '1px solid rgba(148, 163, 184, 0.32)',
                                                background: activeWorkflowStep === step ? 'rgba(8, 47, 73, 0.7)' : 'rgba(2, 10, 22, 0.64)',
                                                color: activeWorkflowStep === step ? activeTheme.accent : '#94a3b8',
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
                                        onClick={() => adjustWorkflowDepth(-1)}
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
                                        onClick={() => adjustWorkflowDepth(1)}
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
                                    {workflowSteps[activeWorkflowStep].summary}
                                </div>
                            </div>
                        ) : (
                            <div style={{ marginTop: 6, fontSize: 11, color: '#475569' }}>
                                Mining {indexedFileCount} sectors · {notebookCellCount} cells
                            </div>
                        )}
                    </div>

                    {/* ─── INTEL NODE (top-left) ────────────────────────────── */}
                    <button
                        type="button"
                        aria-label="Open Intel dock"
                        onClick={() => setActiveDock(activeDock === 'intel' ? null : 'intel')}
                        style={{
                            position: 'absolute', left: `${nodeAnchors.intel.x}%`, top: `${nodeAnchors.intel.y}%`, transform: 'translate(-50%,-50%)',
                            background: 'transparent', border: 'none', cursor: 'pointer', textAlign: 'center', zIndex: 3,
                        }}
                    >
                        <div style={{
                            width: 52, height: 52, borderRadius: '50%', margin: '0 auto',
                            border: `1.5px solid ${activeDock === 'intel' ? activeTheme.accent : 'rgba(148,163,184,0.35)'}`,
                            background: activeDock === 'intel'
                                ? `radial-gradient(circle, ${activeTheme.aura}, rgba(2,6,23,0.9) 70%)`
                                : 'rgba(2,8,20,0.72)',
                            boxShadow: activeDock === 'intel' ? `0 0 18px ${activeTheme.aura}` : 'none',
                            display: 'flex', flexDirection: 'column', alignItems: 'center', justifyContent: 'center',
                            transition: 'border-color 0.2s, box-shadow 0.2s',
                        }}>
                            <div style={{ fontSize: 16, fontWeight: 800, color: activeDock === 'intel' ? activeTheme.accent : '#cbd5e1', lineHeight: 1 }}>{callDensity}%</div>
                            <div style={{ fontSize: 8, textTransform: 'uppercase', color: '#64748b', marginTop: 2 }}>Signal</div>
                        </div>
                        <div style={{ marginTop: 4, fontSize: 10, fontWeight: 700, color: activeDock === 'intel' ? activeTheme.accent : '#94a3b8', textTransform: 'uppercase', letterSpacing: '0.06em' }}>Intel</div>
                        <div style={{ fontSize: 9, color: '#475569' }}>{riskLabel}</div>
                    </button>

                    {/* ─── OBJECTIVES NODE (bottom-left) ────────────────────── */}
                    <button
                        type="button"
                        aria-label="Open Objectives dock"
                        onClick={() => setActiveDock(activeDock === 'objectives' ? null : 'objectives')}
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
                        <div style={{ marginTop: 4, fontSize: 10, fontWeight: 700, color: activeDock === 'objectives' ? '#38bdf8' : '#94a3b8', textTransform: 'uppercase', letterSpacing: '0.06em' }}>Quests</div>
                        <div style={{ fontSize: 9, color: '#475569' }}>{completedQuestCount}/{questProgress.length}</div>
                    </button>

                    {/* ─── PLAYER NODE (top-right) ──────────────────────────── */}
                    <button
                        type="button"
                        aria-label="Open Player dock"
                        onClick={() => setActiveDock(activeDock === 'player' ? null : 'player')}
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
                            <div style={{ fontSize: 16, fontWeight: 800, color: activeDock === 'player' ? '#a78bfa' : '#cbd5e1', lineHeight: 1 }}>Lv{playerLevel}</div>
                            <div style={{ fontSize: 8, textTransform: 'uppercase', color: '#64748b', marginTop: 2 }}>{totalXp} xp</div>
                        </div>
                        <div style={{ marginTop: 4, fontSize: 10, fontWeight: 700, color: activeDock === 'player' ? '#a78bfa' : '#94a3b8', textTransform: 'uppercase', letterSpacing: '0.06em' }}>Player</div>
                        <div style={{ fontSize: 9, color: '#475569' }}>x{Math.max(1, comboStreak)} combo</div>
                    </button>

                    {/* ─── WORKFLOW NODE (bottom-right) ─────────────────────── */}
                    <button
                        type="button"
                        aria-label="Activate main action orb"
                        onClick={() => {
                            setActiveWorkflowStep('commit');
                            setWorkflowDepth(3);
                            setActiveDock('player');
                        }}
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
                        <div style={{ marginTop: 4, fontSize: 10, fontWeight: 700, color: '#fbbf24', textTransform: 'uppercase', letterSpacing: '0.06em' }}>Action</div>
                        <div style={{ fontSize: 9, color: '#9ca3af' }}>Prime +{projectedRewardXp} XP</div>
                    </button>

                    {/* ─── Live ticker strip (top) ──────────────────────────── */}
                    <div style={{
                        position: 'absolute', top: 8, left: 0, right: 0,
                        display: 'flex', justifyContent: 'center', gap: 12,
                        pointerEvents: 'none', zIndex: 2,
                    }}>
                        {selectedFile ? (
                            <>
                                <span style={{ fontSize: 10, color: activeTheme.accent, letterSpacing: '0.08em', textTransform: 'uppercase' }}>
                                    {missionSector.title} ({missionSector.code})
                                </span>
                                <span style={{ fontSize: 10, color: '#475569' }}>·</span>
                                <span style={{ fontSize: 10, color: '#94a3b8' }}>{lineCount} lines · {includeCount} deps</span>
                                <span style={{ fontSize: 10, color: '#475569' }}>·</span>
                                <span style={{ fontSize: 10, color: riskLabel === 'Critical' ? '#fda4af' : riskLabel === 'Elevated' ? '#fcd34d' : '#86efac' }}>
                                    {riskLabel}
                                </span>
                                <span style={{ fontSize: 10, color: '#475569' }}>·</span>
                                <span style={{ fontSize: 10, color: '#67e8f9' }}>
                                    Hyper D{hypersphereProjection.dimensions} align {hypersphereProjection.alignment}%
                                </span>
                                <span style={{ fontSize: 10, color: '#475569' }}>·</span>
                                <span style={{ fontSize: 10, color: '#a5b4fc' }}>
                                    N/2 orbits {Math.max(2, Math.ceil(hypersphereProjection.dimensions / 2))}
                                </span>
                            </>
                        ) : (
                            <>
                                <span style={{ fontSize: 10, color: '#67e8f9', letterSpacing: '0.08em', textTransform: 'uppercase' }}>
                                    All-sector rule mining
                                </span>
                                <span style={{ fontSize: 10, color: '#475569' }}>·</span>
                                <span style={{ fontSize: 10, color: '#94a3b8' }}>
                                    {indexedFileCount} sectors · {notebookCellCount} cells
                                </span>
                                <span style={{ fontSize: 10, color: '#475569' }}>·</span>
                                <span style={{ fontSize: 10, color: '#a5b4fc' }}>
                                    Hyper D{hypersphereProjection.dimensions} · N/2 {Math.max(2, Math.ceil(hypersphereProjection.dimensions / 2))}
                                </span>
                                <span style={{ fontSize: 10, color: '#475569' }}>·</span>
                                <span style={{ fontSize: 10, color: '#7dd3fc' }}>
                                    C files {cFileCount}
                                </span>
                            </>
                        )}
                    </div>

                    <div style={{
                        position: 'absolute',
                        left: '50%',
                        bottom: loopRailBottom,
                        transform: 'translateX(-50%)',
                        width: 'min(840px, calc(100% - 24px))',
                        display: 'grid',
                        gridTemplateColumns: `repeat(${visibleLoops.length}, minmax(120px, 1fr))`,
                        gap: 8,
                        zIndex: 3,
                    }}>
                        {visibleLoops.map((loop) => {
                            const active = loop.id !== null && activeDock === loop.id;
                            return (
                                <button
                                    key={loop.label}
                                    type="button"
                                    onClick={() => {
                                        if (loop.id === null) {
                                            setActiveWorkflowStep('commit');
                                            setWorkflowDepth(3);
                                            setActiveDock('player');
                                            return;
                                        }
                                        setActiveDock(activeDock === loop.id ? null : loop.id);
                                    }}
                                    style={{
                                        borderRadius: 10,
                                        border: active ? `1px solid ${loop.accent}` : `1px solid ${rewardBand.soft}`,
                                        background: active ? 'rgba(8, 47, 73, 0.5)' : 'rgba(2, 10, 22, 0.64)',
                                        color: '#e2e8f0',
                                        padding: '7px 8px',
                                        textAlign: 'left',
                                        cursor: 'pointer',
                                        opacity: 1,
                                        boxShadow: active ? `0 0 16px ${rewardBand.glow}` : 'none',
                                    }}
                                >
                                    <div style={{ display: 'flex', justifyContent: 'space-between', gap: 6, alignItems: 'baseline' }}>
                                        <span style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.07em', color: loop.accent, fontWeight: 700 }}>{loop.label}</span>
                                        <span style={{ fontSize: 11, fontWeight: 700 }}>{loop.value}%</span>
                                    </div>
                                    <div style={{ marginTop: 5, height: 4, borderRadius: 999, background: 'rgba(15, 23, 42, 0.85)', overflow: 'hidden' }}>
                                        <div style={{ width: `${loop.value}%`, height: '100%', background: loop.accent, transition: 'width 220ms ease' }} />
                                    </div>
                                    <div style={{ marginTop: 5, fontSize: 9, color: '#94a3b8', whiteSpace: 'nowrap', overflow: 'hidden', textOverflow: 'ellipsis' }}>
                                        {loop.id === null ? `Prime reward +${projectedRewardXp} XP` : loop.detail}
                                    </div>
                                </button>
                            );
                        })}
                    </div>
                </div>

                <div style={{
                    position: 'absolute',
                    left: '50%',
                    bottom: dockPanelBottom,
                    transform: 'translateX(-50%)',
                    width: 'min(980px, calc(100% - 20px))',
                    zIndex: 4,
                    display: 'grid',
                    gap: 8,
                }}>
                    {activeDock ? (
                        <div style={{
                            borderRadius: 14,
                            border: '1px solid rgba(94, 234, 212, 0.22)',
                            background: 'linear-gradient(180deg, rgba(2, 10, 22, 0.72), rgba(2, 8, 20, 0.7))',
                            boxShadow: '0 12px 20px rgba(2, 6, 23, 0.28)',
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
                                <div style={{ display: 'flex', gap: 6, flexWrap: 'wrap' }}>
                                    {([
                                        { id: 'intel', label: 'Intel Node' },
                                        { id: 'objectives', label: 'Objective Node' },
                                        { id: 'player', label: 'Player Node' },
                                    ] as const).map((node) => (
                                        <button
                                            key={node.id}
                                            type="button"
                                            onClick={() => setActiveDock(node.id)}
                                            style={{
                                                borderRadius: 999,
                                                border: activeDock === node.id ? '1px solid rgba(94, 234, 212, 0.55)' : '1px solid rgba(148, 163, 184, 0.28)',
                                                background: activeDock === node.id ? 'rgba(13, 148, 136, 0.22)' : 'rgba(8, 13, 28, 0.48)',
                                                color: activeDock === node.id ? '#99f6e4' : '#cbd5e1',
                                                padding: '4px 10px',
                                                fontSize: 10,
                                                fontWeight: 700,
                                                letterSpacing: '0.05em',
                                                textTransform: 'uppercase',
                                                cursor: 'pointer',
                                            }}
                                        >
                                            {node.label}
                                        </button>
                                    ))}
                                </div>

                                <button
                                    type="button"
                                    onClick={() => {
                                        setActiveWorkflowStep('commit');
                                        setWorkflowDepth(3);
                                        setActiveDock('player');
                                    }}
                                    style={{
                                        border: '1px solid rgba(16, 185, 129, 0.45)',
                                        borderRadius: 999,
                                        background: 'rgba(5, 46, 22, 0.8)',
                                        color: '#86efac',
                                        fontSize: 10,
                                        fontWeight: 700,
                                        letterSpacing: '0.05em',
                                        padding: '4px 10px',
                                        cursor: 'pointer',
                                        textTransform: 'uppercase',
                                        whiteSpace: 'nowrap',
                                    }}
                                >
                                    Prime Reward
                                </button>
                            </div>

                            <div style={{
                                fontSize: 10,
                                color: rewardBand.line,
                                letterSpacing: '0.05em',
                                textTransform: 'uppercase',
                            }}>
                                Neural relevance {neuralRelevanceScore}% · {rewardTier} · Reward +{projectedRewardXp} XP
                            </div>

                            <div style={{ maxHeight: '31dvh', overflowY: 'auto', paddingRight: 4 }}>
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
                                        playerLevel={playerLevel}
                                        rarityLabel={rarityLabel}
                                        rarityAccent={rarityAccent}
                                        totalXp={totalXp}
                                        lastXpGain={lastXpGain}
                                        comboStreak={comboStreak}
                                    />
                                ) : null}
                            </div>
                        </div>
                    ) : null}

                </div>
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
