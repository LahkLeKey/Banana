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
};

export function ResizableDockGrid({ entries, onPanelSizeChange }: ResizableDockGridProps) {
    const [collapsedPanels, setCollapsedPanels] = useState<Set<string>>(new Set());
    const [expandedPanel, setExpandedPanel] = useState<string | null>(null);

    const toggleCollapse = (panelId: string) => {
        setCollapsedPanels((prev) => {
            const next = new Set(prev);
            if (next.has(panelId)) {
                next.delete(panelId);
            } else {
                next.add(panelId);
            }
            return next;
        });
    };

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
            ...(isBottom ? { bottom: 76 } : { top: 52 }),
            maxWidth: 'min(480px, 50vw)',
            maxHeight: 'calc(50dvh - 60px)',
            display: 'flex',
            flexDirection: isBottom ? 'column-reverse' : 'column',
            gap: '12px',
            overflow: 'visible',
        };

        const visiblePanels = cornerEntries.filter((e) => !collapsedPanels.has(e.id));
        const collapsedPanels_ = cornerEntries.filter((e) => collapsedPanels.has(e.id));

        return (
            <div key={`dock-${corner}`} style={containerStyle}>
                {/* Main panels */}
                <div style={{
                    display: 'flex',
                    flexDirection: isBottom ? 'column-reverse' : 'column',
                    gap: '12px',
                    overflow: 'visible',
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
                            onCollapse={() => toggleCollapse(entry.id)}
                            onResize={(w, h) => onPanelSizeChange?.(entry.id, w, h)}
                        >
                            {entry.content}
                        </ResizablePanel>
                    ))}
                </div>

                {/* Collapsed tabs */}
                {collapsedPanels_.length > 0 && (
                    <div style={{
                        display: 'flex',
                        flexDirection: 'column',
                        gap: '6px',
                        padding: '4px',
                    }}>
                        {collapsedPanels_.map((entry) => (
                            <button
                                key={entry.id}
                                onClick={() => toggleCollapse(entry.id)}
                                style={{
                                    width: '38px',
                                    height: '38px',
                                    background: 'rgba(7, 19, 34, 0.9)',
                                    border: '1px solid rgba(20, 184, 166, 0.2)',
                                    borderRadius: '4px',
                                    display: 'flex',
                                    alignItems: 'center',
                                    justifyContent: 'center',
                                    cursor: 'pointer',
                                    fontSize: '10px',
                                    color: 'rgba(226, 232, 240, 0.5)',
                                    fontWeight: 600,
                                    textTransform: 'uppercase',
                                    transition: 'all 0.2s ease',
                                    pointerEvents: 'auto',
                                }}
                                onMouseEnter={(e) => {
                                    if (e.currentTarget) {
                                        e.currentTarget.style.borderColor = 'rgba(20, 184, 166, 0.5)';
                                        e.currentTarget.style.color = 'rgba(226, 232, 240, 0.8)';
                                        e.currentTarget.style.background = 'rgba(7, 19, 34, 0.95)';
                                    }
                                }}
                                onMouseLeave={(e) => {
                                    if (e.currentTarget) {
                                        e.currentTarget.style.borderColor = 'rgba(20, 184, 166, 0.2)';
                                        e.currentTarget.style.color = 'rgba(226, 232, 240, 0.5)';
                                        e.currentTarget.style.background = 'rgba(7, 19, 34, 0.9)';
                                    }
                                }}
                                title={`Show ${entry.title || entry.id}`}
                            >
                                {entry.id.charAt(0).toUpperCase()}
                            </button>
                        ))}
                    </div>
                )}
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
