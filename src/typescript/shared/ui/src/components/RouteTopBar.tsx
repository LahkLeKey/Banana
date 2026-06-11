import type { ReactNode } from 'react';

type RouteTopBarProps = {
    readonly routeLabel: string;
    readonly showExplorer: boolean;
    readonly onToggleExplorer: () => void;
    readonly onResetSectors: () => void;
    readonly children?: ReactNode;
};

export function RouteTopBar({
    routeLabel,
    showExplorer,
    onToggleExplorer,
    onResetSectors,
    children,
}: RouteTopBarProps) {
    return (
        <div style={{
            position: 'absolute',
            top: 0,
            left: 0,
            right: 0,
            height: 40,
            zIndex: 6,
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'space-between',
            padding: '0 12px',
            gap: 10,
            background: 'linear-gradient(180deg, rgba(2, 8, 20, 0.88) 0%, rgba(2, 10, 24, 0.72) 100%)',
            borderBottom: '1px solid rgba(45, 212, 191, 0.14)',
            backdropFilter: 'blur(8px)',
            pointerEvents: 'auto',
        }}>
            {/* Left: route label */}
            <div style={{
                color: '#93c5fd',
                fontSize: 10,
                fontWeight: 700,
                letterSpacing: '0.08em',
                textTransform: 'uppercase',
                whiteSpace: 'nowrap',
                opacity: 0.7,
            }}>
                {routeLabel}
            </div>

            {/* Center: Explorer + sector reset */}
            <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
                <button
                    type="button"
                    onClick={onToggleExplorer}
                    style={{
                        display: 'flex',
                        alignItems: 'center',
                        gap: 6,
                        borderRadius: 8,
                        border: showExplorer
                            ? '1px solid rgba(45, 212, 191, 0.55)'
                            : '1px solid rgba(148, 163, 184, 0.22)',
                        background: showExplorer
                            ? 'rgba(13, 148, 136, 0.22)'
                            : 'rgba(8, 13, 28, 0.35)',
                        color: showExplorer ? '#5eead4' : '#94a3b8',
                        padding: '4px 11px',
                        cursor: 'pointer',
                        fontSize: 11,
                        fontWeight: 700,
                        letterSpacing: '0.04em',
                        transition: 'background 0.15s, border-color 0.15s, color 0.15s',
                        whiteSpace: 'nowrap',
                    }}
                >
                    <span style={{ fontSize: 13, lineHeight: 1 }}>📂</span>
                    File Explorer
                </button>

                <button
                    type="button"
                    onClick={onResetSectors}
                    title="Reset all sectors"
                    aria-label="Reset all sectors"
                    style={{
                        borderRadius: 8,
                        border: '1px solid rgba(248, 113, 113, 0.32)',
                        background: 'rgba(127, 29, 29, 0.2)',
                        color: '#fecaca',
                        width: 30,
                        height: 28,
                        display: 'inline-flex',
                        alignItems: 'center',
                        justifyContent: 'center',
                        cursor: 'pointer',
                        fontSize: 14,
                        lineHeight: 1,
                    }}
                >
                    ⌂
                </button>
            </div>

            {/* Right: optional slot */}
            <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
                {children}
            </div>
        </div>
    );
}
