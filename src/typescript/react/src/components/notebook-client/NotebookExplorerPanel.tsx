import type { CSSProperties } from 'react';

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

                <div style={{ marginTop: 12, maxHeight: '52dvh', overflow: 'auto', border: '1px solid rgba(45, 212, 191, 0.2)', borderRadius: 10 }}>
                    {files.map((file) => (
                        <button
                            key={file}
                            type="button"
                            onClick={() => onSelectFile(file)}
                            style={{
                                display: 'block',
                                width: '100%',
                                textAlign: 'left',
                                padding: '10px 11px',
                                border: 'none',
                                borderBottom: '1px solid rgba(148, 163, 184, 0.1)',
                                background: file === selectedFile ? 'rgba(20, 184, 166, 0.24)' : 'transparent',
                                color: '#e2e8f0',
                                cursor: 'pointer',
                                fontSize: 12,
                                fontFamily: '"IBM Plex Mono", Consolas, monospace',
                                whiteSpace: 'normal',
                                overflowWrap: 'anywhere',
                                wordBreak: 'break-word',
                            }}
                        >
                            {file}
                        </button>
                    ))}
                    {files.length === 0 ? (
                        <div style={{ padding: '10px 11px', color: '#cbd5e1', fontSize: 13 }}>No sectors match this query.</div>
                    ) : null}
                </div>
            </SurfaceCard>
        </aside>
    );
}
