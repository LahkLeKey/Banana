import { type CSSProperties, type ReactNode } from 'react';
import { Link, useLocation } from 'react-router-dom';

import { RouteActionButton } from '@banana/ui';

type GameplayShellProps = {
    readonly routeLabel: string;
    readonly title?: string;
    readonly subtitle?: string;
    readonly activeOperator?: string;
    readonly onSignOut: () => void;
    readonly children: ReactNode;
    readonly background: string;
    readonly contentStyle?: CSSProperties;
};

type NavItem = {
    readonly to: string;
    readonly label: string;
    readonly locked?: boolean;
};

const navItems: NavItem[] = [
    { to: '/', label: 'Home' },
    { to: '/characters', label: 'Characters' },
    { to: '/world-map', label: 'World Map', locked: true },
    { to: '/legacy', label: 'Legacy' },
    { to: '/profile', label: 'Profile' },
];

export function GameplayShell({
    routeLabel,
    title,
    subtitle,
    activeOperator,
    onSignOut,
    children,
    background,
    contentStyle,
}: GameplayShellProps) {
    const location = useLocation();
    const lockedWorldMap = !activeOperator;

    return (
        <div style={{ minHeight: '100dvh', background, color: '#e2e8f0' }}>
            <header style={headerStyle}>
                <div style={brandBlockStyle}>
                    <div style={routeLabelStyle}>{routeLabel}</div>
                    {title ? <div style={titleStyle}>{title}</div> : null}
                    {subtitle ? <div style={subtitleStyle}>{subtitle}</div> : null}
                </div>

                <nav style={navStyle} aria-label="Gameplay navigation">
                    {navItems.map((item) => {
                        const isActive = location.pathname === item.to || (item.to !== '/' && location.pathname.startsWith(`${item.to}/`));
                        const isLocked = item.to === '/world-map' && lockedWorldMap;
                        return (
                            <Link
                                key={item.to}
                                to={item.to}
                                style={{
                                    ...navLinkStyle,
                                    ...(isActive ? navLinkActiveStyle : null),
                                    ...(isLocked ? navLinkLockedStyle : null),
                                }}
                                title={isLocked ? 'Choose an operator before opening the world map.' : undefined}
                            >
                                <span>{item.label}</span>
                                {isLocked ? <span style={lockBadgeStyle}>Locked</span> : null}
                            </Link>
                        );
                    })}
                </nav>

                <div style={accountBlockStyle}>
                    <div style={operatorPillStyle}>
                        <span style={operatorPillLabelStyle}>Operator</span>
                        <span style={operatorPillValueStyle}>{activeOperator || 'none selected'}</span>
                    </div>
                    <RouteActionButton onClick={onSignOut} tone="neutral">
                        Sign out
                    </RouteActionButton>
                </div>
            </header>

            <main style={{ ...mainStyle, ...contentStyle }}>{children}</main>
        </div>
    );
}

const headerStyle = {
    position: 'sticky' as const,
    top: 0,
    zIndex: 40,
    display: 'grid',
    gridTemplateColumns: 'minmax(240px, 1.1fr) minmax(320px, 1.4fr) auto',
    alignItems: 'center',
    gap: 16,
    padding: '16px 22px',
    borderBottom: '1px solid rgba(148, 163, 184, 0.16)',
    background: 'rgba(3, 8, 18, 0.72)',
    backdropFilter: 'blur(16px)',
};

const brandBlockStyle = {
    display: 'grid',
    gap: 3,
};

const routeLabelStyle = {
    fontSize: 11,
    fontWeight: 800,
    letterSpacing: '0.14em',
    textTransform: 'uppercase' as const,
    color: '#67e8f9',
};

const titleStyle = {
    fontSize: 18,
    fontWeight: 800,
    color: '#f8fafc',
};

const subtitleStyle = {
    fontSize: 13,
    lineHeight: 1.45,
    color: '#cbd5e1',
};

const navStyle = {
    display: 'flex',
    gap: 10,
    flexWrap: 'wrap' as const,
    justifyContent: 'center',
    alignItems: 'center',
};

const navLinkStyle = {
    display: 'inline-flex',
    alignItems: 'center',
    gap: 8,
    padding: '10px 14px',
    borderRadius: 14,
    border: '1px solid rgba(148, 163, 184, 0.22)',
    textDecoration: 'none',
    color: '#e2e8f0',
    background: 'rgba(8, 13, 28, 0.42)',
    fontWeight: 700,
    fontSize: 14,
    whiteSpace: 'nowrap' as const,
    transition: 'transform 0.15s ease, background 0.15s ease, border-color 0.15s ease',
};

const navLinkActiveStyle = {
    border: '1px solid rgba(45, 212, 191, 0.48)',
    background: 'rgba(13, 148, 136, 0.26)',
    color: '#ccfbf1',
};

const navLinkLockedStyle = {
    opacity: 0.72,
    borderStyle: 'dashed' as const,
};

const lockBadgeStyle = {
    fontSize: 10,
    fontWeight: 800,
    letterSpacing: '0.08em',
    textTransform: 'uppercase' as const,
    color: '#fca5a5',
};

const accountBlockStyle = {
    display: 'flex',
    alignItems: 'center',
    gap: 12,
    justifySelf: 'end',
};

const operatorPillStyle = {
    display: 'inline-grid',
    gap: 2,
    padding: '8px 12px',
    borderRadius: 14,
    border: '1px solid rgba(45, 212, 191, 0.18)',
    background: 'rgba(8, 13, 28, 0.55)',
    minWidth: 150,
};

const operatorPillLabelStyle = {
    fontSize: 10,
    letterSpacing: '0.12em',
    textTransform: 'uppercase' as const,
    color: '#7dd3fc',
    fontWeight: 800,
};

const operatorPillValueStyle = {
    fontSize: 13,
    fontWeight: 700,
    color: '#e2e8f0',
};

const mainStyle = {
    padding: '28px 20px 44px',
};
