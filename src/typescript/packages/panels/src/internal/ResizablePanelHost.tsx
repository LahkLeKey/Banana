import { type ReactNode } from 'react';

import { DragGhost } from './ResizablePanelChrome';
import type { DragGhostRect } from './usePanelDragSnap';

type ResizablePanelHostProps = {
    isDragging: boolean;
    ghostRect: DragGhostRect | null;
    zIndex?: number;
    children: ReactNode;
};

export function ResizablePanelHost(
    { isDragging, ghostRect, zIndex, children }: ResizablePanelHostProps) {
    return (
        <div
            data-resizable-panel-host="true"
            style={{ position: 'fixed', inset: 0, pointerEvents: 'none' }}>
            {ghostRect && isDragging ?
                <DragGhost ghostRect={ghostRect} zIndex={zIndex} /> :
                null}
            {children}
        </div>
    );
}
