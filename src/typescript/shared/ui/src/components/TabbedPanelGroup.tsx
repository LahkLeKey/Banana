import { useState, type ReactNode, type CSSProperties } from 'react';

export type TabbedPanelEntry = {
    readonly id: string;
    readonly title: string;
    readonly content: ReactNode;
};

export type TabbedPanelGroupProps = {
    readonly entries: TabbedPanelEntry[];
    readonly defaultActiveId?: string;
    readonly minWidth?: number;
    readonly minHeight?: number;
    readonly maxHeight?: string;
    readonly onTabChange?: (tabId: string) => void;
};

export function TabbedPanelGroup({
    entries,
    defaultActiveId,
    minWidth = 300,
    minHeight = 200,
    maxHeight = '50dvh',
    onTabChange,
}: TabbedPanelGroupProps) {
    const [activeTabId, setActiveTabId] = useState(defaultActiveId || entries[0]?.id || '');

    const handleTabChange = (tabId: string) => {
        setActiveTabId(tabId);
        onTabChange?.(tabId);
    };

    const activeEntry = entries.find((e) => e.id === activeTabId);

    const containerStyle: CSSProperties = {
        display: 'flex',
        flexDirection: 'column',
        background: 'rgba(7, 19, 34, 0.85)',
        border: '1px solid rgba(20, 184, 166, 0.2)',
        borderRadius: '6px',
        overflow: 'hidden',
        minWidth,
        minHeight,
        maxHeight,
        height: '100%',
    };

    const tabBarStyle: CSSProperties = {
        display: 'flex',
        borderBottom: '1px solid rgba(20, 184, 166, 0.15)',
        backgroundColor: 'rgba(0, 0, 0, 0.2)',
        gap: '2px',
        padding: '6px 8px',
        flexWrap: 'wrap',
        flexDirection: 'row',
    };

    const tabButtonStyle: (isActive: boolean) => CSSProperties = (isActive) => ({
        padding: '6px 12px',
        fontSize: '11px',
        fontWeight: 600,
        textTransform: 'uppercase',
        letterSpacing: '0.4px',
        color: isActive ? 'rgba(20, 184, 166, 0.9)' : 'rgba(226, 232, 240, 0.5)',
        background: isActive ? 'rgba(20, 184, 166, 0.1)' : 'transparent',
        border: isActive ? '1px solid rgba(20, 184, 166, 0.4)' : '1px solid rgba(20, 184, 166, 0.15)',
        borderRadius: '4px',
        cursor: 'pointer',
        transition: 'all 0.2s ease',
        whiteSpace: 'nowrap',
    });

    const contentStyle: CSSProperties = {
        flex: 1,
        overflow: 'auto',
        padding: '8px',
        minHeight: 0,
        fontSize: '12px',
        color: 'rgba(226, 232, 240, 0.8)',
        display: 'flex',
        flexDirection: 'column',
    };

    return (
        <div style={containerStyle}>
            <div style={tabBarStyle}>
                {entries.map((entry) => (
                    <button
                        key={entry.id}
                        onClick={() => handleTabChange(entry.id)}
                        style={tabButtonStyle(activeTabId === entry.id)}
                        onMouseEnter={(e) => {
                            if (activeTabId !== entry.id && e.currentTarget) {
                                e.currentTarget.style.borderColor = 'rgba(20, 184, 166, 0.3)';
                                e.currentTarget.style.color = 'rgba(226, 232, 240, 0.7)';
                            }
                        }}
                        onMouseLeave={(e) => {
                            if (activeTabId !== entry.id && e.currentTarget) {
                                e.currentTarget.style.borderColor = 'rgba(20, 184, 166, 0.15)';
                                e.currentTarget.style.color = 'rgba(226, 232, 240, 0.5)';
                            }
                        }}
                    >
                        {entry.title}
                    </button>
                ))}
            </div>
            <div style={contentStyle}>
                {activeEntry?.content}
            </div>
        </div>
    );
}
