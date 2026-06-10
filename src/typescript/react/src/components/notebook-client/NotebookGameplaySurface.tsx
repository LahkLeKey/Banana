import type { CSSProperties } from 'react';

import { Pill, SurfaceCard } from './SurfacePrimitives';

type NotebookGameplaySurfaceProps = {
    readonly selectedFile: string;
    readonly selectedContent: string;
};

const codeSurfaceStyle: CSSProperties = {
    width: '100%',
    minHeight: 'min(440px, 56dvh)',
    maxHeight: '62dvh',
    borderRadius: 12,
    border: '1px solid rgba(45, 212, 191, 0.35)',
    background: 'radial-gradient(circle at top left, rgba(20, 184, 166, 0.14), rgba(2, 6, 23, 0.95) 30%)',
    fontFamily: '"IBM Plex Mono", SFMono-Regular, Menlo, Consolas, monospace',
    fontSize: 13,
    lineHeight: 1.55,
    color: '#d1fae5',
    overflow: 'auto',
    whiteSpace: 'pre-wrap',
    overflowWrap: 'anywhere',
    wordBreak: 'break-word',
    padding: 14,
    position: 'relative',
};

const hudOverlayStyle: CSSProperties = {
    position: 'absolute',
    top: 10,
    right: 10,
    width: 'min(260px, calc(100% - 20px))',
    borderRadius: 10,
    border: '1px solid rgba(94, 234, 212, 0.35)',
    background: 'linear-gradient(160deg, rgba(8, 25, 38, 0.88), rgba(2, 10, 19, 0.92))',
    backdropFilter: 'blur(6px)',
    padding: 10,
    zIndex: 2,
};

export function NotebookGameplaySurface({ selectedFile, selectedContent }: NotebookGameplaySurfaceProps) {
    const lineCount = selectedContent.split("\n").length;
    const hasSelection = selectedFile.trim().length > 0;

    return (
        <SurfaceCard
            title="Mission Viewport"
            description="Source-driven runtime feed for the current sector. Treat this as the playable client viewport backed by notebook cells."
        >
            <div style={{ display: 'flex', flexWrap: 'wrap', gap: 8 }}>
                <Pill color="#5eead4" borderColor="rgba(45, 212, 191, 0.4)">
                    Sector: {selectedFile || 'none'}
                </Pill>
                <Pill color="#a5b4fc" borderColor="rgba(148, 163, 184, 0.35)">
                    Lines: {lineCount}
                </Pill>
                <Pill color="#86efac" borderColor="rgba(34, 197, 94, 0.35)">
                    Renderer: notebook
                </Pill>
            </div>
            <div style={{ ...codeSurfaceStyle, marginTop: 10 }}>
                <aside style={hudOverlayStyle}>
                    <div style={{ fontSize: 11, color: '#67e8f9', letterSpacing: '0.06em', textTransform: 'uppercase' }}>
                        Overlay HUD
                    </div>
                    <div style={{ marginTop: 6, display: 'grid', gridTemplateColumns: 'repeat(2, minmax(0, 1fr))', gap: 6 }}>
                        <div style={{ borderRadius: 8, border: '1px solid rgba(148, 163, 184, 0.25)', padding: 6, background: 'rgba(15, 23, 42, 0.6)' }}>
                            <div style={{ fontSize: 10, color: '#93c5fd', textTransform: 'uppercase' }}>Lock</div>
                            <div style={{ fontSize: 12, marginTop: 2 }}>{hasSelection ? 'Live' : 'Idle'}</div>
                        </div>
                        <div style={{ borderRadius: 8, border: '1px solid rgba(148, 163, 184, 0.25)', padding: 6, background: 'rgba(15, 23, 42, 0.6)' }}>
                            <div style={{ fontSize: 10, color: '#93c5fd', textTransform: 'uppercase' }}>Lines</div>
                            <div style={{ fontSize: 12, marginTop: 2 }}>{lineCount}</div>
                        </div>
                    </div>
                    <div style={{ marginTop: 8, fontSize: 11, color: '#cbd5e1', overflowWrap: 'anywhere' }}>
                        {selectedFile || 'Awaiting sector selection'}
                    </div>
                </aside>
                {selectedContent}
            </div>
        </SurfaceCard>
    );
}
