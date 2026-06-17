import { RouteDeckTransition, ResizableDockGrid, RouteHudControlStrip, RouteTopBar } from '@banana/ui';
import { useCallback, useEffect, useMemo, useState, type CSSProperties } from 'react';
import { useLocation } from 'react-router-dom';

import { MainMenuPanel } from '../components/notebook-client/MainMenuPanel';
import { ObjectivesDockPanel } from '../components/notebook-client/NotebookDockPanels';
import { NotebookExplorerPanel } from '../components/notebook-client/NotebookExplorerPanel';
import { NotebookGameplaySurface } from '../components/notebook-client/NotebookGameplaySurface';
import { NotebookHealthPanel } from '../components/notebook-client/NotebookHealthPanel';
import { NotebookOperationsPanel } from '../components/notebook-client/NotebookOperationsPanel';
import { NotebookVisualizationsPanel } from '../components/notebook-client/NotebookVisualizationsPanel';
import { RouteModePanel } from '../components/notebook-client/RouteModePanel';
import { NotebookTrainingPanel } from '../domain/notebook/viz/NotebookTrainingPanel';
import { withErrorBoundary } from '../components/errors/withErrorBoundary';
import {
    buildQuestProgress,
    calculateLevel,
    calculateSectorXp,
    deriveNextCoreDomainTarget,
    deriveSectorDomain,
    deriveSectorRarity,
    type SectorDomain,
} from '../lib/notebook-client/gamification';
import { useNotebookLayoutStore } from '../lib/notebook-client/layoutStore';
import { resolveRouteHudPreset, type NotebookRouteMode } from '../lib/notebook-client/routeModeConfig';
import { useNotebookWindowOrchestrator, type NotebookHudPanelId } from '../lib/notebook-client/useNotebookWindowOrchestrator';
import { useNotebookClient } from '../lib/notebook-client/useNotebookClient';

const SafeNotebookGameplaySurface =
    withErrorBoundary(NotebookGameplaySurface, {
        componentName: 'NotebookGameplaySurface',
    });
const SafeNotebookExplorerPanel = withErrorBoundary(NotebookExplorerPanel, {
    componentName: 'NotebookExplorerPanel',
});
const SafeMainMenuPanel = withErrorBoundary(MainMenuPanel, {
    componentName: 'MainMenuPanel',
});
const SafeRouteModePanel = withErrorBoundary(RouteModePanel, {
    componentName: 'RouteModePanel',
});
const SafeNotebookHealthPanel = withErrorBoundary(NotebookHealthPanel, {
    componentName: 'NotebookHealthPanel',
});
const SafeNotebookOperationsPanel = withErrorBoundary(NotebookOperationsPanel, {
    componentName: 'NotebookOperationsPanel',
});
const SafeNotebookVisualizationsPanel = withErrorBoundary(NotebookVisualizationsPanel, {
    componentName: 'NotebookVisualizationsPanel',
});
const SafeNotebookTrainingPanel = withErrorBoundary(NotebookTrainingPanel, {
    componentName: 'NotebookTrainingPanel',
});

const shellStyle: CSSProperties = {
    height: '100dvh',
    overflow: 'hidden',
    background:
        'radial-gradient(circle at 8% 10%, rgba(20, 184, 166, 0.22), transparent 28%), radial-gradient(circle at 88% 14%, rgba(14, 165, 233, 0.16), transparent 34%), linear-gradient(155deg, #02050c 0%, #071322 45%, #0a1b2d 100%)',
    color: '#e2e8f0',
    padding: 0,
    fontFamily: '"Rajdhani", "Segoe UI", sans-serif',
};

const stageStyle: CSSProperties = {
    position: 'relative',
    height: '100dvh',
    overflow: 'hidden',
    padding: 0,
};

const panelResetDefaults: Record<NotebookHudPanelId, number> = {
    explorer: 0,
    menu: 0,
    status: 0,
    operations: 0,
    visualizations: 1,
    training: 1,
    intelNode: 0,
    objectiveNode: 0,
    playerNode: 0,
    questLog: 0,
    nodeOps: 0,
};

const panelDefaultSizes: Record<NotebookHudPanelId, { width: number; height: number }> = {
    menu: { width: 360, height: 260 },
    playerNode: { width: 340, height: 180 },
    explorer: { width: 360, height: 300 },
    intelNode: { width: 340, height: 180 },
    status: { width: 350, height: 220 },
    questLog: { width: 360, height: 260 },
    objectiveNode: { width: 360, height: 260 },
    nodeOps: { width: 360, height: 220 },
    operations: { width: 360, height: 240 },
    visualizations: { width: 460, height: 680 },
    training: { width: 440, height: 680 },
};

type NotebookAnalyticsTelemetry = {
    analyticsAvailable: boolean;
    analyticsCohort: string;
    k3h4Clusters: number;
    k3h4Convergence: string;
    k3h4RuntimeMode: 'multiplicative' | 'power';
    k3h4ProjectionNodes: readonly {
        id: 'intel' | 'objectives' | 'player' | 'ops';
        x: number;
        y: number;
        z: number;
        coherence: number;
        inradius: number;
        nearestNeighborDistance: number;
    }[];
    k3h4Centers: readonly {
        clusterId: number;
        centerQ16: readonly number[];
        memberVectorIds: readonly number[];
        memberCount: number;
    }[];
    k3h4Radii: readonly {
        clusterId: number;
        nearestNeighborDistanceQ16: number;
        inscribedRadiusQ16: number;
        radiusState: 'ok' | 'degenerate' | 'invalid';
    }[];
    k3h4WeightedVoronoiScores: readonly {
        vectorId: number;
        clusterId: number;
        distanceToCenterQ16: number;
        weightedScoreQ16: number;
        scoreValidity: 'valid' | 'clamped' | 'invalid';
    }[];
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
};

const DEFAULT_NOTEBOOK_ANALYTICS_TELEMETRY: NotebookAnalyticsTelemetry = {
    analyticsAvailable: true,
    analyticsCohort: 'default',
    k3h4Clusters: 0,
    k3h4Convergence: 'unknown',
    k3h4RuntimeMode: 'multiplicative',
    k3h4ProjectionNodes: [],
    k3h4Centers: [],
    k3h4Radii: [],
    k3h4WeightedVoronoiScores: [],
    abiCoveragePresent: 0,
    abiCoverageExpected: 5,
    abiCoveragePercent: 0,
    abiCoverageComplete: false,
    abiCoverageMissing: [],
    abiLayerLedger: [],
};

export function DataSciencePlaygroundPage() {
    const location = useLocation();
    const { loading, manifest, manifestError, manifestSource, notebookError, notebookSource, cellLookup, notebookCellCount } = useNotebookClient();
    const [query, setQuery] = useState('');
    const [selectedFile, setSelectedFile] = useState<string>('');
    const [recentFiles, setRecentFiles] = useState<string[]>([]);
    const {
        showExplorer,
        showMenu,
        showStatus,
        showOperations,
        showVisualizations,
        showTraining,
        showIntelNode,
        showObjectiveNode,
        showPlayerNode,
        showQuestLog,
        showNodeOps,
        panelVisibility,
        pinnedPanels,
        togglePanelPin,
        togglePanel,
        closePanel,
        closeForViewport,
        resetHudPanels,
        closeAllWindows,
        reopenAllWindows,
    } = useNotebookWindowOrchestrator();
    const objectivePanel = useNotebookLayoutStore((state) => state.objectivePanel);
    const setObjectivePanel = useNotebookLayoutStore((state) => state.setObjectivePanel);
    const [modeDeckAnimating, setModeDeckAnimating] = useState(false);
    const [prefersReducedMotion, setPrefersReducedMotion] = useState(false);
    const [capturedFiles, setCapturedFiles] = useState<string[]>([]);
    const [totalXp, setTotalXp] = useState(0);
    const [comboStreak, setComboStreak] = useState(0);
    const [comboDomain, setComboDomain] = useState<SectorDomain | ''>('');
    const [lastSelectionForXp, setLastSelectionForXp] = useState('');
    const [lastXpGain, setLastXpGain] = useState(0);
    const [questToast, setQuestToast] = useState('');
    const [announcedQuestIds, setAnnouncedQuestIds] = useState<string[]>([]);
    const [panelResetTokens, setPanelResetTokens] = useState<Record<NotebookHudPanelId, number>>(panelResetDefaults);
    const [analyticsTelemetry, setAnalyticsTelemetry] = useState<NotebookAnalyticsTelemetry>(
        DEFAULT_NOTEBOOK_ANALYTICS_TELEMETRY,
    );

    const routeHudPreset = useMemo(() => resolveRouteHudPreset(location.pathname), [location.pathname]);

    const allFiles = useMemo(() => manifest?.files ?? [], [manifest?.files]);

    const filteredFiles = useMemo(() => {
        const normalizedQuery = query.trim().toLowerCase();
        if (!normalizedQuery) {
            return allFiles;
        }
        return allFiles.filter((file) => file.toLowerCase().includes(normalizedQuery));
    }, [allFiles, query]);

    const handleSelectFile = useCallback((file: string) => {
        setSelectedFile(file);
        setRecentFiles(prev => [file, ...prev.filter(f => f !== file)].slice(0, 8));
    }, []);

    const handleResetAllSectors = useCallback(() => {
        setQuery('');
        setSelectedFile('');
        setRecentFiles([]);
        setCapturedFiles([]);
        setTotalXp(0);
        setComboStreak(0);
        setComboDomain('');
        setLastSelectionForXp('');
        setLastXpGain(0);
        setQuestToast('');
        setAnnouncedQuestIds([]);
        setPanelResetTokens(panelResetDefaults);
        resetHudPanels();
        closeForViewport();
    }, [closeForViewport, resetHudPanels]);

    const resetPanelLayout = useCallback((panel: NotebookHudPanelId) => {
        setPanelResetTokens((previous) => ({
            ...previous,
            [panel]: previous[panel] + 1,
        }));
    }, []);

    const handleResetHud = useCallback(() => {
        resetHudPanels();
        setPanelResetTokens((previous) => ({
            explorer: previous.explorer + 1,
            menu: previous.menu + 1,
            status: previous.status + 1,
            operations: previous.operations + 1,
            visualizations: previous.visualizations + 1,
            training: previous.training + 1,
            intelNode: previous.intelNode + 1,
            objectiveNode: previous.objectiveNode + 1,
            playerNode: previous.playerNode + 1,
            questLog: previous.questLog + 1,
            nodeOps: previous.nodeOps + 1,
        }));
    }, [resetHudPanels]);

    useEffect(() => {
        if (filteredFiles.length === 0) {
            setSelectedFile('');
            return;
        }

        if (selectedFile && !filteredFiles.includes(selectedFile)) {
            setSelectedFile('');
        }
    }, [filteredFiles, selectedFile]);

    useEffect(() => {
        setModeDeckAnimating(true);
        const timer = window.setTimeout(() => {
            setModeDeckAnimating(false);
        }, 360);

        return () => {
            window.clearTimeout(timer);
        };
    }, [routeHudPreset.mode]);

    useEffect(() => {
        if (typeof window === 'undefined' || typeof window.matchMedia !== 'function') {
            return;
        }

        const mediaQuery = window.matchMedia('(prefers-reduced-motion: reduce)');
        const applyPreference = () => {
            setPrefersReducedMotion(mediaQuery.matches);
        };

        applyPreference();
        mediaQuery.addEventListener('change', applyPreference);

        return () => {
            mediaQuery.removeEventListener('change', applyPreference);
        };
    }, []);

    useEffect(() => {
        if (!selectedFile || selectedFile === lastSelectionForXp) {
            return;
        }

        const sectorText = cellLookup.get(selectedFile) ?? '';
        const sectorLineCount = sectorText.length > 0 ? sectorText.split('\n').length : 1;
        const sectorDomain = deriveSectorDomain(selectedFile);
        const sectorRarity = deriveSectorRarity(selectedFile, sectorLineCount);
        const firstCapture = !capturedFiles.includes(selectedFile);
        const nextCombo = comboDomain === sectorDomain ? comboStreak + 1 : 1;
        const gainedXp = calculateSectorXp(
            sectorLineCount,
            sectorRarity.multiplier,
            nextCombo,
            firstCapture,
        );

        setCapturedFiles((previous) => firstCapture ? [...previous, selectedFile] : previous);
        setComboDomain(sectorDomain);
        setComboStreak(nextCombo);
        setTotalXp((previous) => previous + gainedXp);
        setLastXpGain(gainedXp);
        setLastSelectionForXp(selectedFile);
    }, [
        selectedFile,
        lastSelectionForXp,
        cellLookup,
        capturedFiles,
        comboDomain,
        comboStreak,
    ]);

    const aggregatedSectorContent = useMemo(() => {
        if (allFiles.length === 0) {
            return 'Select a file from the notebook index.';
        }

        const sectorBlocks = allFiles
            .map((file) => {
                const content = cellLookup.get(file);
                if (!content || content.trim().length === 0) {
                    return '';
                }
                return `// sector:${file}\n${content}`;
            })
            .filter((block) => block.length > 0);

        if (sectorBlocks.length === 0) {
            return 'Select a file from the notebook index.';
        }

        return sectorBlocks.join('\n\n');
    }, [allFiles, cellLookup]);
    const selectedContent = selectedFile
        ? (cellLookup.get(selectedFile) ?? 'No code cell available for this file.')
        : aggregatedSectorContent;
    const selectedLineCount = useMemo(
        () => selectedContent.split('\n').length,
        [selectedContent],
    );
    const selectedDomain = useMemo(
        () => deriveSectorDomain(selectedFile),
        [selectedFile],
    );
    const selectedRarity = useMemo(
        () => deriveSectorRarity(selectedFile, selectedLineCount),
        [selectedFile, selectedLineCount],
    );
    const playerLevel = useMemo(() => calculateLevel(totalXp), [totalXp]);
    const questProgress = useMemo(() => buildQuestProgress(capturedFiles), [capturedFiles]);
    const nextDomainTarget = useMemo(
        () => deriveNextCoreDomainTarget(capturedFiles),
        [capturedFiles],
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

    useEffect(() => {
        const newlyCompleted = questProgress.find(
            (quest) => quest.completed && !announcedQuestIds.includes(quest.id),
        );
        if (!newlyCompleted) {
            return;
        }

        setAnnouncedQuestIds((previous) => [...previous, newlyCompleted.id]);
        setQuestToast(`Quest Complete: ${newlyCompleted.title}`);
        const timer = window.setTimeout(() => {
            setQuestToast('');
        }, 2300);

        return () => {
            window.clearTimeout(timer);
        };
    }, [announcedQuestIds, completedQuestCount, questProgress]);
    const manifestLabel = manifest ? `Files indexed: ${manifest.file_count}` : 'Manifest pending';
    const sourceRootLabel = manifest ? `Source root: ${manifest.source_root}` : '';
    const maxLinesLabel = manifest ? `Max lines/file: ${manifest.max_lines_per_file}` : 'Run workflow to publish metadata';
    const sourceLabel = manifestSource.length > 0 ? `Manifest source: ${manifestSource}` : (notebookSource.length > 0 ? `Notebook source: ${notebookSource}` : '');
    const notebookAvailabilityLabel = manifest ? 'Artifacts loaded and indexed.' : (loading ? 'Loading notebook artifacts...' : 'Artifacts unavailable. Run scaffold workflow.');

    const dockRenderers = {
        explorer: (
            <SafeNotebookExplorerPanel
                files={filteredFiles}
                query={query}
                selectedFile={selectedFile}
                onChangeQuery={setQuery}
                onSelectFile={handleSelectFile}
                fileCountLabel={manifestLabel}
                maxLinesLabel={sourceLabel.length > 0 ? `${maxLinesLabel} | ${sourceLabel}` : maxLinesLabel}
                sourceRootLabel={sourceRootLabel}
                notebookManifestHref="/notebooks/catalog-index.json"
                notebookPayloadHref="/notebooks/native-c-catalog.ipynb"
                notebookAvailabilityLabel={notebookAvailabilityLabel}
            />
        ),
        menu: (
            <RouteDeckTransition
                reducedMotion={prefersReducedMotion}
                animating={modeDeckAnimating}
                delayMs={0}
                inactiveOpacity={0.74}
                inactiveTransform="translateY(8px) scale(0.988)"
                inactiveFilter="saturate(1.05)"
            >
                {routeHudPreset.mode === 'runtime'
                    ? (
                        <SafeMainMenuPanel
                            indexedFileCount={manifest?.file_count ?? 0}
                            notebookCellCount={notebookCellCount}
                            manifestAvailable={Boolean(manifest)}
                            playerLevel={playerLevel}
                            totalXp={totalXp}
                            comboStreak={comboStreak}
                            completedQuestCount={completedQuestCount}
                            totalQuestCount={questProgress.length}
                        />
                    )
                    : <SafeRouteModePanel mode={routeHudPreset.mode} />}
            </RouteDeckTransition>
        ),
        status: (
            <RouteDeckTransition
                reducedMotion={prefersReducedMotion}
                animating={modeDeckAnimating}
                delayMs={40}
                inactiveOpacity={0.86}
                inactiveTransform="translateY(7px) scale(1)"
                inactiveFilter="saturate(1.03)"
            >
                <SafeNotebookHealthPanel
                    loading={loading}
                    manifestError={manifestError}
                    notebookError={notebookError}
                    manifestSource={manifestSource}
                    notebookSource={notebookSource}
                />
            </RouteDeckTransition>
        ),
        operations: (
            <RouteDeckTransition
                reducedMotion={prefersReducedMotion}
                animating={modeDeckAnimating}
                delayMs={80}
                inactiveOpacity={0.8}
                inactiveTransform="translateY(9px) scale(0.992)"
                inactiveFilter="saturate(1.07)"
            >
                <SafeNotebookOperationsPanel
                    selectedFile={selectedFile}
                    selectedContent={selectedContent}
                    indexedFileCount={manifest?.file_count ?? 0}
                    notebookCellCount={notebookCellCount}
                    loading={loading}
                    manifestError={manifestError}
                    notebookError={notebookError}
                    manifestSource={manifestSource}
                    notebookSource={notebookSource}
                    analyticsAvailable={analyticsTelemetry.analyticsAvailable}
                    analyticsCohort={analyticsTelemetry.analyticsCohort}
                    k3h4Clusters={analyticsTelemetry.k3h4Clusters}
                    k3h4Convergence={analyticsTelemetry.k3h4Convergence}
                    abiCoveragePresent={analyticsTelemetry.abiCoveragePresent}
                    abiCoverageExpected={analyticsTelemetry.abiCoverageExpected}
                    abiCoveragePercent={analyticsTelemetry.abiCoveragePercent}
                    abiCoverageComplete={analyticsTelemetry.abiCoverageComplete}
                    abiCoverageMissing={analyticsTelemetry.abiCoverageMissing}
                    abiLayerLedger={analyticsTelemetry.abiLayerLedger}
                />
            </RouteDeckTransition>
        ),
        visualizations: (
            <RouteDeckTransition
                reducedMotion={prefersReducedMotion}
                animating={modeDeckAnimating}
                delayMs={100}
                inactiveOpacity={0.82}
                inactiveTransform="translateY(9px) scale(0.992)"
                inactiveFilter="saturate(1.05)"
            >
                <SafeNotebookVisualizationsPanel
                    analyticsAvailable={analyticsTelemetry.analyticsAvailable}
                    analyticsCohort={analyticsTelemetry.analyticsCohort}
                    k3h4Clusters={analyticsTelemetry.k3h4Clusters}
                    k3h4Convergence={analyticsTelemetry.k3h4Convergence}
                    k3h4RuntimeMode={analyticsTelemetry.k3h4RuntimeMode}
                    k3h4ProjectionNodes={analyticsTelemetry.k3h4ProjectionNodes}
                    k3h4Centers={analyticsTelemetry.k3h4Centers}
                    k3h4Radii={analyticsTelemetry.k3h4Radii}
                    k3h4WeightedVoronoiScores={analyticsTelemetry.k3h4WeightedVoronoiScores}
                />
            </RouteDeckTransition>
        ),
        training: (
            <RouteDeckTransition
                reducedMotion={prefersReducedMotion}
                animating={modeDeckAnimating}
                delayMs={120}
                inactiveOpacity={0.84}
                inactiveTransform="translateY(9px) scale(0.992)"
                inactiveFilter="saturate(1.04)"
            >
                <SafeNotebookTrainingPanel />
            </RouteDeckTransition>
        ),
        objectiveNode: (
            <ObjectivesDockPanel
                completedQuestCount={completedQuestCount}
                questProgress={questProgress}
                questPercent={questPercent}
                objectivePanel={objectivePanel}
                setObjectivePanel={(value) => {
                    if (typeof value === 'function') {
                        setObjectivePanel(value(objectivePanel));
                        return;
                    }
                    setObjectivePanel(value);
                }}
                nextDomainTarget={nextDomainTarget}
            />
        ),
        intelNode: (
            <div style={{ display: 'grid', gap: 8 }}>
                <div style={{ fontSize: 11, color: '#67e8f9', textTransform: 'uppercase', letterSpacing: '0.06em', fontWeight: 700 }}>
                    Intel Node
                </div>
                <div style={{ fontSize: 12, color: '#cbd5e1' }}>
                    Sector: {selectedFile || 'all sectors'}
                </div>
                <div style={{ fontSize: 11, color: '#94a3b8' }}>
                    Domain: {selectedDomain || 'other'} · Cells: {notebookCellCount}
                </div>
                <div style={{ fontSize: 11, color: '#94a3b8' }}>
                    Route Mode: {routeHudPreset.label}
                </div>
            </div>
        ),
        playerNode: (
            <div style={{ display: 'grid', gap: 8 }}>
                <div style={{ fontSize: 11, color: '#c4b5fd', textTransform: 'uppercase', letterSpacing: '0.06em', fontWeight: 700 }}>
                    Player Node
                </div>
                <div style={{ fontSize: 12, color: '#cbd5e1' }}>
                    Level {playerLevel} · XP {totalXp}
                </div>
                <div style={{ fontSize: 11, color: '#94a3b8' }}>
                    Combo x{Math.max(1, comboStreak)} · Last Gain +{lastXpGain}
                </div>
                <div style={{ fontSize: 11, color: '#94a3b8' }}>
                    Quests {completedQuestCount}/{questProgress.length}
                </div>
            </div>
        ),
        nodeOps: (
            <div style={{ display: 'grid', gap: 8 }}>
                <div style={{ fontSize: 11, color: '#fbbf24', textTransform: 'uppercase', letterSpacing: '0.06em', fontWeight: 700 }}>
                    Node Ops Console
                </div>
                <div style={{ fontSize: 12, color: '#cbd5e1' }}>
                    Active Sector: {selectedFile || 'none'}
                </div>
                <div style={{ fontSize: 11, color: '#94a3b8' }}>
                    Command actions Snapshot / Inspect / Train / Route are routed from the gameplay action node.
                </div>
                <div style={{ fontSize: 11, color: '#94a3b8' }}>
                    Current chain: x{Math.max(1, comboStreak)} · XP +{lastXpGain}
                </div>
            </div>
        ),
        questLog: (
            <div style={{ display: 'grid', gap: 8 }}>
                <div style={{ fontSize: 11, color: '#99f6e4', textTransform: 'uppercase', letterSpacing: '0.06em', fontWeight: 700 }}>
                    Quest Log
                </div>
                <div style={{ fontSize: 11, color: '#94a3b8' }}>
                    Completed {completedQuestCount}/{questProgress.length} · {questPercent}%
                </div>
                <div style={{ display: 'grid', gap: 6, maxHeight: '22dvh', overflowY: 'auto', paddingRight: 4 }}>
                    {questProgress.map((quest) => (
                        <div
                            key={quest.id}
                            style={{
                                display: 'flex',
                                justifyContent: 'space-between',
                                gap: 10,
                                alignItems: 'center',
                                fontSize: 12,
                                color: '#cbd5e1',
                                borderRadius: 8,
                                border: `1px solid ${quest.completed ? 'rgba(74, 222, 128, 0.5)' : 'rgba(148, 163, 184, 0.32)'}`,
                                background: quest.completed ? 'rgba(20, 83, 45, 0.45)' : 'rgba(15, 23, 42, 0.52)',
                                padding: '6px 8px',
                            }}
                        >
                            <span>{quest.title}</span>
                            <span style={{
                                color: quest.completed ? '#86efac' : '#7dd3fc',
                                fontWeight: 700,
                                whiteSpace: 'nowrap',
                            }}>
                                {quest.progress}/{quest.target}
                            </span>
                        </div>
                    ))}
                </div>
            </div>
        ),
    } as const;

    const dockLayout: Array<{ id: keyof typeof dockRenderers; corner: 'top-left' | 'top-right' | 'bottom-left' | 'bottom-right'; title: string; }> = [
        { id: 'visualizations', corner: 'top-left', title: 'VISUALIZATIONS' },
        { id: 'menu', corner: 'top-left', title: 'MAIN MENU' },
        { id: 'playerNode', corner: 'top-left', title: 'PLAYER NODE' },
        { id: 'training', corner: 'top-right', title: 'TRAINING' },
        { id: 'explorer', corner: 'top-right', title: 'FILE EXPLORER' },
        { id: 'intelNode', corner: 'top-right', title: 'INTEL NODE' },
        { id: 'status', corner: 'bottom-left', title: 'ROUTE REGISTRY' },
        { id: 'questLog', corner: 'bottom-left', title: 'QUEST LOG' },
        { id: 'objectiveNode', corner: 'bottom-right', title: 'OBJECTIVE NODE' },
        { id: 'nodeOps', corner: 'bottom-right', title: 'NODE OPS' },
        { id: 'operations', corner: 'bottom-right', title: 'OPERATIONS' },
    ];
    const dockEntries = dockLayout.map((dock) => ({
        id: dock.id,
        corner: dock.corner,
        visible: dock.id === 'visualizations' || dock.id === 'training' ? true : panelVisibility[dock.id],
        content: dockRenderers[dock.id],
        title: dock.title,
        defaultWidth: panelDefaultSizes[dock.id].width,
        defaultHeight: panelDefaultSizes[dock.id].height,
        resetToken: panelResetTokens[dock.id],
    }));

    return (
        <main style={shellStyle}>
            <section style={stageStyle}>
                <RouteTopBar
                    routeLabel={routeHudPreset.label}
                    showExplorer={showExplorer}
                    showExplorerButton={false}
                    nodeBrowserFiles={allFiles}
                    selectedNodeFile={selectedFile}
                    onSelectNodeFile={handleSelectFile}
                    onResetSectors={handleResetAllSectors}
                />

                <SafeNotebookGameplaySurface
                    selectedFile={selectedFile}
                    selectedContent={selectedContent}
                    fullBleed
                    routeMode={routeHudPreset.mode}
                    allIndexedFiles={allFiles}
                    contentByFile={cellLookup}
                    indexedFileCount={manifest?.file_count ?? 0}
                    notebookCellCount={notebookCellCount}
                    selectedDomain={selectedDomain}
                    rarityLabel={selectedRarity.label}
                    rarityAccent={selectedRarity.accent}
                    playerLevel={playerLevel}
                    totalXp={totalXp}
                    comboStreak={comboStreak}
                    lastXpGain={lastXpGain}
                    questProgress={questProgress}
                    questToast={questToast}
                    nextDomainTarget={nextDomainTarget}
                    taskbarOwnsObjectiveAndQuestWindows
                    taskbarOwnsIntelAndPlayerWindows
                    taskbarOwnsNodeOpsWindow
                    onToggleIntelNodeWindow={() => togglePanel('intelNode')}
                    onToggleObjectiveNodeWindow={() => togglePanel('objectiveNode')}
                    onTogglePlayerNodeWindow={() => togglePanel('playerNode')}
                    onToggleNodeOpsWindow={() => togglePanel('nodeOps')}
                    onAnalyticsTelemetryChange={setAnalyticsTelemetry}
                />

                <ResizableDockGrid
                    entries={dockEntries}
                    onPanelClose={(panelId) => {
                        closePanel(panelId as Parameters<typeof closePanel>[0]);
                    }}
                />

                <RouteHudControlStrip
                    routeLabel={routeHudPreset.label}
                    showExplorer={showExplorer}
                    showMenu={showMenu}
                    showOperations={showOperations}
                    showStatus={showStatus}
                    showVisualizations={showVisualizations}
                    showTraining={showTraining}
                    showIntelNode={showIntelNode}
                    showObjectiveNode={showObjectiveNode}
                    showPlayerNode={showPlayerNode}
                    showQuestLog={showQuestLog}
                    showNodeOps={showNodeOps}
                    pinnedPanels={pinnedPanels}
                    onToggleExplorer={() => togglePanel('explorer')}
                    onToggleMenu={() => togglePanel('menu')}
                    onToggleOperations={() => togglePanel('operations')}
                    onToggleStatus={() => togglePanel('status')}
                    onToggleVisualizations={() => togglePanel('visualizations')}
                    onToggleTraining={() => togglePanel('training')}
                    onToggleIntelNode={() => togglePanel('intelNode')}
                    onToggleObjectiveNode={() => togglePanel('objectiveNode')}
                    onTogglePlayerNode={() => togglePanel('playerNode')}
                    onToggleQuestLog={() => togglePanel('questLog')}
                    onToggleNodeOps={() => togglePanel('nodeOps')}
                    onTogglePanelPin={togglePanelPin}
                    onResetPanel={(panel) => resetPanelLayout(panel as NotebookHudPanelId)}
                    onFocusViewport={closeForViewport}
                    onResetHud={handleResetHud}
                    onCloseAllPanels={closeAllWindows}
                    onReopenPanels={reopenAllWindows}
                />
            </section>
        </main>
    );
}
