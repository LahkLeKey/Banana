import type { ReactNode } from 'react';

type RouteSubActionTab = {
    readonly id: string;
    readonly label: string;
};

type RouteSubActionBarProps = {
    readonly tabs?: RouteSubActionTab[];
    readonly activeTab?: string;
    readonly onTabChange?: (id: string) => void;
    readonly actions?: ReactNode;
    readonly meta?: string;
};

export type { RouteSubActionTab };

/**
 * Compact contextual action strip for dock panels.
 * Replaces ad-hoc inline button rows with a consistent sub-bar contract.
 * - Left: tab strip (optional)
 * - Right: action slot (optional)
 * - Below: meta label (optional)
 */
export function RouteSubActionBar({
    tabs,
    activeTab,
    onTabChange,
    actions,
    meta,
}: RouteSubActionBarProps) {
    return (
        <div style={{
            display: 'flex',
            flexDirection: 'column',
            gap: 0,
            flexShrink: 0,
        }}>
            <div style={{
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'space-between',
                gap: 8,
                padding: '7px 12px',
                borderBottom: '1px solid rgba(45, 212, 191, 0.14)',
                background: 'rgba(3, 10, 20, 0.55)',
                flexWrap: 'wrap',
                flexShrink: 0,
            }}>
                {tabs && tabs.length > 0 ? (
                    <div style={{ display: 'flex', gap: 4 }}>
                        {tabs.map((tab) => (
                            <button
                                key={tab.id}
                                type="button"
                                onClick={() => onTabChange?.(tab.id)}
                                style={{
                                    borderRadius: 8,
                                    border: tab.id === activeTab
                                        ? '1px solid rgba(45, 212, 191, 0.5)'
                                        : '1px solid rgba(148, 163, 184, 0.22)',
                                    background: tab.id === activeTab
                                        ? 'rgba(13, 148, 136, 0.28)'
                                        : 'transparent',
                                    color: tab.id === activeTab ? '#ccfbf1' : '#94a3b8',
                                    fontSize: 11,
                                    fontWeight: 700,
                                    letterSpacing: '0.04em',
                                    textTransform: 'uppercase',
                                    padding: '5px 10px',
                                    cursor: 'pointer',
                                    transition: 'background 0.14s',
                                }}
                            >
                                {tab.label}
                            </button>
                        ))}
                    </div>
                ) : null}

                {actions ? (
                    <div style={{ display: 'flex', gap: 6, alignItems: 'center', marginLeft: 'auto', flexShrink: 0 }}>
                        {actions}
                    </div>
                ) : null}
            </div>

            {meta ? (
                <div style={{
                    padding: '3px 12px 4px',
                    fontSize: 10,
                    color: '#5eead4',
                    letterSpacing: '0.06em',
                    textTransform: 'uppercase',
                    background: 'rgba(3, 10, 20, 0.42)',
                    borderBottom: '1px solid rgba(45, 212, 191, 0.08)',
                    flexShrink: 0,
                }}>
                    {meta}
                </div>
            ) : null}
        </div>
    );
}

type RouteSubActionLinkProps = {
    readonly href: string;
    readonly children: ReactNode;
};

export function RouteSubActionLink({ href, children }: RouteSubActionLinkProps) {
    return (
        <a
            href={href}
            style={{
                fontSize: 10,
                fontWeight: 700,
                letterSpacing: '0.06em',
                textTransform: 'uppercase',
                color: '#5eead4',
                textDecoration: 'none',
                border: '1px solid rgba(45, 212, 191, 0.3)',
                borderRadius: 6,
                padding: '3px 7px',
                background: 'rgba(6, 30, 42, 0.6)',
                whiteSpace: 'nowrap',
            }}
        >
            {children}
        </a>
    );
}
