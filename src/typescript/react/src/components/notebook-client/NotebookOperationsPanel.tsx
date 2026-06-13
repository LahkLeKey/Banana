import { RoutePill as Pill, RouteSubActionBar } from '@banana/ui';
import { useMemo, useState } from 'react';

import { NotebookHealthPanel } from './NotebookHealthPanel';
import { HudIcon, HudRailButton, HudStatusChip } from './HudPrimitives';
import { TrainingOperationsPanel } from './TrainingOperationsPanel';

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
    analyticsAvailable?: boolean;
    analyticsCohort?: string;
    k3h4Clusters?: number;
    k3h4Convergence?: string;
    abiCoveragePresent?: number;
    abiCoverageExpected?: number;
    abiCoveragePercent?: number;
    abiCoverageComplete?: boolean;
    abiCoverageMissing?: readonly string[];
    abiLayerLedger?: readonly {
        layer: 'learning' | 'reward' | 'link' | 'vector' | 'hypersphere';
        present: boolean;
        contractVersion: number;
        status: 'ok' | 'unsupported-version' | 'invalid-payload' | 'nonfinite-value' | 'crc-mismatch' | 'missing';
        payloadBytes: number;
        byteOrderTag: number;
        deterministicHash: number | string | null;
    }[];
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

function deriveProvinceLabel(selectedFile: string): string {
    if (!selectedFile) {
        return 'No province selected';
    }

    const lane = selectedFile.replace(/\\/g, '/').split('/').filter(Boolean).pop() ?? selectedFile;
    return lane
        .replace(/\.[^.]+$/, '')
        .replace(/[_-]+/g, ' ')
        .replace(/\b\w/g, (match) => match.toUpperCase());
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
        analyticsAvailable = true,
        analyticsCohort = 'default',
        k3h4Clusters = 0,
        k3h4Convergence = 'unknown',
        abiCoveragePresent = 0,
        abiCoverageExpected = 5,
        abiCoveragePercent = 0,
        abiCoverageComplete = false,
        abiCoverageMissing = [],
        abiLayerLedger = [],
    } = props;

    const threatIndex = useMemo(() => deriveThreatIndex(selectedFile), [selectedFile]);
    const provinceLabel = useMemo(() => deriveProvinceLabel(selectedFile), [selectedFile]);
    const selectedLineCount = useMemo(() => selectedContent.split('\n').length, [selectedContent]);
    const [drawerOpen, setDrawerOpen] = useState(true);
    const [overlayMode, setOverlayMode] = useState<'training' | 'diagnostics' | 'split'>('training');
    const overlayModeLabel = overlayMode === 'training' ? 'Logistics' : overlayMode === 'diagnostics' ? 'Diagnostics' : 'Split';
    const networkStatusLabel = loading ? 'SYNC' : 'LIVE';
    const analyticsStatusLabel = analyticsAvailable ? 'AVAILABLE' : 'UNAVAILABLE';
    const abiCoverageSummary = `${abiCoveragePresent}/${abiCoverageExpected}`;
    const abiCoverageLabel = `ABI ${abiCoverageSummary} · ${abiCoveragePercent}%`;
    const abiCoverageStateLabel = abiCoverageComplete ? 'COMPLETE' : 'PARTIAL';
    const abiCoverageMissingLabel = abiCoverageMissing.length > 0 ? abiCoverageMissing.join(', ') : 'none';
    const deckStatusLabel = drawerOpen ? 'OPEN' : 'PARKED';

    const renderAbiLedger = () => (
        <div style={{
            marginTop: 8,
            borderRadius: 10,
            border: '1px solid rgba(148, 163, 184, 0.3)',
            background: 'rgba(2, 10, 20, 0.45)',
            padding: 8,
            display: 'grid',
            gap: 6,
        }}>
            <div style={{ fontSize: 10, letterSpacing: '0.06em', textTransform: 'uppercase', color: '#cbd5e1' }}>
                ABI Layer Ledger
            </div>
            <div style={{ display: 'grid', gap: 4 }}>
                {abiLayerLedger.map((layer) => (
                    <div key={layer.layer} style={{
                        display: 'grid',
                        gridTemplateColumns: 'minmax(76px, 92px) 1fr',
                        gap: 8,
                        alignItems: 'center',
                        fontSize: 11,
                        color: '#e2e8f0',
                        borderRadius: 8,
                        border: `1px solid ${layer.present ? 'rgba(34, 197, 94, 0.28)' : 'rgba(148, 163, 184, 0.24)'}`,
                        background: layer.present ? 'rgba(20, 83, 45, 0.28)' : 'rgba(30, 41, 59, 0.35)',
                        padding: '6px 8px',
                    }}>
                        <span style={{ textTransform: 'uppercase', letterSpacing: '0.04em', color: layer.present ? '#86efac' : '#94a3b8' }}>
                            {layer.layer}
                        </span>
                        <span style={{ color: 'rgba(226, 232, 240, 0.9)' }}>
                            {layer.status} · v{layer.contractVersion} · {layer.payloadBytes}b · hash {layer.deterministicHash ?? 'n/a'}
                        </span>
                    </div>
                ))}
            </div>
        </div>
    );

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
                                <HudStatusChip label="K3H4" value={`${k3h4Clusters} clusters`} color="#a7f3d0" borderColor="rgba(16, 185, 129, 0.28)" />
                                <HudStatusChip label="Converge" value={k3h4Convergence} color="#bae6fd" borderColor="rgba(56, 189, 248, 0.3)" />
                                <HudStatusChip label="Analytics" value={analyticsStatusLabel} color={analyticsAvailable ? '#86efac' : '#fda4af'} borderColor={analyticsAvailable ? 'rgba(34, 197, 94, 0.28)' : 'rgba(244, 63, 94, 0.3)'} />
                                <HudStatusChip label="ABI" value={abiCoverageLabel} color={abiCoverageComplete ? '#86efac' : '#fcd34d'} borderColor={abiCoverageComplete ? 'rgba(34, 197, 94, 0.28)' : 'rgba(251, 191, 36, 0.3)'} />
                                <span title="ABI legend: green = all expected layers present; amber = one or more layers missing." style={{ display: 'inline-flex' }}>
                                    <HudStatusChip label="Legend" value="Green/Amber" color="#cbd5e1" borderColor="rgba(148, 163, 184, 0.28)" />
                                </span>
                                <HudStatusChip label="Deck" value={deckStatusLabel} color="#99f6e4" borderColor="rgba(45, 212, 191, 0.3)" />
                            </div>

                            <div style={{
                                display: 'grid',
                                gap: 4,
                                borderRadius: 10,
                                border: `1px solid ${abiCoverageComplete ? 'rgba(34, 197, 94, 0.3)' : 'rgba(251, 191, 36, 0.35)'}`,
                                background: abiCoverageComplete ? 'rgba(20, 83, 45, 0.32)' : 'rgba(120, 53, 15, 0.3)',
                                padding: '8px 10px',
                            }}>
                                <div style={{ fontSize: 10, letterSpacing: '0.06em', textTransform: 'uppercase', color: abiCoverageComplete ? '#86efac' : '#fcd34d' }}>
                                    ABI Coverage {abiCoverageStateLabel}
                                </div>
                                <div style={{ fontSize: 11, color: '#e2e8f0' }}>
                                    {abiCoverageLabel}
                                </div>
                                <div style={{ fontSize: 10, color: abiCoverageComplete ? 'rgba(187, 247, 208, 0.86)' : 'rgba(254, 240, 138, 0.88)' }}>
                                    Missing: {abiCoverageMissingLabel}
                                </div>
                            </div>

                            {renderAbiLedger()}
                        </div>

                        {renderDrawerContent()}
                    </div>
                </div>
            </div>
        );
    }

    return (
        <div style={{ display: 'flex', flexDirection: 'column', minHeight: 0 }}>
            {/* Sub-action bar: mode tabs replace card header + pill rows + routing section */}
            <RouteSubActionBar
                tabs={[
                    { id: 'training', label: 'Logistics' },
                    { id: 'diagnostics', label: 'Relay' },
                    { id: 'split', label: 'Split' },
                ]}
                activeTab={overlayMode}
                onTabChange={(id) => setOverlayMode(id as 'training' | 'diagnostics' | 'split')}
                meta={`${provinceLabel} · Risk ${threatIndex} · ${indexedFileCount} sectors · ${selectedLineCount} lines`}
            />

            {/* Status chips inline below bar — no wrapper card */}
            <div style={{ display: 'flex', gap: 6, padding: '6px 12px', borderBottom: '1px solid rgba(45, 212, 191, 0.08)', flexWrap: 'wrap', flexShrink: 0 }}>
                <Pill color="#fcd34d" borderColor="rgba(251, 191, 36, 0.4)">Risk: {threatIndex}</Pill>
                <Pill color="#93c5fd" borderColor="rgba(148, 163, 184, 0.35)">{indexedFileCount} sectors</Pill>
                <Pill color="#86efac" borderColor="rgba(34, 197, 94, 0.35)">{selectedLineCount} lines</Pill>
                <Pill color={loading ? '#fcd34d' : '#86efac'} borderColor={loading ? 'rgba(251, 191, 36, 0.3)' : 'rgba(34, 197, 94, 0.3)'}>{networkStatusLabel}</Pill>
                <Pill color={analyticsAvailable ? '#86efac' : '#fda4af'} borderColor={analyticsAvailable ? 'rgba(34, 197, 94, 0.28)' : 'rgba(244, 63, 94, 0.3)'}>{analyticsStatusLabel}</Pill>
                <Pill color="#a7f3d0" borderColor="rgba(16, 185, 129, 0.28)">K3H4: {k3h4Clusters}</Pill>
                <Pill color="#bae6fd" borderColor="rgba(56, 189, 248, 0.3)">Convergence: {k3h4Convergence}</Pill>
                <Pill color={abiCoverageComplete ? '#86efac' : '#fcd34d'} borderColor={abiCoverageComplete ? 'rgba(34, 197, 94, 0.28)' : 'rgba(251, 191, 36, 0.3)'}>{abiCoverageLabel}</Pill>
                <span title="ABI legend: green = all expected layers present; amber = one or more layers missing." style={{ display: 'inline-flex' }}>
                    <Pill color="#cbd5e1" borderColor="rgba(148, 163, 184, 0.35)">ABI Legend</Pill>
                </span>
                <Pill color="#e9d5ff" borderColor="rgba(168, 85, 247, 0.26)">Cohort: {analyticsCohort}</Pill>
            </div>

            {/* Mode content — flat, no nested card */}
            <div style={{ flex: 1, minHeight: 0, overflow: 'auto', padding: 10 }}>
                {renderDrawerContent()}
                {renderAbiLedger()}
            </div>
        </div>
    );
}
