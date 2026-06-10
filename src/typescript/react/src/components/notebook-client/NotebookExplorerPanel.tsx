import { useState } from 'react';
import { RouteSubActionBar, RouteSubActionLink } from '@banana/ui';

import { toCampaignMissionCard, toMissionOverlayContract } from '../../domain/notebook/explorer-domain';

type NotebookExplorerPanelProps = {
    readonly files: string[];
    readonly query: string;
    readonly selectedFile: string;
    readonly onChangeQuery: (query: string) => void;
    readonly onSelectFile: (file: string) => void;
    readonly fileCountLabel: string;
    readonly maxLinesLabel: string;
    readonly sourceRootLabel: string;
    readonly notebookManifestHref: string;
    readonly notebookPayloadHref: string;
    readonly notebookAvailabilityLabel: string;
};

const rarityStyle = {
    common: {
        border: 'rgba(148, 163, 184, 0.35)',
        aura: 'rgba(15, 23, 42, 0.3)',
        label: 'Common',
        text: '#cbd5e1',
    },
    rare: {
        border: 'rgba(56, 189, 248, 0.55)',
        aura: 'rgba(8, 47, 73, 0.38)',
        label: 'Rare',
        text: '#bae6fd',
    },
    epic: {
        border: 'rgba(244, 114, 182, 0.62)',
        aura: 'rgba(131, 24, 67, 0.34)',
        label: 'Epic',
        text: '#fbcfe8',
    },
} as const;

export function NotebookExplorerPanel(props: NotebookExplorerPanelProps) {
    const {
        files,
        query,
        selectedFile,
        onChangeQuery,
        onSelectFile,
        fileCountLabel,
        notebookManifestHref,
        notebookPayloadHref,
    } = props;
    const [activeTab, setActiveTab] = useState<'campaign' | 'index'>('campaign');
    const [activeMissionOverlay, setActiveMissionOverlay] = useState<string | null>(null);

    const missionFiles = files.slice(0, 24);
    const campaignCards = missionFiles.map((file) => toCampaignMissionCard(file, selectedFile));
    const activeOverlayCard = campaignCards.find((card) => card.file === activeMissionOverlay) ?? null;

    return (
        <aside style={{ alignSelf: 'start', minWidth: 0, display: 'flex', flexDirection: 'column', minHeight: 0 }}>
            {/* Sub-action bar: tabs + archive quick-links + file count meta */}
            <RouteSubActionBar
                tabs={[
                    { id: 'campaign', label: 'Campaign' },
                    { id: 'index', label: 'Dev Index' },
                ]}
                activeTab={activeTab}
                onTabChange={(id) => {
                    setActiveTab(id as 'campaign' | 'index');
                    setActiveMissionOverlay(null);
                }}
                actions={
                    <>
                        <RouteSubActionLink href={notebookPayloadHref}>Archive</RouteSubActionLink>
                        <RouteSubActionLink href={notebookManifestHref}>Index</RouteSubActionLink>
                    </>
                }
                meta={fileCountLabel}
            />

            {/* Search — sits below the sub-bar, no card wrapper */}
            <div style={{ padding: '8px 10px', borderBottom: '1px solid rgba(45, 212, 191, 0.1)', flexShrink: 0 }}>
                <input
                    type="text"
                    value={query}
                    onChange={(event) => onChangeQuery(event.target.value)}
                    placeholder="Search missions…"
                    style={{
                        width: '100%',
                        boxSizing: 'border-box',
                        borderRadius: 8,
                        border: '1px solid rgba(45, 212, 191, 0.28)',
                        background: 'rgba(4, 12, 22, 0.88)',
                        color: '#e2e8f0',
                        padding: '7px 10px',
                        fontSize: 12,
                        fontFamily: '"IBM Plex Mono", monospace',
                        outline: 'none',
                    }}
                />
            </div>

            {/* Relative container so the overlay floats above the list — NOT inside it */}
            <div style={{ position: 'relative', flex: 1, minHeight: 0, display: 'flex', flexDirection: 'column' }}>
                {activeOverlayCard ? (() => {
                    const overlay = toMissionOverlayContract(activeOverlayCard);
                    const tier = rarityStyle[activeOverlayCard.rarity];
                    return (
                        <div style={{
                            position: 'absolute',
                            inset: 0,
                            zIndex: 8,
                            borderRadius: 10,
                            border: `1px solid ${tier.border}`,
                            background: 'linear-gradient(180deg, rgba(4, 14, 28, 0.98), rgba(3, 10, 22, 0.99))',
                            boxShadow: '0 8px 32px rgba(2, 6, 23, 0.7)',
                            display: 'flex',
                            flexDirection: 'column',
                            padding: 14,
                            gap: 10,
                            overflow: 'auto',
                        }}>
                            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', gap: 8 }}>
                                <span style={{ fontSize: 12, textTransform: 'uppercase', letterSpacing: '0.07em', color: tier.text, fontWeight: 700 }}>
                                    {activeOverlayCard.title}
                                    <span style={{ marginLeft: 6, opacity: 0.55, fontWeight: 400 }}>· {tier.label}</span>
                                </span>
                                <button
                                    type="button"
                                    onClick={() => setActiveMissionOverlay(null)}
                                    style={{ border: '1px solid rgba(148, 163, 184, 0.3)', borderRadius: 999, background: 'rgba(15, 23, 42, 0.7)', color: '#cbd5e1', fontSize: 10, padding: '3px 8px', cursor: 'pointer', fontWeight: 700, textTransform: 'uppercase', flexShrink: 0 }}
                                >
                                    ✕
                                </button>
                            </div>
                            <div style={{ fontSize: 13, color: '#e2e8f0', lineHeight: 1.5 }}>{overlay.missionBrief}</div>
                            <div style={{ display: 'flex', justifyContent: 'space-between', fontSize: 11, color: '#94a3b8', flexWrap: 'wrap', gap: 6 }}>
                                <span>{overlay.objectiveLabel}</span>
                                <span style={{ color: tier.text }}>{overlay.threatLabel}</span>
                            </div>
                            <button
                                type="button"
                                onClick={() => { onSelectFile(activeOverlayCard.file); setActiveMissionOverlay(null); }}
                                style={{ border: `1px solid ${tier.border}`, borderRadius: 8, background: 'rgba(8, 47, 73, 0.9)', color: '#e0f2fe', fontSize: 11, textTransform: 'uppercase', letterSpacing: '0.05em', padding: '7px 12px', cursor: 'pointer', fontWeight: 700, alignSelf: 'flex-start' }}
                            >
                                Deploy
                            </button>
                        </div>
                    );
                })() : null}

                {activeTab === 'campaign' ? (
                    <div style={{ flex: 1, overflow: 'auto' }}>
                        {campaignCards.map((sector) => {
                            const file = sector.file;
                            const tier = rarityStyle[sector.rarity];
                            return (
                                <div
                                    key={file}
                                    style={{
                                        display: 'flex',
                                        gap: 8,
                                        alignItems: 'center',
                                        minWidth: 0,
                                        padding: '9px 11px',
                                        borderBottom: '1px solid rgba(148, 163, 184, 0.08)',
                                        background: sector.isSelected
                                            ? `linear-gradient(90deg, ${tier.aura}, rgba(8, 47, 73, 0.2))`
                                            : 'transparent',
                                        boxShadow: sector.isSelected ? `inset 0 0 0 1px ${tier.border}` : 'none',
                                    }}
                                >
                                    <button
                                        type="button"
                                        onClick={() => onSelectFile(file)}
                                        style={{ border: 'none', background: 'transparent', color: '#e2e8f0', cursor: 'pointer', textAlign: 'left', padding: 0, minWidth: 0, flex: 1, display: 'grid', gap: 3 }}
                                    >
                                        <div style={{ display: 'flex', justifyContent: 'space-between', gap: 8, alignItems: 'center', minWidth: 0 }}>
                                            <span style={{ fontSize: 13, fontWeight: 700, color: '#e6fffb', overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}>
                                                {sector.title}
                                            </span>
                                            <span style={{ flexShrink: 0, borderRadius: 999, border: `1px solid ${tier.border}`, padding: '2px 6px', fontSize: 10, textTransform: 'uppercase', color: tier.text, background: 'rgba(2, 8, 18, 0.6)' }}>
                                                {sector.domain} · {tier.label}
                                            </span>
                                        </div>
                                        <div style={{ fontSize: 11, color: '#93c5fd', overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}>
                                            {sector.lane}
                                        </div>
                                    </button>
                                    <button
                                        type="button"
                                        aria-label={`Drill up ${sector.title}`}
                                        onClick={() => setActiveMissionOverlay((prev) => prev === file ? null : file)}
                                        style={{ flexShrink: 0, borderRadius: 6, border: `1px solid ${tier.border}`, padding: '4px 8px', fontSize: 11, textTransform: 'uppercase', color: tier.text, background: activeMissionOverlay === file ? 'rgba(8, 47, 73, 0.85)' : 'transparent', cursor: 'pointer', fontWeight: 700 }}
                                    >
                                        ↑
                                    </button>
                                </div>
                            );
                        })}
                        {missionFiles.length === 0 ? (
                            <div style={{ padding: '12px 11px', color: '#64748b', fontSize: 12 }}>No mission sectors available.</div>
                        ) : null}
                    </div>
                ) : (
                    <div style={{ flex: 1, overflow: 'auto' }}>
                        {files.map((file) => {
                            const sector = toCampaignMissionCard(file, selectedFile);
                            return (
                                <button
                                    key={file}
                                    type="button"
                                    onClick={() => onSelectFile(file)}
                                    style={{ display: 'grid', width: '100%', textAlign: 'left', padding: '8px 11px', border: 'none', borderBottom: '1px solid rgba(148, 163, 184, 0.08)', background: file === selectedFile ? 'linear-gradient(90deg, rgba(30, 41, 59, 0.7), rgba(8, 47, 73, 0.25))' : 'transparent', color: '#e2e8f0', cursor: 'pointer', gap: 3 }}
                                >
                                    <div style={{ display: 'flex', justifyContent: 'space-between', gap: 8, minWidth: 0 }}>
                                        <span style={{ fontSize: 12, fontWeight: 700, color: '#dbeafe', overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}>{sector.title}</span>
                                        <span style={{ fontSize: 10, color: '#93c5fd', textTransform: 'uppercase', flexShrink: 0 }}>{sector.domain}</span>
                                    </div>
                                    <div style={{ fontSize: 10, color: '#475569', fontFamily: '"IBM Plex Mono", monospace', overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}>
                                        {file}
                                    </div>
                                </button>
                            );
                        })}
                        {files.length === 0 ? (
                            <div style={{ padding: '12px 11px', color: '#64748b', fontSize: 12 }}>No sectors match.</div>
                        ) : null}
                    </div>
                )}
            </div>
        </aside>
    );
}
