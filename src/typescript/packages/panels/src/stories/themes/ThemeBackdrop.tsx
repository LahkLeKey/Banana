import * as React from 'react';

const desktopStyle: React.CSSProperties = {
    minHeight: '82vh',
    position: 'relative',
    overflow: 'hidden',
    borderRadius: 12,
    border: '1px solid var(--banana-panel-chrome-border, rgba(56, 189, 248, 0.22))',
    boxShadow: '0 18px 60px rgba(2, 6, 23, 0.6)',
    background:
        'var(--banana-panel-desktop-gradient, radial-gradient(circle at 20% 0%, rgba(14, 165, 233, 0.24), transparent 38%), radial-gradient(circle at 90% 10%, rgba(16, 185, 129, 0.15), transparent 34%), repeating-linear-gradient(0deg, rgba(148,163,184,0.05) 0 1px, transparent 1px 36px), repeating-linear-gradient(90deg, rgba(148,163,184,0.05) 0 1px, transparent 1px 36px), linear-gradient(180deg, #071122 0%, #0b1b31 100%))',
    fontFamily: 'var(--banana-panel-font-family, "Segoe UI", system-ui, sans-serif)',
};

const taskbarStyle: React.CSSProperties = {
    position: 'absolute',
    left: 0,
    right: 0,
    bottom: 0,
    height: 38,
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    padding: '0 10px',
    borderTop: '1px solid rgba(148,163,184,0.2)',
    background: 'var(--banana-panel-taskbar-background, rgba(2, 6, 23, 0.62))',
    backdropFilter: 'blur(8px)',
};

const desktopHeaderStyle: React.CSSProperties = {
    position: 'absolute',
    left: 0,
    right: 0,
    top: 0,
    height: 40,
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    padding: '0 12px',
    borderBottom: '1px solid rgba(148,163,184,0.2)',
    background: 'var(--banana-panel-header-background, rgba(2, 6, 23, 0.5))',
    backdropFilter: 'blur(8px)',
};

export function ThemeBackdrop({
    children,
    label,
    immersive = false,
}: {
    children: React.ReactNode;
    label: string;
    immersive?: boolean;
}) {
    const mergedDesktopStyle: React.CSSProperties = {
        ...desktopStyle,
        ...(immersive
            ? {
                minHeight: 'calc(100vh - 4px)',
                borderRadius: 6,
            }
            : {}),
    };

    const contentInset = immersive ? '48px 8px 44px 8px' : '48px 12px 48px 12px';

    return (
        <div style={mergedDesktopStyle}>
            <div style={desktopHeaderStyle}>
                <strong style={{ color: 'var(--banana-panel-header-text-color, #e2e8f0)', fontSize: 12 }}>{label}</strong>
                <span style={{ color: 'var(--banana-panel-subtle-text-color, rgba(226,232,240,0.7))', fontSize: 11 }}>panels.banana.engineer</span>
            </div>
            <div style={{ position: 'absolute', inset: contentInset }}>{children}</div>
            <div style={taskbarStyle}>
                <span style={{ color: 'var(--banana-panel-header-text-color, #e2e8f0)', fontSize: 11 }}>banana.os</span>
                <span style={{ color: 'var(--banana-panel-subtle-text-color, rgba(226,232,240,0.75))', fontSize: 11 }}>workspace active</span>
            </div>
        </div>
    );
}
