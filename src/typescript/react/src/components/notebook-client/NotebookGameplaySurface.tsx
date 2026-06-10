import type { CSSProperties } from 'react';

import { Pill, SurfaceCard } from './SurfacePrimitives';

type NotebookGameplaySurfaceProps = {
    readonly selectedFile: string;
    readonly selectedContent: string;
    readonly fullBleed?: boolean;
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

// Scanline overlay for the fullBleed stage — gives a subtle CRT feel
const scanlineUri = `url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='2' height='4'%3E%3Crect width='2' height='2' fill='rgba(0,0,0,0.08)'/%3E%3C/svg%3E")`;

export function NotebookGameplaySurface({ selectedFile, selectedContent, fullBleed = false }: NotebookGameplaySurfaceProps) {
    const lineCount = selectedContent.split('\n').length;

    if (fullBleed) {
        return (
            <div style={{
                position: 'absolute',
                inset: 0,
                borderRadius: 18,
                border: '1px solid rgba(45, 212, 191, 0.22)',
                background: `${scanlineUri}, radial-gradient(ellipse at 12% 8%, rgba(20, 184, 166, 0.18), transparent 38%), radial-gradient(ellipse at 88% 92%, rgba(14, 165, 233, 0.12), transparent 40%), linear-gradient(160deg, #020c18 0%, #071525 55%, #050f1e 100%)`,
                overflow: 'hidden',
                zIndex: 1,
            }}>
                {/* Scrollable code region — inset so content doesn't hide under docks */}
                <div style={{
                    position: 'absolute',
                    inset: 0,
                    overflow: 'auto',
                    padding: '44px max(16px, min(460px, 36vw)) 120px max(16px, min(460px, 36vw))',
                    fontFamily: '"IBM Plex Mono", SFMono-Regular, Menlo, Consolas, monospace',
                    fontSize: 13,
                    lineHeight: 1.65,
                    color: '#b2f5ea',
                    whiteSpace: 'pre-wrap',
                    overflowWrap: 'anywhere',
                    wordBreak: 'break-word',
                    boxSizing: 'border-box',
                }}>
                    {selectedContent}
                </div>
                {/* Sector label — top center */}
                <div style={{
                    position: 'absolute',
                    top: 14,
                    left: '50%',
                    transform: 'translateX(-50%)',
                    zIndex: 3,
                    fontSize: 11,
                    color: 'rgba(103, 232, 249, 0.7)',
                    textTransform: 'uppercase',
                    letterSpacing: '0.12em',
                    pointerEvents: 'none',
                    whiteSpace: 'nowrap',
                }}>
                    {selectedFile || 'No sector selected'} &nbsp;·&nbsp; {lineCount} lines
                </div>
                {/* Corner vignette borders */}
                <div style={{ position: 'absolute', inset: 0, borderRadius: 18, boxShadow: 'inset 0 0 60px rgba(2, 6, 23, 0.55)', pointerEvents: 'none', zIndex: 2 }} />
            </div>
        );
    }

    return (
        <SurfaceCard
            title="Mission Viewport"
            description="Source-driven runtime feed for the current sector."
        >
            <div style={{ display: 'flex', flexWrap: 'wrap', gap: 8 }}>
                <Pill color="#5eead4" borderColor="rgba(45, 212, 191, 0.4)">Sector: {selectedFile || 'none'}</Pill>
                <Pill color="#a5b4fc" borderColor="rgba(148, 163, 184, 0.35)">Lines: {lineCount}</Pill>
                <Pill color="#86efac" borderColor="rgba(34, 197, 94, 0.35)">Renderer: notebook</Pill>
            </div>
            <div style={{ ...codeSurfaceStyle, marginTop: 10 }}>
                {selectedContent}
            </div>
        </SurfaceCard>
    );
}
