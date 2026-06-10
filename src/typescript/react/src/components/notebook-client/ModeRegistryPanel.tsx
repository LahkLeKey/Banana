import type { CSSProperties } from 'react';

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
    top: 18,
    left: '50%',
    transform: 'translateX(-50%)',
    zIndex: 8,
    pointerEvents: 'none',
    width: 'min(760px, calc(100vw - 36px))',
    borderRadius: 12,
    border: '1px solid rgba(45, 212, 191, 0.24)',
    background: 'linear-gradient(180deg, rgba(2, 12, 24, 0.82), rgba(3, 18, 32, 0.78))',
    boxShadow: '0 14px 30px rgba(2, 6, 23, 0.45)',
    padding: '8px 10px',
    color: '#cbd5e1',
    fontFamily: '"IBM Plex Mono", "Cascadia Mono", monospace',
    fontSize: 11,
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

    return (
        <aside style={shellStyle}>
            <div style={{ display: 'flex', justifyContent: 'space-between', gap: 12, flexWrap: 'wrap' }}>
                <span style={{ color: '#67e8f9', letterSpacing: '0.04em' }}>
                    MODE REGISTRY (DEV)
                </span>
                <span style={{ color: '#93c5fd' }}>
                    route={pathname}
                </span>
            </div>
            <div style={{ marginTop: 6, display: 'flex', gap: 12, flexWrap: 'wrap' }}>
                <span>mode={mode}</span>
                <span>label={label}</span>
                <span>{dockFlags}</span>
                <span>reducedMotion:{prefersReducedMotion ? 'on' : 'off'}</span>
            </div>

            <div style={{ marginTop: 8, display: 'flex', gap: 6, flexWrap: 'wrap', pointerEvents: 'auto' }}>
                {['/notebooks', '/login', '/marketing', '/account'].map((href) => (
                    <a
                        key={href}
                        href={href}
                        style={{
                            borderRadius: 999,
                            border: '1px solid rgba(148, 163, 184, 0.3)',
                            background: 'rgba(8, 13, 28, 0.45)',
                            color: '#93c5fd',
                            textDecoration: 'none',
                            padding: '4px 8px',
                            fontSize: 10,
                            letterSpacing: '0.03em',
                        }}
                    >
                        {href}
                    </a>
                ))}
            </div>

            <div style={{ marginTop: 8, color: '#94a3b8' }}>
                transitionHistory={transitions.map((entry) => `${entry.time}:${entry.mode}@${entry.pathname}`).join('  |  ') || 'none'}
            </div>
        </aside>
    );
}
