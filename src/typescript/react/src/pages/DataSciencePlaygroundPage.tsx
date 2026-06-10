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
import { HudActionBar, HudActionButton, HudCornerDock, HudDockSurface, HudFilePicker } from '../components/notebook-client/HudPrimitives';
import {
    buildQuestProgress,
    calculateLevel,
    calculateSectorXp,
    deriveNextCoreDomainTarget,
    deriveSectorDomain,
    deriveSectorRarity,
    type SectorDomain,
} from '../lib/notebook-client/gamification';
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
    padding: 16,
};

const routeDeckTransitionStyle: CSSProperties = {
    transition: 'opacity 220ms ease, transform 220ms ease, filter 220ms ease',
};

function resolveDeckTransitionStyle(
    reducedMotion: boolean,
    animating: boolean,
    delayMs: number,
    inactiveOpacity: number,
    inactiveTransform: string,
    inactiveFilter: string,
): CSSProperties {
    if (reducedMotion) {
        return {
            transition: 'none',
            transitionDelay: '0ms',
            opacity: 1,
            transform: 'none',
            filter: 'none',
        };
    }

    return {
        ...routeDeckTransitionStyle,
        transitionDelay: animating ? `${delayMs}ms` : '0ms',
        opacity: animating ? inactiveOpacity : 1,
        transform: animating ? inactiveTransform : 'translateY(0) scale(1)',
        filter: animating ? inactiveFilter : 'none',
    };
}

export function DataSciencePlaygroundPage() {
    const location = useLocation();
    const { loading, manifest, manifestError, manifestSource, notebookError, notebookSource, cellLookup, notebookCellCount } = useNotebookClient();
    const [query, setQuery] = useState('');
    const [selectedFile, setSelectedFile] = useState<string>('');
    const [recentFiles, setRecentFiles] = useState<string[]>([]);
    const [showExplorer, setShowExplorer] = useState(true);
    const [showMenu, setShowMenu] = useState(true);
    const [showStatus, setShowStatus] = useState(true);
    const [showOperations, setShowOperations] = useState(true);
    const [explorerDropupOpen, setExplorerDropupOpen] = useState(false);
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
        setShowExplorer(routeHudPreset.explorer);
        setShowMenu(routeHudPreset.menu);
        setShowStatus(routeHudPreset.status);
        setShowOperations(routeHudPreset.operations);
        setExplorerDropupOpen(false);
    }, [routeHudPreset]);

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

    const selectedContent = selectedFile ? (cellLookup.get(selectedFile) ?? 'No code cell available for this file.') : 'Select a file from the notebook index.';
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
            <div style={resolveDeckTransitionStyle(prefersReducedMotion, modeDeckAnimating, 0, 0.74, 'translateY(8px) scale(0.988)', 'saturate(1.05)')}>
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
            </div>
        ),
        status: (
            <div style={resolveDeckTransitionStyle(prefersReducedMotion, modeDeckAnimating, 40, 0.86, 'translateY(7px) scale(1)', 'saturate(1.03)')}>
                <SafeNotebookHealthPanel
                    loading={loading}
                    manifestError={manifestError}
                    notebookError={notebookError}
                    manifestSource={manifestSource}
                    notebookSource={notebookSource}
                />
            </div>
        ),
        operations: (
            <div style={resolveDeckTransitionStyle(prefersReducedMotion, modeDeckAnimating, 80, 0.8, 'translateY(9px) scale(0.992)', 'saturate(1.07)')}>
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
            </div>
        ),
    } as const;

    const dockLayout: Array<{ id: keyof typeof dockRenderers; corner: 'top-left' | 'top-right' | 'bottom-left' | 'bottom-right'; }> = [
        { id: 'explorer', corner: 'top-left' },
        { id: 'menu', corner: 'top-right' },
        { id: 'status', corner: 'bottom-left' },
        { id: 'operations', corner: 'bottom-right' },
    ];

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

                <SafeNotebookGameplaySurface
                    selectedFile={selectedFile}
                    selectedContent={selectedContent}
                    fullBleed
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

                {dockLayout.map((dock) => (dockState[dock.id] ? (
                    <HudCornerDock key={dock.id} corner={dock.corner}>
                        <HudDockSurface>{dockRenderers[dock.id]}</HudDockSurface>
                    </HudCornerDock>
                ) : null))}

                <HudActionBar>
                    <HudActionButton
                        label="Explorer"
                        onClick={() => { setShowExplorer(true); setExplorerDropupOpen((open) => !open); }}
                        active={showExplorer || explorerDropupOpen}
                    />

                    <HudActionButton
                        label="Command Menu"
                        onClick={() => setShowMenu((open) => !open)}
                        active={showMenu}
                    />

                    <HudActionButton
                        label="Operations"
                        onClick={() => setShowOperations((open) => !open)}
                        active={showOperations}
                    />

                    <HudActionButton
                        label="Status"
                        onClick={() => setShowStatus((open) => !open)}
                        active={showStatus}
                    />

                    <span style={{
                        borderRadius: 999,
                        border: '1px solid rgba(148, 163, 184, 0.3)',
                        background: 'rgba(8, 13, 28, 0.5)',
                        color: '#93c5fd',
                        padding: '8px 13px',
                        fontSize: 11,
                        fontWeight: 700,
                        letterSpacing: '0.04em',
                        textTransform: 'uppercase',
                        whiteSpace: 'nowrap',
                    }}>
                        {routeHudPreset.label}
                    </span>

                    <div style={{ width: 1, background: 'rgba(45, 212, 191, 0.2)', alignSelf: 'stretch', margin: '0 2px' }} />

                    <HudActionButton
                        label="Focus Viewport"
                        onClick={() => {
                            setShowExplorer(false);
                            setShowMenu(false);
                            setShowStatus(false);
                            setShowOperations(false);
                            setExplorerDropupOpen(false);
                        }}
                        muted
                    />
                    <HudActionButton
                        label="Reset HUD"
                        onClick={() => {
                            setShowExplorer(true);
                            setShowMenu(true);
                            setShowStatus(true);
                            setShowOperations(true);
                        }}
                        muted
                    />
                </HudActionBar>

                {explorerDropupOpen && (
                    <HudFilePicker
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
