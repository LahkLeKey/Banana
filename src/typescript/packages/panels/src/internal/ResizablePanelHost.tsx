import { type ReactNode } from 'react';

import { DragGhost } from './ResizablePanelChrome';
import type { DragGhostRect } from './usePanelDragSnap';

type ResizablePanelHostProps = {
    isDragging: boolean;
    ghostRect: DragGhostRect | null;
    zIndex?: number;
    hostMode?: 'viewport' | 'container';
    children: ReactNode;
};

export function ResizablePanelHost(
    { isDragging, ghostRect, zIndex, hostMode = 'viewport', children }: ResizablePanelHostProps) {
    return (
        <div
            data-resizable-panel-host="true"
            style={{
                position: hostMode === 'container' ? 'absolute' : 'fixed',
                inset: 0,
                pointerEvents: 'none',
                zIndex: zIndex ?? 1,
            }}>
            {ghostRect && isDragging ?
                <DragGhost ghostRect={ghostRect} zIndex={zIndex} /> :
                null}
            {children}
        </div>
    );
}
