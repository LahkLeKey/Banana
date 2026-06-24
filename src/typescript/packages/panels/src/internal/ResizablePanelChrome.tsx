import { type CSSProperties, type MouseEvent as ReactMouseEvent } from 'react';

import type { DragGhostRect } from './usePanelDragSnap';

export type ResizeDirection = 'left' | 'right' | 'top' | 'bottom' | 'bottom-right';

type CollapsedLauncherProps = {
    id: string;
    x: number;
    y: number;
    onExpand?: () => void;
};

export function CollapsedLauncher({ id, x, y, onExpand }: CollapsedLauncherProps) {
    return (
        <div
            style={{
                position: 'fixed',
                left: 0,
                top: 0,
                transform: `translate3d(${x}px, ${y}px, 0)`,
                width: '40px',
                height: '40px',
                background: 'rgba(7, 19, 34, 0.9)',
                border: '1px solid rgba(20, 184, 166, 0.2)',
                borderRadius: '4px',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'center',
                cursor: onExpand ? 'pointer' : 'default',
                fontSize: '11px',
                color: 'rgba(226, 232, 240, 0.6)',
                overflow: 'hidden',
            }}
            role={onExpand ? 'button' : undefined}
            tabIndex={onExpand ? 0 : undefined}
            onClick={onExpand}
            onKeyDown={(event) => {
                if (!onExpand) {
                    return;
                }
                if (event.key === 'Enter' || event.key === ' ') {
                    event.preventDefault();
                    onExpand();
                }
            }}>
            {id.charAt(0).toUpperCase()}
        </div>
    );
}

type DragGhostProps = {
    ghostRect: DragGhostRect;
    zIndex?: number;
};

export function DragGhost({ ghostRect, zIndex }: DragGhostProps) {
    return (
        <div
            style={{
                position: 'fixed',
                left: ghostRect.left,
                top: ghostRect.top,
                width: ghostRect.width,
                height: ghostRect.height,
                borderRadius: 8,
                border: '1px dashed rgba(56, 189, 248, 0.95)',
                background: 'rgba(12, 74, 110, 0.12)',
                boxShadow: '0 0 0 1px rgba(56, 189, 248, 0.35) inset',
                zIndex: (zIndex ?? 1) + 200,
                pointerEvents: 'none',
            }}
        />
    );
}

type ResizeHandlesProps = {
    leftHandleStyle: CSSProperties;
    rightHandleStyle: CSSProperties;
    topHandleStyle: CSSProperties;
    bottomHandleStyle: CSSProperties;
    cornerGripStyle: CSSProperties;
    onStartResize: (direction: ResizeDirection,
        event: ReactMouseEvent<HTMLDivElement>) => void;
};

export function ResizeHandles({
    leftHandleStyle,
    rightHandleStyle,
    topHandleStyle,
    bottomHandleStyle,
    cornerGripStyle,
    onStartResize,
}: ResizeHandlesProps) {
    return (
        <>
            <div style={leftHandleStyle} onMouseDown={(event) => onStartResize('left', event)} />
            <div
                style={rightHandleStyle}
                onMouseDown={(event) => onStartResize('right', event)}
            />
            <div style={topHandleStyle} onMouseDown={(event) => onStartResize('top', event)} />
            <div
                style={bottomHandleStyle}
                onMouseDown={(event) => onStartResize('bottom', event)}
            />
            <div
                style={cornerGripStyle}
                onMouseDown={(event) => onStartResize('bottom-right', event)}
                title="Resize panel"
            />
        </>
    );
}
