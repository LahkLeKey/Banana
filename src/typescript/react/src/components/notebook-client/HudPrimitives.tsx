import { useState, type CSSProperties, type ReactNode } from 'react';

export type HudIconKind = 'economy' | 'logistics' | 'relay' | 'intel' | 'split';

const iconStyle: CSSProperties = {
    width: 14,
    height: 14,
    viewBox: '0 0 14 14',
    fill: 'none',
    stroke: 'currentColor',
    strokeWidth: 1.4,
    strokeLinecap: 'round',
    strokeLinejoin: 'round',
    'aria-hidden': true,
    focusable: false,
};

export function HudIcon({ kind }: { kind: HudIconKind; }) {
    if (kind === 'economy') {
        return (
            <svg {...iconStyle}>
                <circle cx="7" cy="7" r="4.5" />
                <path d="M7 4.1v5.8" />
                <path d="M5.4 5.3c0-.8.7-1.4 1.6-1.4s1.6.6 1.6 1.4-.7 1.4-1.6 1.4-1.6.6-1.6 1.4.7 1.4 1.6 1.4 1.6-.6 1.6-1.4" />
            </svg>
        );
    }

    if (kind === 'logistics') {
        return (
            <svg {...iconStyle}>
                <path d="M2.8 4.4h5.4" />
                <path d="M7.2 2.4l2 2-2 2" />
                <path d="M11.2 9.6H5.8" />
                <path d="M6.8 7.6l-2 2 2 2" />
            </svg>
        );
    }

    if (kind === 'relay') {
        return (
            <svg {...iconStyle}>
                <path d="M7 10.8V7" />
                <path d="M4.5 5.3c1.6-1.6 3.4-1.6 5 0" />
                <path d="M3 3.7c2.3-2.3 5.7-2.3 8 0" />
                <circle cx="7" cy="11.1" r="0.8" fill="currentColor" stroke="none" />
            </svg>
        );
    }

    if (kind === 'intel') {
        return (
            <svg {...iconStyle}>
                <path d="M2.3 7c1.2-2 2.8-3 4.7-3s3.5 1 4.7 3c-1.2 2-2.8 3-4.7 3s-3.5-1-4.7-3Z" />
                <circle cx="7" cy="7" r="1.4" />
            </svg>
        );
    }

    return (
        <svg {...iconStyle}>
            <path d="M4 2.8v8.4" />
            <path d="M10 2.8v8.4" />
            <path d="M4 7h6" />
        </svg>
    );
}

export function HudStatusChip(
    { label, value, color, borderColor }:
        { label: string; value: string; color: string; borderColor: string; }) {
    return (
        <span style={{
            display: 'inline-flex',
            alignItems: 'center',
            gap: 6,
            borderRadius: 999,
            border: `1px solid ${borderColor}`,
            background: 'rgba(8, 13, 28, 0.56)',
            color,
            padding: '5px 8px',
            fontFamily: '"JetBrains Mono", "Cascadia Mono", monospace',
            fontSize: 10,
            letterSpacing: '0.04em',
            textTransform: 'uppercase',
            whiteSpace: 'nowrap',
        }}>
            <span style={{ opacity: 0.78 }}>{label}</span>
            <span style={{ fontWeight: 700 }}>{value}</span>
        </span>
    );
}

export function HudRailButton(
    { title, active, color, activeBorderColor, activeBackground, children, onClick }:
        {
            title: string;
            active: boolean;
            color: string;
            activeBorderColor: string;
            activeBackground: string;
            children: ReactNode;
            onClick: () => void;
        }) {
    return (
        <button
            type="button"
            title={title}
            onClick={onClick}
            style={{
                borderRadius: 10,
                border: active ? `1px solid ${activeBorderColor}` : '1px solid rgba(148, 163, 184, 0.4)',
                background: active ? activeBackground : 'rgba(8, 13, 28, 0.55)',
                color,
                padding: '8px 4px',
                cursor: 'pointer',
                fontSize: 10,
                fontWeight: 700,
                lineHeight: 1.1,
                fontFamily: '"JetBrains Mono", "Cascadia Mono", monospace',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'center',
                gap: 4,
            }}
        >
            {children}
        </button>
    );
}

export function HudCornerDock(
    { corner, children }:
        { corner: 'top-left' | 'top-right' | 'bottom-left' | 'bottom-right'; children: ReactNode; }) {

    const isBottom = corner === 'bottom-left' || corner === 'bottom-right';
    const isRight = corner === 'top-right' || corner === 'bottom-right';

    // Width: at most 44% of viewport so two docks on the same axis never overlap
    // Height: at most 46dvh so top/bottom docks each stay within their half
    const base: CSSProperties = {
        position: 'absolute',
        zIndex: 4,
        pointerEvents: 'auto',
        width: 'min(400px, calc(44vw - 16px))',
        maxHeight: 'calc(46dvh - 60px)',
        display: 'flex',
        flexDirection: 'column',
        // Bottom docks: anchor at bottom edge, content grows upward
        // Top docks: anchor at top edge, content grows downward
        justifyContent: isBottom ? 'flex-end' : 'flex-start',
        overflow: 'hidden',
    };

    const hPos: CSSProperties = isRight ? { right: 16 } : { left: 16 };
    const vPos: CSSProperties = isBottom ? { bottom: 76 } : { top: 16 };

    return <div style={{ ...base, ...hPos, ...vPos }}>{children}</div>;
}

export function HudFilePicker(
    { files, recentFiles, selected, onSelect, onClose }:
        {
            files: string[];
            recentFiles: string[];
            selected: string;
            onSelect: (file: string) => void;
            onClose: () => void;
        }) {
    const [q, setQ] = useState('');
    const normalizedQ = q.trim().toLowerCase();
    const matchedFiles = normalizedQ
        ? files.filter(f => f.toLowerCase().includes(normalizedQ))
        : files;

    const pinnedSection = recentFiles.length > 0 && !normalizedQ;

    const rowStyle = (isSelected: boolean): CSSProperties => ({
        padding: '6px 10px',
        borderRadius: 8,
        cursor: 'pointer',
        fontSize: 12,
        fontFamily: '"IBM Plex Mono", monospace',
        color: isSelected ? '#67e8f9' : '#cbd5e1',
        background: isSelected ? 'rgba(45, 212, 191, 0.14)' : 'transparent',
        border: isSelected ? '1px solid rgba(45, 212, 191, 0.3)' : '1px solid transparent',
        whiteSpace: 'nowrap',
        overflow: 'hidden',
        textOverflow: 'ellipsis',
        display: 'block',
        width: '100%',
        textAlign: 'left',
    });

    return (
        <div style={{
            position: 'absolute',
            bottom: 72,
            left: '50%',
            transform: 'translateX(-50%)',
            width: 'min(620px, calc(100vw - 32px))',
            maxHeight: 'calc(54dvh - 80px)',
            zIndex: 20,
            borderRadius: 18,
            border: '1px solid rgba(45, 212, 191, 0.38)',
            background: 'linear-gradient(170deg, rgba(2, 10, 22, 0.97), rgba(1, 7, 16, 0.98))',
            boxShadow: '0 -16px 48px rgba(2, 6, 23, 0.7)',
            display: 'flex',
            flexDirection: 'column',
            overflow: 'hidden',
        }}>
            {/* Header */}
            <div style={{ display: 'flex', alignItems: 'center', gap: 8, padding: '10px 12px 8px', borderBottom: '1px solid rgba(45, 212, 191, 0.18)', flexShrink: 0 }}>
                <span style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.1em', color: '#67e8f9', flexShrink: 0 }}>Sector Select</span>
                <input
                    type="text"
                    value={q}
                    onChange={e => setQ(e.target.value)}
                    placeholder="Filter sectors..."
                    autoFocus
                    style={{
                        flex: 1,
                        borderRadius: 8,
                        border: '1px solid rgba(45, 212, 191, 0.3)',
                        background: 'rgba(6, 15, 26, 0.9)',
                        color: '#e2e8f0',
                        padding: '5px 9px',
                        fontSize: 12,
                        fontFamily: '"IBM Plex Mono", monospace',
                        outline: 'none',
                    }}
                />
                <button type="button" onClick={onClose} style={{ borderRadius: 999, border: '1px solid rgba(148, 163, 184, 0.3)', background: 'rgba(8, 13, 28, 0.5)', color: '#94a3b8', padding: '4px 9px', cursor: 'pointer', fontSize: 11, flexShrink: 0 }}>✕</button>
            </div>
            {/* File list */}
            <div style={{ flex: 1, overflow: 'auto', padding: '6px 8px' }}>
                {pinnedSection && (
                    <>
                        <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#5eead4', padding: '4px 4px 6px', opacity: 0.8 }}>Recent</div>
                        {recentFiles.map(f => (
                            <button key={`r-${f}`} type="button" style={rowStyle(f === selected)} onClick={() => onSelect(f)}>
                                ◈ {f}
                            </button>
                        ))}
                        <div style={{ margin: '6px 0', borderTop: '1px solid rgba(45, 212, 191, 0.12)' }} />
                        <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#94a3b8', padding: '2px 4px 6px', opacity: 0.7 }}>All Sectors ({files.length})</div>
                    </>
                )}
                {matchedFiles.map(f => (
                    <button key={f} type="button" style={rowStyle(f === selected)} onClick={() => onSelect(f)}>
                        {f}
                    </button>
                ))}
                {matchedFiles.length === 0 && (
                    <div style={{ padding: '16px 10px', textAlign: 'center', color: '#64748b', fontSize: 12 }}>No sectors match</div>
                )}
            </div>
        </div>
    );
}

export function HudActionBar({ children }: { children: ReactNode; }) {
    return (
        <div style={{
            position: 'absolute',
            left: '50%',
            bottom: 16,
            transform: 'translateX(-50%)',
            zIndex: 5,
            pointerEvents: 'auto',
            width: 'min(760px, calc(100vw - 32px))',
            borderRadius: 999,
            border: '1px solid rgba(45, 212, 191, 0.34)',
            background: 'linear-gradient(180deg, rgba(2, 10, 20, 0.92), rgba(3, 20, 33, 0.95))',
            boxShadow: '0 18px 44px rgba(2, 6, 23, 0.52)',
            padding: '10px 12px',
            display: 'flex',
            justifyContent: 'center',
            gap: 8,
            flexWrap: 'wrap',
        }}>
            {children}
        </div>
    );
}
