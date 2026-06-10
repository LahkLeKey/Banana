import { useCallback, useEffect, useMemo, useState, type CSSProperties } from 'react';

import { MainMenuPanel } from '../components/notebook-client/MainMenuPanel';
import { NotebookExplorerPanel } from '../components/notebook-client/NotebookExplorerPanel';
import { NotebookGameplaySurface } from '../components/notebook-client/NotebookGameplaySurface';
import { NotebookOperationsPanel } from '../components/notebook-client/NotebookOperationsPanel';
import { HudActionBar, HudCornerDock, HudFilePicker } from '../components/notebook-client/HudPrimitives';
import { useNotebookClient } from '../lib/notebook-client/useNotebookClient';

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
    height: '100%',
    overflow: 'hidden',
    padding: 16,
};

const dockPanelStyle: CSSProperties = {
    borderRadius: 18,
    border: '1px solid rgba(45, 212, 191, 0.28)',
    background: 'linear-gradient(170deg, rgba(3, 10, 19, 0.9), rgba(1, 7, 14, 0.94))',
    boxShadow: '0 22px 72px rgba(2, 6, 23, 0.48)',
    overflow: 'hidden',
    display: 'flex',
    flexDirection: 'column',
    minHeight: 0,
};

const dockInnerStyle: CSSProperties = {
    flex: '1 1 auto',
    minHeight: 0,
    overflow: 'auto',
};

export function DataSciencePlaygroundPage() {
    const { loading, manifest, manifestError, manifestSource, notebookError, notebookSource, cellLookup, notebookCellCount } = useNotebookClient();
    const [query, setQuery] = useState('');
    const [selectedFile, setSelectedFile] = useState<string>('');
    const [recentFiles, setRecentFiles] = useState<string[]>([]);
    const [showExplorer, setShowExplorer] = useState(true);
    const [showMenu, setShowMenu] = useState(true);
    const [showDeck, setShowDeck] = useState(true);
    const [explorerDropupOpen, setExplorerDropupOpen] = useState(false);

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
        if (!selectedFile || !filteredFiles.includes(selectedFile)) {
            handleSelectFile(filteredFiles[0]);
        }
    }, [filteredFiles, selectedFile, handleSelectFile]);

    const selectedContent = selectedFile ? (cellLookup.get(selectedFile) ?? 'No code cell available for this file.') : 'Select a file from the notebook index.';
    const manifestLabel = manifest ? `Files indexed: ${manifest.file_count}` : 'Manifest pending';
    const sourceRootLabel = manifest ? `Source root: ${manifest.source_root}` : '';
    const maxLinesLabel = manifest ? `Max lines/file: ${manifest.max_lines_per_file}` : 'Run workflow to publish metadata';
    const sourceLabel = manifestSource.length > 0 ? `Manifest source: ${manifestSource}` : (notebookSource.length > 0 ? `Notebook source: ${notebookSource}` : '');
    const notebookAvailabilityLabel = manifest ? 'Artifacts loaded and indexed.' : (loading ? 'Loading notebook artifacts...' : 'Artifacts unavailable. Run scaffold workflow.');

    return (
        <main style={shellStyle}>
            <section style={stageStyle}>
                <NotebookGameplaySurface selectedFile={selectedFile} selectedContent={selectedContent} fullBleed />

                {showExplorer ? (
                    <HudCornerDock corner="top-left">
                        <div style={dockPanelStyle}>
                            <div style={dockInnerStyle}>
                                <NotebookExplorerPanel
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
                            </div>
                        </div>
                    </HudCornerDock>
                ) : null}

                {showMenu ? (
                    <HudCornerDock corner="top-right">
                        <div style={dockPanelStyle}>
                            <div style={dockInnerStyle}>
                                <MainMenuPanel
                                    indexedFileCount={manifest?.file_count ?? 0}
                                    notebookCellCount={notebookCellCount}
                                    manifestAvailable={Boolean(manifest)}
                                />
                            </div>
                        </div>
                    </HudCornerDock>
                ) : null}

                {showDeck ? (
                    <HudCornerDock corner="bottom-right">
                        <div style={dockPanelStyle}>
                            <div style={dockInnerStyle}>
                                <NotebookOperationsPanel
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
                        </div>
                    </HudCornerDock>
                ) : null}

                <HudActionBar>
                    {/* Explorer — left anchor toggle + dropup */}
                    <button
                        type="button"
                        onClick={() => { setShowExplorer(true); setExplorerDropupOpen(o => !o); }}
                        style={{ borderRadius: 999, border: `1px solid ${explorerDropupOpen ? 'rgba(45, 212, 191, 0.7)' : 'rgba(45, 212, 191, 0.35)'}`, background: explorerDropupOpen ? 'rgba(13, 148, 136, 0.32)' : showExplorer ? 'rgba(13, 148, 136, 0.18)' : 'rgba(8, 13, 28, 0.55)', color: '#e2e8f0', padding: '8px 14px', cursor: 'pointer', fontSize: 12, fontWeight: 700 }}
                    >
                        {explorerDropupOpen ? '▼ Explorer' : '▲ Explorer'}
                    </button>
                    {/* Toggle dock visibility */}
                    <button
                        type="button"
                        onClick={() => setShowExplorer(o => !o)}
                        style={{ borderRadius: 999, border: '1px solid rgba(45, 212, 191, 0.22)', background: 'rgba(8, 13, 28, 0.45)', color: showExplorer ? '#5eead4' : '#475569', padding: '8px 10px', cursor: 'pointer', fontSize: 11, fontWeight: 700 }}
                        title={showExplorer ? 'Hide explorer dock' : 'Show explorer dock'}
                    >{showExplorer ? '◧' : '□'}</button>

                    <div style={{ width: 1, background: 'rgba(45, 212, 191, 0.2)', alignSelf: 'stretch', margin: '0 2px' }} />

                    {/* Command Menu toggle */}
                    <button
                        type="button"
                        onClick={() => setShowMenu(o => !o)}
                        style={{ borderRadius: 999, border: '1px solid rgba(45, 212, 191, 0.35)', background: showMenu ? 'rgba(13, 148, 136, 0.18)' : 'rgba(8, 13, 28, 0.55)', color: '#e2e8f0', padding: '8px 14px', cursor: 'pointer', fontSize: 12, fontWeight: 700 }}
                    >Uplink</button>

                    <div style={{ width: 1, background: 'rgba(45, 212, 191, 0.2)', alignSelf: 'stretch', margin: '0 2px' }} />

                    {/* Operations toggle */}
                    <button
                        type="button"
                        onClick={() => setShowDeck(o => !o)}
                        style={{ borderRadius: 999, border: '1px solid rgba(45, 212, 191, 0.35)', background: showDeck ? 'rgba(13, 148, 136, 0.18)' : 'rgba(8, 13, 28, 0.55)', color: '#e2e8f0', padding: '8px 14px', cursor: 'pointer', fontSize: 12, fontWeight: 700 }}
                    >Ops Deck</button>

                    <div style={{ width: 1, background: 'rgba(45, 212, 191, 0.2)', alignSelf: 'stretch', margin: '0 2px' }} />

                    <button
                        type="button"
                        onClick={() => { setShowExplorer(false); setShowMenu(false); setShowDeck(false); setExplorerDropupOpen(false); }}
                        style={{ borderRadius: 999, border: '1px solid rgba(148, 163, 184, 0.25)', background: 'rgba(8, 13, 28, 0.55)', color: '#94a3b8', padding: '8px 12px', cursor: 'pointer', fontSize: 11, fontWeight: 700 }}
                    >Focus</button>
                    <button
                        type="button"
                        onClick={() => { setShowExplorer(true); setShowMenu(true); setShowDeck(true); }}
                        style={{ borderRadius: 999, border: '1px solid rgba(148, 163, 184, 0.25)', background: 'rgba(8, 13, 28, 0.55)', color: '#94a3b8', padding: '8px 12px', cursor: 'pointer', fontSize: 11, fontWeight: 700 }}
                    >Reset</button>
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
