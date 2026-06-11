import { useState, type ReactNode, type CSSProperties } from 'react';
import { ResizablePanel } from './ResizablePanel';
import { PanelOverlay } from './PanelOverlay';

export type ResizableDockEntry = {
    readonly id: string;
    readonly corner: 'top-left' | 'top-right' | 'bottom-left' | 'bottom-right';
    readonly visible: boolean;
    readonly content: ReactNode;
    readonly title?: string;
    readonly defaultWidth?: number;
    readonly defaultHeight?: number;
};

export type ResizableDockGridProps = {
    readonly entries: ResizableDockEntry[];
    readonly onPanelSizeChange?: (panelId: string, width: number, height: number) => void;
    readonly onPanelClose?: (panelId: string) => void;
};

export function ResizableDockGrid({ entries, onPanelSizeChange, onPanelClose }: ResizableDockGridProps) {
    const [expandedPanel, setExpandedPanel] = useState<string | null>(null);

    // Group entries by corner
    const corners = {
        'top-left': entries.filter((e) => e.corner === 'top-left' && e.visible),
        'top-right': entries.filter((e) => e.corner === 'top-right' && e.visible),
        'bottom-left': entries.filter((e) => e.corner === 'bottom-left' && e.visible),
        'bottom-right': entries.filter((e) => e.corner === 'bottom-right' && e.visible),
    };

    const renderCornerDock = (corner: 'top-left' | 'top-right' | 'bottom-left' | 'bottom-right', cornerEntries: ResizableDockEntry[]) => {
        if (cornerEntries.length === 0) return null;

        const isBottom = corner === 'bottom-left' || corner === 'bottom-right';
        const isRight = corner === 'top-right' || corner === 'bottom-right';

        const containerStyle: CSSProperties = {
            position: 'absolute',
            zIndex: 4,
            pointerEvents: 'auto',
            ...(isRight ? { right: 16 } : { left: 16 }),
            ...(isBottom ? { bottom: 96 } : { top: 52 }),
            maxWidth: 'min(460px, calc(50vw - 24px))',
            maxHeight: 'calc(50dvh - 92px)',
            display: 'flex',
            flexDirection: isBottom ? 'column-reverse' : 'column',
            gap: '12px',
            overflow: 'hidden',
        };

        const visiblePanels = cornerEntries;

        return (
            <div key={`dock-${corner}`} style={containerStyle}>
                {/* Main panels */}
                <div style={{
                    display: 'flex',
                    flexDirection: isBottom ? 'column-reverse' : 'column',
                    gap: '12px',
                    overflow: 'hidden',
                }}>
                    {visiblePanels.map((entry) => (
                        <ResizablePanel
                            key={entry.id}
                            id={entry.id}
                            title={entry.title}
                            defaultWidth={entry.defaultWidth || 360}
                            defaultHeight={entry.defaultHeight || 280}
                            corner={corner}
                            isCollapsed={false}
                            onCollapse={() => onPanelClose?.(entry.id)}
                            onExpand={() => setExpandedPanel(entry.id)}
                            onResize={(w, h) => onPanelSizeChange?.(entry.id, w, h)}
                        >
                            {entry.content}
                        </ResizablePanel>
                    ))}
                </div>
            </div>
        );
    };

    return (
        <>
            {renderCornerDock('top-left', corners['top-left'])}
            {renderCornerDock('top-right', corners['top-right'])}
            {renderCornerDock('bottom-left', corners['bottom-left'])}
            {renderCornerDock('bottom-right', corners['bottom-right'])}

            {/* Expanded panel overlay */}
            {expandedPanel && (
                <PanelOverlay
                    isOpen={true}
                    title={entries.find((e) => e.id === expandedPanel)?.title}
                    onClose={() => setExpandedPanel(null)}
                >
                    {entries.find((e) => e.id === expandedPanel)?.content}
                </PanelOverlay>
            )}
        </>
    );
}
