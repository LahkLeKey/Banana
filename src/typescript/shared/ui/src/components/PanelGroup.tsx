import { useState, type ReactNode, type CSSProperties } from 'react';
import { ResizablePanel, type ResizablePanelProps } from './ResizablePanel';

export type PanelGroupEntry = {
    readonly id: string;
    readonly title?: string;
    readonly children: ReactNode;
    readonly defaultWidth?: number;
    readonly defaultHeight?: number;
};

export type PanelGroupProps = {
    readonly entries: PanelGroupEntry[];
    readonly layout?: 'grid' | 'stack' | 'flex';
    readonly gap?: number;
    readonly maxColumns?: number;
    readonly corner?: 'top-left' | 'top-right' | 'bottom-left' | 'bottom-right';
    readonly onPanelSizeChange?: (panelId: string, width: number, height: number) => void;
};

export function PanelGroup({
    entries,
    layout = 'grid',
    gap = 12,
    maxColumns = 2,
    corner = 'top-left',
    onPanelSizeChange,
}: PanelGroupProps) {
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

    const containerStyle: CSSProperties = {
        display: 'flex',
        gap: `${gap}px`,
        flexWrap: 'wrap',
        alignContent: 'flex-start',
        width: '100%',
        height: '100%',
    };

    const stackStyle: CSSProperties = {
        display: 'flex',
        flexDirection: 'column',
        gap: `${gap}px`,
        width: '100%',
        height: '100%',
        overflow: 'auto',
    };

    const gridStyle: CSSProperties = {
        display: 'grid',
        gridTemplateColumns: `repeat(${maxColumns}, 1fr)`,
        gap: `${gap}px`,
        width: '100%',
        height: '100%',
        gridAutoFlow: 'dense' as const,
    };

    const collapsedTabsStyle: CSSProperties = {
        display: 'flex',
        flexDirection: 'column',
        gap: '4px',
        padding: '8px',
    };

    // Separate active and collapsed panels
    const activePanels = entries.filter((e) => !collapsedPanels.has(e.id));
    const collapsedTabPanels = entries.filter((e) => collapsedPanels.has(e.id));

    const activeLayoutStyle = layout === 'stack' ? stackStyle : layout === 'grid' ? gridStyle : containerStyle;

    return (
        <div style={{
            display: 'flex',
            gap: `${gap}px`,
            width: '100%',
            height: '100%',
        }}>
            {/* Main panel area */}
            <div style={activeLayoutStyle}>
                {activePanels.map((entry) => (
                    <ResizablePanel
                        key={entry.id}
                        id={entry.id}
                        title={entry.title}
                        defaultWidth={entry.defaultWidth}
                        defaultHeight={entry.defaultHeight}
                        isCollapsed={expandedPanel === entry.id}
                        onCollapse={() => toggleCollapse(entry.id)}
                        onResize={(w, h) => onPanelSizeChange?.(entry.id, w, h)}
                        corner={corner}
                    >
                        {entry.children}
                    </ResizablePanel>
                ))}
            </div>

            {/* Collapsed tabs sidebar */}
            {collapsedTabPanels.length > 0 && (
                <div style={collapsedTabsStyle}>
                    {collapsedTabPanels.map((entry) => (
                        <button
                            key={entry.id}
                            onClick={() => toggleCollapse(entry.id)}
                            style={{
                                width: '40px',
                                height: '40px',
                                background: 'rgba(7, 19, 34, 0.9)',
                                border: '1px solid rgba(20, 184, 166, 0.2)',
                                borderRadius: '4px',
                                display: 'flex',
                                alignItems: 'center',
                                justifyContent: 'center',
                                cursor: 'pointer',
                                fontSize: '11px',
                                color: 'rgba(226, 232, 240, 0.6)',
                                overflow: 'hidden',
                                textTransform: 'uppercase',
                                fontWeight: 600,
                                transition: 'all 0.2s',
                            }}
                            onMouseEnter={(e) => {
                                if (e.currentTarget) {
                                    e.currentTarget.style.borderColor = 'rgba(20, 184, 166, 0.4)';
                                    e.currentTarget.style.color = 'rgba(226, 232, 240, 0.8)';
                                }
                            }}
                            onMouseLeave={(e) => {
                                if (e.currentTarget) {
                                    e.currentTarget.style.borderColor = 'rgba(20, 184, 166, 0.2)';
                                    e.currentTarget.style.color = 'rgba(226, 232, 240, 0.6)';
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
}
