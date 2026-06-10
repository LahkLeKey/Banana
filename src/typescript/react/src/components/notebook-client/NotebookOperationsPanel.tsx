import { useMemo, useState } from 'react';

import { NotebookHealthPanel } from './NotebookHealthPanel';
import { HudIcon, HudRailButton, HudStatusChip } from './HudPrimitives';
import { TrainingOperationsPanel } from './TrainingOperationsPanel';
import { Pill, SurfaceCard } from './SurfacePrimitives';

type NotebookOperationsPanelProps = {
    selectedFile: string;
    selectedContent: string;
    indexedFileCount: number;
    notebookCellCount: number;
    loading: boolean;
    manifestError: string | null;
    notebookError: string | null;
    manifestSource: string;
    notebookSource: string;
    overlay?: boolean;
};

function deriveThreatIndex(selectedFile: string): string {
    if (!selectedFile) {
        return 'Low';
    }

    const normalized = selectedFile.toLowerCase();
    if (normalized.includes('/runtime/') || normalized.includes('/world/')) {
        return 'High';
    }

    if (normalized.includes('/render/') || normalized.includes('/physics/')) {
        return 'Medium';
    }

    return 'Low';
}

export function NotebookOperationsPanel(props: NotebookOperationsPanelProps) {
    const {
        selectedFile,
        selectedContent,
        indexedFileCount,
        notebookCellCount,
        loading,
        manifestError,
        notebookError,
        manifestSource,
        notebookSource,
        overlay = false,
    } = props;

    const threatIndex = useMemo(() => deriveThreatIndex(selectedFile), [selectedFile]);
    const selectedLineCount = useMemo(() => selectedContent.split('\n').length, [selectedContent]);
    const [drawerOpen, setDrawerOpen] = useState(true);
    const [overlayMode, setOverlayMode] = useState<'training' | 'diagnostics' | 'split'>('training');
    const overlayModeLabel = overlayMode === 'training' ? 'Logistics' : overlayMode === 'diagnostics' ? 'Diagnostics' : 'Split';
    const networkStatusLabel = loading ? 'SYNC' : 'LIVE';
    const deckStatusLabel = drawerOpen ? 'OPEN' : 'PARKED';

    const renderDrawerContent = () => {
        if (overlayMode === 'training') {
            return (
                <div style={{
                    borderRadius: 10,
                    border: '1px solid rgba(250, 204, 21, 0.25)',
                    background: 'rgba(36, 22, 5, 0.35)',
                    padding: 10,
                }}>
                    <div style={{ fontSize: 11, letterSpacing: '0.06em', textTransform: 'uppercase', color: '#fcd34d', marginBottom: 8 }}>
                        Training Overlay
                    </div>
                    <TrainingOperationsPanel
                        selectedFile={selectedFile}
                        selectedContent={selectedContent}
                        indexedFileCount={indexedFileCount}
                        embedded
                    />
                </div>
            );
        }

        if (overlayMode === 'diagnostics') {
            return (
                <div style={{
                    borderRadius: 10,
                    border: '1px solid rgba(45, 212, 191, 0.22)',
                    background: 'rgba(3, 14, 25, 0.35)',
                    padding: 10,
                }}>
                    <div style={{ fontSize: 11, letterSpacing: '0.06em', textTransform: 'uppercase', color: '#67e8f9', marginBottom: 8 }}>
                        Diagnostics Overlay
                    </div>
                    <NotebookHealthPanel
                        loading={loading}
                        manifestError={manifestError}
                        notebookError={notebookError}
                        manifestSource={manifestSource}
                        notebookSource={notebookSource}
                        embedded
                    />
                </div>
            );
        }

        return (
            <div style={{
                display: 'grid',
                gap: 8,
                gridTemplateColumns: 'repeat(auto-fit, minmax(180px, 1fr))',
            }}>
                <div style={{
                    borderRadius: 10,
                    border: '1px solid rgba(250, 204, 21, 0.25)',
                    background: 'rgba(36, 22, 5, 0.35)',
                    padding: 10,
                }}>
                    <div style={{ fontSize: 11, letterSpacing: '0.06em', textTransform: 'uppercase', color: '#fcd34d', marginBottom: 8 }}>
                        Training Overlay
                    </div>
                    <TrainingOperationsPanel
                        selectedFile={selectedFile}
                        selectedContent={selectedContent}
                        indexedFileCount={indexedFileCount}
                        embedded
                    />
                </div>

                <div style={{
                    borderRadius: 10,
                    border: '1px solid rgba(45, 212, 191, 0.22)',
                    background: 'rgba(3, 14, 25, 0.35)',
                    padding: 10,
                }}>
                    <div style={{ fontSize: 11, letterSpacing: '0.06em', textTransform: 'uppercase', color: '#67e8f9', marginBottom: 8 }}>
                        Diagnostics Overlay
                    </div>
                    <NotebookHealthPanel
                        loading={loading}
                        manifestError={manifestError}
                        notebookError={notebookError}
                        manifestSource={manifestSource}
                        notebookSource={notebookSource}
                        embedded
                    />
                </div>
            </div>
        );
    };

    if (overlay) {
        return (
            <div style={{
                position: 'absolute',
                top: 12,
                left: 12,
                right: 12,
                zIndex: 4,
                pointerEvents: 'none',
            }}>
                <div style={{ display: 'flex', alignItems: 'flex-start', gap: 10 }}>
                    <aside style={{
                        width: 72,
                        borderRadius: 16,
                        border: '1px solid rgba(45, 212, 191, 0.35)',
                        background: 'linear-gradient(180deg, rgba(3, 12, 24, 0.94), rgba(6, 20, 33, 0.9))',
                        boxShadow: '0 18px 32px rgba(2, 6, 23, 0.45)',
                        padding: 8,
                        pointerEvents: 'auto',
                    }}>
                        <div style={{ fontSize: 9, letterSpacing: '0.08em', textTransform: 'uppercase', color: '#67e8f9', textAlign: 'center', marginBottom: 6 }}>
                            HUD
                        </div>

                        <div style={{ display: 'grid', gap: 6 }}>
                            <HudRailButton
                                title="Economy overview"
                                active={overlayMode === 'training'}
                                color="#fef3c7"
                                activeBorderColor="rgba(251, 191, 36, 0.75)"
                                activeBackground="rgba(120, 53, 15, 0.38)"
                                onClick={() => {
                                    setOverlayMode('training');
                                    setDrawerOpen(true);
                                }}
                            >
                                <span style={{ display: 'inline-flex', color: '#fcd34d' }}><HudIcon kind="economy" /></span>
                            </HudRailButton>
                            <HudRailButton
                                title="Logistics operations"
                                active={overlayMode === 'training'}
                                color="#fef3c7"
                                activeBorderColor="rgba(251, 191, 36, 0.75)"
                                activeBackground="rgba(120, 53, 15, 0.38)"
                                onClick={() => {
                                    setOverlayMode('training');
                                    setDrawerOpen(true);
                                }}
                            >
                                <span style={{ display: 'inline-flex', color: '#fef3c7' }}><HudIcon kind="logistics" /></span>
                            </HudRailButton>
                            <HudRailButton
                                title="Relay diagnostics"
                                active={overlayMode === 'diagnostics'}
                                color="#cffafe"
                                activeBorderColor="rgba(34, 211, 238, 0.75)"
                                activeBackground="rgba(8, 47, 73, 0.46)"
                                onClick={() => {
                                    setOverlayMode('diagnostics');
                                    setDrawerOpen(true);
                                }}
                            >
                                <span style={{ display: 'inline-flex', color: '#67e8f9' }}><HudIcon kind="relay" /></span>
                            </HudRailButton>
                            <HudRailButton
                                title="Intel diagnostics"
                                active={overlayMode === 'diagnostics'}
                                color="#cffafe"
                                activeBorderColor="rgba(34, 211, 238, 0.75)"
                                activeBackground="rgba(8, 47, 73, 0.46)"
                                onClick={() => {
                                    setOverlayMode('diagnostics');
                                    setDrawerOpen(true);
                                }}
                            >
                                <span style={{ display: 'inline-flex', color: '#cffafe' }}><HudIcon kind="intel" /></span>
                            </HudRailButton>
                            <HudRailButton
                                title="Split logistics and diagnostics"
                                active={overlayMode === 'split'}
                                color="#99f6e4"
                                activeBorderColor="rgba(45, 212, 191, 0.75)"
                                activeBackground="rgba(15, 118, 110, 0.34)"
                                onClick={() => {
                                    setOverlayMode('split');
                                    setDrawerOpen(true);
                                }}
                            >
                                <span style={{ display: 'inline-flex', color: '#99f6e4' }}><HudIcon kind="split" /></span>
                            </HudRailButton>
                        </div>

                        <button
                            type="button"
                            title={drawerOpen ? 'Hide command deck drawer' : 'Open command deck drawer'}
                            onClick={() => setDrawerOpen((current) => !current)}
                            style={{
                                marginTop: 8,
                                width: '100%',
                                borderRadius: 10,
                                border: '1px solid rgba(45, 212, 191, 0.45)',
                                background: drawerOpen ? 'rgba(13, 148, 136, 0.24)' : 'rgba(8, 13, 28, 0.6)',
                                color: '#e2e8f0',
                                padding: '8px 4px',
                                cursor: 'pointer',
                                fontSize: 9,
                                fontWeight: 700,
                                letterSpacing: '0.05em',
                                textTransform: 'uppercase',
                            }}
                        >
                            {drawerOpen ? 'Hide Deck' : 'Open Deck'}
                        </button>
                    </aside>

                    <div style={{
                        width: 'min(470px, calc(100vw - 150px))',
                        borderRadius: 14,
                        border: '1px solid rgba(45, 212, 191, 0.36)',
                        background: 'linear-gradient(165deg, rgba(2, 10, 20, 0.96), rgba(3, 20, 33, 0.94))',
                        boxShadow: '0 20px 40px rgba(2, 6, 23, 0.48)',
                        padding: 8,
                        maxHeight: 'min(58dvh, 520px)',
                        overflowY: 'auto',
                        transform: drawerOpen ? 'translateX(0)' : 'translateX(-16px)',
                        opacity: drawerOpen ? 1 : 0,
                        pointerEvents: drawerOpen ? 'auto' : 'none',
                        transition: 'transform 220ms ease, opacity 180ms ease',
                    }}>
                        <div style={{
                            position: 'sticky',
                            top: 0,
                            zIndex: 2,
                            display: 'grid',
                            gap: 8,
                            marginBottom: 10,
                            paddingBottom: 8,
                            background: 'linear-gradient(180deg, rgba(2, 10, 20, 0.96), rgba(2, 10, 20, 0.7))',
                        }}>
                            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', gap: 8 }}>
                                <div style={{ fontSize: 11, letterSpacing: '0.06em', textTransform: 'uppercase', color: '#67e8f9' }}>
                                    {overlayModeLabel} Command Deck
                                </div>
                                <button
                                    type="button"
                                    title="Close command deck drawer"
                                    onClick={() => setDrawerOpen(false)}
                                    style={{
                                        borderRadius: 999,
                                        border: '1px solid rgba(148, 163, 184, 0.35)',
                                        background: 'rgba(8, 13, 28, 0.45)',
                                        color: '#e2e8f0',
                                        fontSize: 11,
                                        fontWeight: 700,
                                        padding: '5px 10px',
                                        cursor: 'pointer',
                                    }}
                                >
                                    Close
                                </button>
                            </div>

                            <div style={{ display: 'flex', flexWrap: 'wrap', gap: 6 }}>
                                <Pill color="#fcd34d" borderColor="rgba(251, 191, 36, 0.4)">Risk: {threatIndex}</Pill>
                                <Pill color="#93c5fd" borderColor="rgba(148, 163, 184, 0.35)">Provinces: {indexedFileCount}</Pill>
                                <Pill color="#86efac" borderColor="rgba(34, 197, 94, 0.35)">Intel: {selectedLineCount}</Pill>
                            </div>

                            <div style={{ display: 'flex', flexWrap: 'wrap', gap: 6 }}>
                                <HudStatusChip label="Mode" value={overlayModeLabel} color="#67e8f9" borderColor="rgba(45, 212, 191, 0.32)" />
                                <HudStatusChip label="Net" value={networkStatusLabel} color="#93c5fd" borderColor="rgba(148, 163, 184, 0.26)" />
                                <HudStatusChip label="Auth" value="Server" color="#fcd34d" borderColor="rgba(251, 191, 36, 0.28)" />
                                <HudStatusChip label="Deck" value={deckStatusLabel} color="#99f6e4" borderColor="rgba(45, 212, 191, 0.3)" />
                            </div>
                        </div>

                        {renderDrawerContent()}
                    </div>
                </div>
            </div>
        );
    }

    return (
        <SurfaceCard
            title="Command Deck"
            description="Central management panel for province posture, logistics orders, and relay diagnostics."
        >
            <div style={{ display: 'flex', flexWrap: 'wrap', gap: 8, marginBottom: 10 }}>
                <Pill color="#fcd34d" borderColor="rgba(251, 191, 36, 0.4)">Risk: {threatIndex}</Pill>
                <Pill color="#93c5fd" borderColor="rgba(148, 163, 184, 0.35)">Provinces: {indexedFileCount}</Pill>
                <Pill color="#93c5fd" borderColor="rgba(148, 163, 184, 0.35)">Cells: {notebookCellCount}</Pill>
                <Pill color="#86efac" borderColor="rgba(34, 197, 94, 0.35)">Intel Lines: {selectedLineCount}</Pill>
            </div>

            <div style={{ display: 'flex', flexWrap: 'wrap', gap: 8, alignItems: 'center' }}>
                <Pill color="#99f6e4" borderColor="rgba(45, 212, 191, 0.65)">Strategy Deck</Pill>
                <Pill color="#fcd34d" borderColor="rgba(251, 191, 36, 0.5)">Logistics Deck</Pill>
                <Pill color="#67e8f9" borderColor="rgba(45, 212, 191, 0.45)">Relay Deck</Pill>
                <button
                    type="button"
                    onClick={() => setDrawerOpen((current) => !current)}
                    style={{
                        marginLeft: 'auto',
                        borderRadius: 999,
                        border: '1px solid rgba(45, 212, 191, 0.45)',
                        background: drawerOpen ? 'rgba(13, 148, 136, 0.22)' : 'rgba(8, 13, 28, 0.5)',
                        color: '#e2e8f0',
                        padding: '6px 12px',
                        cursor: 'pointer',
                        fontSize: 12,
                        fontWeight: 700,
                        letterSpacing: '0.04em',
                    }}
                >
                    {drawerOpen ? 'Collapse Deck' : 'Open Deck'}
                </button>
            </div>

            <section style={{
                position: 'relative',
                marginTop: 12,
                borderRadius: 12,
                border: '1px solid rgba(45, 212, 191, 0.25)',
                background: 'linear-gradient(165deg, rgba(2, 8, 18, 0.82), rgba(3, 15, 27, 0.76))',
                minHeight: 320,
                padding: 12,
            }}>
                <div style={{ display: 'grid', gap: 10, gridTemplateColumns: 'repeat(auto-fit, minmax(180px, 1fr))' }}>
                    <div style={{ borderRadius: 10, border: '1px solid rgba(148, 163, 184, 0.25)', background: 'rgba(8, 13, 28, 0.62)', padding: 10 }}>
                        <div style={{ fontSize: 11, color: '#93c5fd', textTransform: 'uppercase' }}>Province Focus</div>
                        <div style={{ marginTop: 4, fontSize: 12, color: '#cbd5e1', overflowWrap: 'anywhere' }}>{selectedFile || 'No province selected'}</div>
                    </div>
                    <div style={{ borderRadius: 10, border: '1px solid rgba(148, 163, 184, 0.25)', background: 'rgba(8, 13, 28, 0.62)', padding: 10 }}>
                        <div style={{ fontSize: 11, color: '#93c5fd', textTransform: 'uppercase' }}>Deck Status</div>
                        <div style={{ marginTop: 4, fontWeight: 700 }}>{drawerOpen ? 'Deck Open' : 'Deck Parked'}</div>
                    </div>
                    <div style={{ borderRadius: 10, border: '1px solid rgba(148, 163, 184, 0.25)', background: 'rgba(8, 13, 28, 0.62)', padding: 10 }}>
                        <div style={{ fontSize: 11, color: '#93c5fd', textTransform: 'uppercase' }}>Network Load</div>
                        <div style={{ marginTop: 4, fontWeight: 700 }}>{loading ? 'Syncing' : 'Stable'}</div>
                    </div>
                </div>

                <div style={{ marginTop: 12 }}>
                    <div style={{
                        borderRadius: 10,
                        border: '1px solid rgba(148, 163, 184, 0.22)',
                        background: 'rgba(8, 13, 28, 0.4)',
                        padding: 10,
                        minHeight: 96,
                    }}>
                        <div style={{ fontSize: 11, textTransform: 'uppercase', color: '#93c5fd', letterSpacing: '0.06em' }}>
                            Deck Routing
                        </div>
                        <div style={{ marginTop: 8, display: 'flex', flexWrap: 'wrap', gap: 8 }}>
                            <button
                                type="button"
                                onClick={() => {
                                    setOverlayMode('training');
                                    setDrawerOpen(true);
                                }}
                                style={{
                                    borderRadius: 999,
                                    border: overlayMode === 'training' ? '1px solid rgba(251, 191, 36, 0.7)' : '1px solid rgba(148, 163, 184, 0.35)',
                                    background: overlayMode === 'training' ? 'rgba(120, 53, 15, 0.35)' : 'rgba(8, 13, 28, 0.45)',
                                    color: '#fef3c7',
                                    padding: '6px 11px',
                                    cursor: 'pointer',
                                    fontSize: 12,
                                    fontWeight: 700,
                                }}
                            >
                                Logistics
                            </button>
                            <button
                                type="button"
                                onClick={() => {
                                    setOverlayMode('diagnostics');
                                    setDrawerOpen(true);
                                }}
                                style={{
                                    borderRadius: 999,
                                    border: overlayMode === 'diagnostics' ? '1px solid rgba(34, 211, 238, 0.7)' : '1px solid rgba(148, 163, 184, 0.35)',
                                    background: overlayMode === 'diagnostics' ? 'rgba(8, 47, 73, 0.45)' : 'rgba(8, 13, 28, 0.45)',
                                    color: '#cffafe',
                                    padding: '6px 11px',
                                    cursor: 'pointer',
                                    fontSize: 12,
                                    fontWeight: 700,
                                }}
                            >
                                Relay
                            </button>
                            <button
                                type="button"
                                onClick={() => {
                                    setOverlayMode('split');
                                    setDrawerOpen(true);
                                }}
                                style={{
                                    borderRadius: 999,
                                    border: overlayMode === 'split' ? '1px solid rgba(45, 212, 191, 0.7)' : '1px solid rgba(148, 163, 184, 0.35)',
                                    background: overlayMode === 'split' ? 'rgba(15, 118, 110, 0.3)' : 'rgba(8, 13, 28, 0.45)',
                                    color: '#99f6e4',
                                    padding: '6px 11px',
                                    cursor: 'pointer',
                                    fontSize: 12,
                                    fontWeight: 700,
                                }}
                            >
                                Split View
                            </button>
                        </div>
                        <div style={{ marginTop: 8, color: '#cbd5e1', fontSize: 12 }}>
                            {drawerOpen ? `Deck active: ${overlayMode}` : 'Deck parked. Open deck to view panel content.'}
                        </div>
                    </div>
                </div>

                <div style={{
                    position: 'absolute',
                    right: 12,
                    top: 72,
                    bottom: 12,
                    width: 'min(540px, calc(100% - 24px))',
                    borderRadius: 12,
                    border: '1px solid rgba(45, 212, 191, 0.32)',
                    background: 'linear-gradient(165deg, rgba(2, 10, 20, 0.96), rgba(3, 20, 33, 0.94))',
                    boxShadow: '0 24px 44px rgba(2, 6, 23, 0.5)',
                    padding: 10,
                    overflowY: 'auto',
                    transform: drawerOpen ? 'translateX(0)' : 'translateX(calc(100% + 18px))',
                    opacity: drawerOpen ? 1 : 0,
                    pointerEvents: drawerOpen ? 'auto' : 'none',
                    transition: 'transform 220ms ease, opacity 180ms ease',
                }}>
                    <div style={{
                        position: 'sticky',
                        top: 0,
                        zIndex: 2,
                        display: 'flex',
                        justifyContent: 'space-between',
                        alignItems: 'center',
                        gap: 8,
                        marginBottom: 10,
                        paddingBottom: 8,
                        background: 'linear-gradient(180deg, rgba(2, 10, 20, 0.96), rgba(2, 10, 20, 0.7))',
                    }}>
                        <div style={{ fontSize: 11, letterSpacing: '0.06em', textTransform: 'uppercase', color: '#67e8f9' }}>
                            {overlayMode === 'split' ? 'Split Command Deck' : `${overlayMode} Command Deck`}
                        </div>
                        <button
                            type="button"
                            onClick={() => setDrawerOpen(false)}
                            style={{
                                borderRadius: 999,
                                border: '1px solid rgba(148, 163, 184, 0.35)',
                                background: 'rgba(8, 13, 28, 0.45)',
                                color: '#e2e8f0',
                                fontSize: 11,
                                fontWeight: 700,
                                padding: '5px 10px',
                                cursor: 'pointer',
                            }}
                        >
                            Hide
                        </button>
                    </div>

                    {renderDrawerContent()}
                </div>
            </section>
        </SurfaceCard>
    );
}
