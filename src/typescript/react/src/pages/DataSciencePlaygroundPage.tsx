import { RouteDeckTransition, RouteDockGrid, RouteFilePickerOverlay, RouteHudControlStrip, RouteTopBar } from '@banana/ui';
import { useCallback, useEffect, useMemo, useState, type CSSProperties } from 'react';
import { useLocation } from 'react-router-dom';

import { MainMenuPanel } from '../components/notebook-client/MainMenuPanel';
import { ModeRegistryPanel } from '../components/notebook-client/ModeRegistryPanel';
import { NotebookExplorerPanel } from '../components/notebook-client/NotebookExplorerPanel';
import { NotebookGameplaySurface } from '../components/notebook-client/NotebookGameplaySurface';
import { NotebookHealthPanel } from '../components/notebook-client/NotebookHealthPanel';
import { NotebookOperationsPanel } from '../components/notebook-client/NotebookOperationsPanel';
import { RouteModePanel } from '../components/notebook-client/RouteModePanel';
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

export function DataSciencePlaygroundPage() {
    const location = useLocation();
    const { loading, manifest, manifestError, manifestSource, notebookError, notebookSource, cellLookup, notebookCellCount } = useNotebookClient();
    const [query, setQuery] = useState('');
    const [selectedFile, setSelectedFile] = useState<string>('');
    const [recentFiles, setRecentFiles] = useState<string[]>([]);
    const showExplorer = useNotebookLayoutStore((state) => state.showExplorer);
    const showMenu = useNotebookLayoutStore((state) => state.showMenu);
    const showStatus = useNotebookLayoutStore((state) => state.showStatus);
    const showOperations = useNotebookLayoutStore((state) => state.showOperations);
    const explorerDropupOpen = useNotebookLayoutStore((state) => state.explorerDropupOpen);
    const setExplorerDropupOpen = useNotebookLayoutStore((state) => state.setExplorerDropupOpen);
    const applyHudPreset = useNotebookLayoutStore((state) => state.applyHudPreset);
    const focusHudPanel = useNotebookLayoutStore((state) => state.focusHudPanel);
    const resetHudPanels = useNotebookLayoutStore((state) => state.resetHudPanels);
    const activeDock = useNotebookLayoutStore((state) => state.activeDock);
    const [modeDeckAnimating, setModeDeckAnimating] = useState(false);
    const [prefersReducedMotion, setPrefersReducedMotion] = useState(false);
    const [transitionHistory, setTransitionHistory] = useState<Array<{ pathname: string; mode: NotebookRouteMode; time: string; }>>([]);
    const [capturedFiles, setCapturedFiles] = useState<string[]>([]);
    const [totalXp, setTotalXp] = useState(0);
    const [comboStreak, setComboStreak] = useState(0);
    const [comboDomain, setComboDomain] = useState<SectorDomain | ''>('');
    const [lastSelectionForXp, setLastSelectionForXp] = useState('');
    const [lastXpGain, setLastXpGain] = useState(0);
    const [questToast, setQuestToast] = useState('');
    const [announcedQuestIds, setAnnouncedQuestIds] = useState<string[]>([]);

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
        setExplorerDropupOpen(false);
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
        setExplorerDropupOpen(false);
        resetHudPanels();
        focusHudPanel(null);
    }, [focusHudPanel, resetHudPanels, setExplorerDropupOpen]);

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
        applyHudPreset({
            explorer: routeHudPreset.explorer,
            menu: routeHudPreset.menu,
            status: routeHudPreset.status,
            operations: routeHudPreset.operations,
        });
    }, [applyHudPreset, routeHudPreset]);

    useEffect(() => {
        if (!selectedFile) {
            return;
        }

        if (activeDock === 'intel') {
            focusHudPanel('status');
            return;
        }
        if (activeDock === 'objectives') {
            focusHudPanel('operations');
            return;
        }
        if (activeDock === 'player') {
            focusHudPanel('menu');
        }
    }, [activeDock, focusHudPanel, selectedFile]);

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
        const timestamp = new Date().toLocaleTimeString();
        setTransitionHistory((previous) => {
            const nextEntry = {
                pathname: location.pathname,
                mode: routeHudPreset.mode,
                time: timestamp,
            };

            const deduped = previous.filter((entry) => !(entry.pathname === nextEntry.pathname && entry.mode === nextEntry.mode));
            return [nextEntry, ...deduped].slice(0, 3);
        });
    }, [location.pathname, routeHudPreset.mode]);

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

    const dockState = {
        explorer: showExplorer,
        menu: showMenu,
        status: showStatus,
        operations: showOperations,
    } as const;

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
                />
            </RouteDeckTransition>
        ),
    } as const;

    const dockLayout: Array<{ id: keyof typeof dockRenderers; corner: 'top-left' | 'top-right' | 'bottom-left' | 'bottom-right'; }> = [
        { id: 'explorer', corner: 'top-left' },
        { id: 'menu', corner: 'top-right' },
        { id: 'status', corner: 'bottom-left' },
        { id: 'operations', corner: 'bottom-right' },
    ];
    const dockEntries = dockLayout.map((dock) => ({
        id: dock.id,
        corner: dock.corner,
        visible: dockState[dock.id],
        content: dockRenderers[dock.id],
    }));

    return (
        <main style={shellStyle}>
            <section style={stageStyle}>
                {import.meta.env.DEV ? (
                    <ModeRegistryPanel
                        pathname={location.pathname}
                        mode={routeHudPreset.mode}
                        label={routeHudPreset.label}
                        explorer={showExplorer}
                        menu={showMenu}
                        status={showStatus}
                        operations={showOperations}
                        prefersReducedMotion={prefersReducedMotion}
                        transitions={transitionHistory}
                    />
                ) : null}

                <RouteTopBar
                    routeLabel={routeHudPreset.label}
                    showExplorer={showExplorer}
                    onToggleExplorer={() => {
                        focusHudPanel('explorer');
                        setExplorerDropupOpen(!explorerDropupOpen);
                    }}
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
                />

                <RouteDockGrid entries={dockEntries} surface="overlay" />

                <RouteHudControlStrip
                    routeLabel={routeHudPreset.label}
                    showMenu={showMenu}
                    showOperations={showOperations}
                    showStatus={showStatus}
                    onToggleMenu={() => focusHudPanel(showMenu ? null : 'menu')}
                    onToggleOperations={() => focusHudPanel(showOperations ? null : 'operations')}
                    onToggleStatus={() => focusHudPanel(showStatus ? null : 'status')}
                    onFocusViewport={() => {
                        focusHudPanel(null);
                    }}
                    onResetHud={resetHudPanels}
                />

                {explorerDropupOpen && showExplorer && (
                    <RouteFilePickerOverlay
                        files={allFiles}
                        recentFiles={recentFiles}
                        selected={selectedFile}
                        onSelect={handleSelectFile}
                        onClose={() => setExplorerDropupOpen(false)}
                    />
                )}
            </section>
        </main>
    );
}
