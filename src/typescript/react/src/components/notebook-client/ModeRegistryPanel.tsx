import { useState, type CSSProperties } from 'react';
import { PanelBase } from '@banana/ui';

import type { NotebookRouteMode } from '../../lib/notebook-client/routeModeConfig';

type ModeRegistryPanelProps = {
    pathname: string;
    mode: NotebookRouteMode;
    label: string;
    explorer: boolean;
    menu: boolean;
    status: boolean;
    operations: boolean;
    prefersReducedMotion: boolean;
    transitions: Array<{ pathname: string; mode: NotebookRouteMode; time: string; }>;
};

const shellStyle: CSSProperties = {
    position: 'absolute',
    top: 48,
    zIndex: 3,
    width: 'min(380px, calc(100vw - 28px))',
    maxHeight: 'calc(100dvh - 120px)',
    pointerEvents: 'none',
};

function formatFlag(name: string, enabled: boolean): string {
    return `${name}:${enabled ? 'on' : 'off'}`;
}

export function ModeRegistryPanel(props: ModeRegistryPanelProps) {
    const {
        pathname,
        mode,
        label,
        explorer,
        menu,
        status,
        operations,
        prefersReducedMotion,
        transitions = [],
    } = props;

    const dockFlags = [
        formatFlag('explorer', explorer),
        formatFlag('menu', menu),
        formatFlag('status', status),
        formatFlag('operations', operations),
    ].join('  ');
    const [expanded, setExpanded] = useState(false);

    const containerStyle: CSSProperties = {
        ...shellStyle,
        ...(explorer ? { left: 14 } : { right: 14 }),
        width: expanded ? 'min(520px, calc(100vw - 28px))' : shellStyle.width,
        maxHeight: expanded ? '62dvh' : '156px',
    };

    return (
        <aside style={containerStyle}>
            <div style={{ pointerEvents: 'auto' }}>
                <PanelBase
                    title="Mode Registry (Dev)"
                    variant="compact"
                    headerAction={(
                        <button
                            type="button"
                            onClick={() => setExpanded((current) => !current)}
                            style={{
                                borderRadius: 6,
                                border: '1px solid rgba(45, 212, 191, 0.3)',
                                background: expanded ? 'rgba(45, 212, 191, 0.14)' : 'rgba(8, 47, 73, 0.4)',
                                color: '#5eead4',
                                padding: '2px 8px',
                                fontSize: 9,
                                fontWeight: 600,
                                textTransform: 'uppercase',
                                cursor: 'pointer',
                            }}
                            title={expanded ? 'Compact mode registry' : 'Expand mode registry'}
                        >
                            {expanded ? 'Compact' : 'Expand'}
                        </button>
                    )}
                >
                    <div style={{ display: 'flex', justifyContent: 'space-between', gap: 10, flexWrap: 'wrap', fontSize: 11, fontFamily: '"IBM Plex Mono", monospace', marginBottom: 8 }}>
                        <span style={{ color: '#67e8f9' }}>route={pathname}</span>
                        <span style={{ color: '#93c5fd' }}>mode={mode}</span>
                    </div>
                    <div style={{ fontSize: 11, fontFamily: '"IBM Plex Mono", monospace', marginBottom: 8 }}>
                        <div style={{ color: '#cbd5e1', marginBottom: 3 }}>label={label}</div>
                        <div style={{ color: '#94a3b8' }}>{dockFlags}</div>
                        <div style={{ color: '#94a3b8' }}>reducedMotion:{prefersReducedMotion ? 'on' : 'off'}</div>
                    </div>

                    <div style={{ marginTop: 6, display: 'flex', gap: 4, flexWrap: 'wrap', pointerEvents: 'auto' }}>
                        {['/notebooks', '/login', '/marketing', '/account'].map((href) => (
                            <a
                                key={href}
                                href={href}
                                style={{
                                    borderRadius: 6,
                                    border: '1px solid rgba(45, 212, 191, 0.3)',
                                    background: 'rgba(8, 47, 73, 0.4)',
                                    color: '#5eead4',
                                    textDecoration: 'none',
                                    padding: '4px 8px',
                                    fontSize: 9,
                                    fontWeight: 500,
                                    transition: 'all 0.15s ease',
                                }}
                                onMouseEnter={(e) => {
                                    e.currentTarget.style.background = 'rgba(45, 212, 191, 0.15)';
                                    e.currentTarget.style.borderColor = 'rgba(45, 212, 191, 0.5)';
                                }}
                                onMouseLeave={(e) => {
                                    e.currentTarget.style.background = 'rgba(8, 47, 73, 0.4)';
                                    e.currentTarget.style.borderColor = 'rgba(45, 212, 191, 0.3)';
                                }}
                            >
                                {href}
                            </a>
                        ))}
                    </div>

                    {expanded ? (
                        <div style={{ marginTop: 6, fontSize: 9, color: '#64748b', whiteSpace: 'nowrap', overflow: 'hidden', textOverflow: 'ellipsis', fontFamily: '"IBM Plex Mono", monospace' }}>
                            history={transitions.map((entry) => `${entry.time}:${entry.mode}@${entry.pathname}`).join(' | ') || 'none'}
                        </div>
                    ) : null}
                </PanelBase>
            </div>
        </aside>
    );
}
