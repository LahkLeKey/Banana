import type { ReactNode } from 'react';

type RouteHudControlStripProps = {
    readonly routeLabel: string;
    readonly showExplorer: boolean;
    readonly explorerDropupOpen: boolean;
    readonly showMenu: boolean;
    readonly showOperations: boolean;
    readonly showStatus: boolean;
    readonly onToggleExplorer: () => void;
    readonly onToggleMenu: () => void;
    readonly onToggleOperations: () => void;
    readonly onToggleStatus: () => void;
    readonly onFocusViewport: () => void;
    readonly onResetHud: () => void;
};

type HudButtonProps = {
    readonly label: string;
    readonly onClick: () => void;
    readonly active?: boolean;
    readonly muted?: boolean;
};

function HudButton({ label, onClick, active = false, muted = false }: HudButtonProps) {
    return (
        <button
            type="button"
            onClick={onClick}
            style={{
                borderRadius: 999,
                border: active ? '1px solid rgba(45, 212, 191, 0.6)' : '1px solid rgba(148, 163, 184, 0.3)',
                background: active ? 'rgba(13, 148, 136, 0.18)' : 'rgba(8, 13, 28, 0.4)',
                color: muted ? '#94a3b8' : '#e2e8f0',
                padding: '6px 11px',
                cursor: 'pointer',
                fontSize: 11,
                fontWeight: 700,
                whiteSpace: 'nowrap',
            }}
        >
            {label}
        </button>
    );
}

function Bar({ children }: { readonly children: ReactNode }) {
    return (
        <div style={{
            position: 'absolute',
            left: '50%',
            bottom: 10,
            transform: 'translateX(-50%)',
            zIndex: 5,
            pointerEvents: 'auto',
            width: 'min(700px, calc(100vw - 28px))',
            borderRadius: 999,
            border: '1px solid rgba(45, 212, 191, 0.26)',
            background: 'linear-gradient(180deg, rgba(2, 10, 20, 0.74), rgba(3, 20, 33, 0.78))',
            boxShadow: '0 10px 24px rgba(2, 6, 23, 0.38)',
            backdropFilter: 'blur(6px)',
            padding: '8px 10px',
            display: 'flex',
            justifyContent: 'center',
            gap: 6,
            flexWrap: 'wrap',
        }}>
            {children}
        </div>
    );
}

export function RouteHudControlStrip({
    routeLabel,
    showExplorer,
    explorerDropupOpen,
    showMenu,
    showOperations,
    showStatus,
    onToggleExplorer,
    onToggleMenu,
    onToggleOperations,
    onToggleStatus,
    onFocusViewport,
    onResetHud,
}: RouteHudControlStripProps) {
    return (
        <Bar>
            <HudButton
                label="Explorer"
                onClick={onToggleExplorer}
                active={showExplorer || explorerDropupOpen}
            />

            <HudButton
                label="Command Menu"
                onClick={onToggleMenu}
                active={showMenu}
            />

            <HudButton
                label="Operations"
                onClick={onToggleOperations}
                active={showOperations}
            />

            <HudButton
                label="Status"
                onClick={onToggleStatus}
                active={showStatus}
            />

            <span style={{
                borderRadius: 999,
                border: '1px solid rgba(148, 163, 184, 0.3)',
                background: 'rgba(8, 13, 28, 0.5)',
                color: '#93c5fd',
                padding: '8px 13px',
                fontSize: 11,
                fontWeight: 700,
                letterSpacing: '0.04em',
                textTransform: 'uppercase',
                whiteSpace: 'nowrap',
            }}>
                {routeLabel}
            </span>

            <div style={{ width: 1, background: 'rgba(45, 212, 191, 0.2)', alignSelf: 'stretch', margin: '0 2px' }} />

            <HudButton
                label="Focus Viewport"
                onClick={onFocusViewport}
                muted
            />
            <HudButton
                label="Reset HUD"
                onClick={onResetHud}
                muted
            />
        </Bar>
    );
}
