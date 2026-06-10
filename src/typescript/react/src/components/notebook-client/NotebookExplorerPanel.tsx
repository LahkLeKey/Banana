import { useState, type CSSProperties } from 'react';

import { ActionLink, SurfaceCard } from './SurfacePrimitives';

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

const inputStyle: CSSProperties = {
    width: '100%',
    borderRadius: 10,
    border: '1px solid rgba(45, 212, 191, 0.35)',
    background: 'rgba(6, 15, 26, 0.9)',
    color: '#e2e8f0',
    padding: '10px 11px',
};

function deriveSectorMeta(file: string): { title: string; domain: string; lane: string; code: string; } {
    const normalized = file.replace(/\\/g, '/');
    const parts = normalized.split('/').filter(Boolean);
    const lane = parts.length > 0 ? parts[parts.length - 1] ?? normalized : normalized;
    const laneLabel = lane
        .replace(/\.[^.]+$/, '')
        .replace(/[_-]+/g, ' ')
        .replace(/\b\w/g, (match) => match.toUpperCase());
    const title = lane
        .replace(/\.[^.]+$/, '')
        .replace(/[_-]+/g, ' ')
        .replace(/\b\w/g, (match) => match.toUpperCase());

    let domain = 'Other';
    if (normalized.includes('/ai/')) {
        domain = 'AI';
    } else if (normalized.includes('/physics/')) {
        domain = 'Physics';
    } else if (normalized.includes('/render/')) {
        domain = 'Render';
    } else if (normalized.includes('/world/')) {
        domain = 'World';
    } else if (normalized.includes('/runtime/')) {
        domain = 'Runtime';
    }

    return {
        title,
        domain,
        lane: laneLabel,
        code: `${domain.slice(0, 2).toUpperCase()}-${Math.abs(normalized.split('').reduce((acc, char) => acc + char.charCodeAt(0), 0)) % 1000}`,
    };
}

export function NotebookExplorerPanel(props: NotebookExplorerPanelProps) {
    const {
        files,
        query,
        selectedFile,
        onChangeQuery,
        onSelectFile,
        fileCountLabel,
        maxLinesLabel,
        sourceRootLabel,
        notebookManifestHref,
        notebookPayloadHref,
        notebookAvailabilityLabel,
    } = props;
    const [showDeveloperIndex, setShowDeveloperIndex] = useState(false);
    const missionFiles = files.slice(0, 24);

    return (
        <aside style={{ alignSelf: 'start', minWidth: 0 }}>
            <SurfaceCard title="World Codex" description="Curated source map and notebook artifact index for runtime sectors.">
                <p style={{ margin: 0, color: '#cbd5e1', lineHeight: 1.6 }}>
                    Rebuild mission dataset:
                </p>
                <pre style={{ margin: '8px 0 0', whiteSpace: 'pre-wrap', color: '#a5f3fc' }}>
                    bash scripts/scaffold-abi-notebook-workflow.sh
                </pre>

                <div style={{ marginTop: 6, fontSize: 13, color: '#a5b4fc' }}>{fileCountLabel}</div>
                <div style={{ marginTop: 4, fontSize: 13, color: '#a5b4fc', overflowWrap: 'anywhere' }}>{maxLinesLabel}</div>
                <div style={{ marginTop: 4, fontSize: 13, color: '#a5b4fc' }}>{sourceRootLabel}</div>

                <div style={{ marginTop: 12, border: '1px solid rgba(45, 212, 191, 0.2)', borderRadius: 10, padding: 10, background: 'rgba(2, 8, 18, 0.6)' }}>
                    <div style={{ fontSize: 12, letterSpacing: '0.06em', textTransform: 'uppercase', color: '#67e8f9', fontWeight: 700 }}>Notebook Artifacts</div>
                    <div style={{ marginTop: 6, fontSize: 13, color: '#cbd5e1' }}>{notebookAvailabilityLabel}</div>
                    <div style={{ display: 'flex', flexWrap: 'wrap', gap: 8, marginTop: 8 }}>
                        <ActionLink href={notebookPayloadHref}>native-c-catalog.ipynb</ActionLink>
                        <ActionLink href={notebookManifestHref}>catalog-index.json</ActionLink>
                    </div>
                </div>

                <input
                    type="text"
                    value={query}
                    onChange={(event) => onChangeQuery(event.target.value)}
                    placeholder="Search sectors (e.g. gameplay_service)"
                    style={inputStyle}
                />

                <div style={{ marginTop: 10, display: 'flex', gap: 8, flexWrap: 'wrap' }}>
                    <button
                        type="button"
                        onClick={() => setShowDeveloperIndex(false)}
                        style={{
                            borderRadius: 999,
                            border: '1px solid rgba(45, 212, 191, 0.35)',
                            background: !showDeveloperIndex ? 'rgba(13, 148, 136, 0.32)' : 'rgba(8, 13, 28, 0.5)',
                            color: '#ccfbf1',
                            fontSize: 11,
                            fontWeight: 700,
                            letterSpacing: '0.04em',
                            textTransform: 'uppercase',
                            padding: '6px 10px',
                            cursor: 'pointer',
                        }}
                    >
                        Campaign Missions
                    </button>
                    <button
                        type="button"
                        onClick={() => setShowDeveloperIndex(true)}
                        style={{
                            borderRadius: 999,
                            border: '1px solid rgba(148, 163, 184, 0.35)',
                            background: showDeveloperIndex ? 'rgba(30, 41, 59, 0.8)' : 'rgba(8, 13, 28, 0.5)',
                            color: '#cbd5e1',
                            fontSize: 11,
                            fontWeight: 700,
                            letterSpacing: '0.04em',
                            textTransform: 'uppercase',
                            padding: '6px 10px',
                            cursor: 'pointer',
                        }}
                    >
                        Developer Index
                    </button>
                </div>

                {!showDeveloperIndex ? (
                    <div style={{ marginTop: 12, maxHeight: '52dvh', overflow: 'auto', border: '1px solid rgba(45, 212, 191, 0.2)', borderRadius: 10 }}>
                        {missionFiles.map((file) => {
                            const sector = deriveSectorMeta(file);
                            return (
                                <button
                                    key={file}
                                    type="button"
                                    onClick={() => onSelectFile(file)}
                                    style={{
                                        display: 'grid',
                                        width: '100%',
                                        textAlign: 'left',
                                        padding: '10px 11px',
                                        border: 'none',
                                        borderBottom: '1px solid rgba(148, 163, 184, 0.1)',
                                        background: file === selectedFile ? 'linear-gradient(90deg, rgba(20, 184, 166, 0.28), rgba(8, 47, 73, 0.28))' : 'transparent',
                                        color: '#e2e8f0',
                                        cursor: 'pointer',
                                        gap: 5,
                                    }}
                                >
                                    <div style={{ display: 'flex', justifyContent: 'space-between', gap: 8, alignItems: 'center', minWidth: 0 }}>
                                        <span style={{
                                            fontSize: 13,
                                            fontWeight: 700,
                                            color: '#e6fffb',
                                            overflow: 'hidden',
                                            textOverflow: 'ellipsis',
                                            whiteSpace: 'nowrap',
                                        }}>
                                            Secure {sector.title}
                                        </span>
                                        <span style={{
                                            flexShrink: 0,
                                            borderRadius: 999,
                                            border: '1px solid rgba(45, 212, 191, 0.35)',
                                            padding: '2px 7px',
                                            fontSize: 10,
                                            textTransform: 'uppercase',
                                            letterSpacing: '0.06em',
                                            color: '#67e8f9',
                                            background: 'rgba(2, 8, 18, 0.62)',
                                        }}>
                                            {sector.domain}
                                        </span>
                                    </div>
                                    <div style={{
                                        fontSize: 11,
                                        color: '#93c5fd',
                                        overflow: 'hidden',
                                        textOverflow: 'ellipsis',
                                        whiteSpace: 'nowrap',
                                    }}>
                                        Operation lane: {sector.lane}
                                    </div>
                                    <div style={{
                                        fontSize: 10,
                                        color: '#64748b',
                                        overflow: 'hidden',
                                        textOverflow: 'ellipsis',
                                        whiteSpace: 'nowrap',
                                        textTransform: 'uppercase',
                                    }}>
                                        Objective ID: {sector.code}
                                    </div>
                                </button>
                            );
                        })}
                        {missionFiles.length === 0 ? (
                            <div style={{ padding: '10px 11px', color: '#cbd5e1', fontSize: 13 }}>No mission sectors available.</div>
                        ) : null}
                    </div>
                ) : (
                    <div style={{ marginTop: 12, maxHeight: '52dvh', overflow: 'auto', border: '1px solid rgba(148, 163, 184, 0.24)', borderRadius: 10 }}>
                        {files.map((file) => {
                            const sector = deriveSectorMeta(file);
                            return (
                                <button
                                    key={file}
                                    type="button"
                                    onClick={() => onSelectFile(file)}
                                    style={{
                                        display: 'grid',
                                        width: '100%',
                                        textAlign: 'left',
                                        padding: '10px 11px',
                                        border: 'none',
                                        borderBottom: '1px solid rgba(148, 163, 184, 0.1)',
                                        background: file === selectedFile ? 'linear-gradient(90deg, rgba(30, 41, 59, 0.75), rgba(8, 47, 73, 0.3))' : 'transparent',
                                        color: '#e2e8f0',
                                        cursor: 'pointer',
                                        gap: 4,
                                    }}
                                >
                                    <div style={{ display: 'flex', justifyContent: 'space-between', gap: 8, alignItems: 'center', minWidth: 0 }}>
                                        <span style={{ fontSize: 12, fontWeight: 700, color: '#dbeafe' }}>{sector.title}</span>
                                        <span style={{ fontSize: 10, color: '#93c5fd', textTransform: 'uppercase' }}>{sector.domain}</span>
                                    </div>
                                    <div style={{ fontSize: 10, color: '#64748b', fontFamily: '"IBM Plex Mono", Consolas, monospace', overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}>
                                        {file}
                                    </div>
                                </button>
                            );
                        })}
                        {files.length === 0 ? (
                            <div style={{ padding: '10px 11px', color: '#cbd5e1', fontSize: 13 }}>No sectors match this query.</div>
                        ) : null}
                    </div>
                )}
            </SurfaceCard>
        </aside>
    );
}
