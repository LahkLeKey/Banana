import { useEffect, useMemo, useRef, useState, type ReactNode, type CSSProperties } from 'react';
import { ResizablePanel, type AnchorSide } from './ResizablePanel';
import { PanelOverlay } from './PanelOverlay';
import { useResizableDockLayoutStore, type DockCorner } from '../hooks/useResizableDockLayoutStore';

type AnchorLink = { id: string; side: AnchorSide; };

const groupPalette = ['#22d3ee', '#f59e0b', '#a78bfa', '#34d399', '#f472b6', '#60a5fa'];

export type ResizableDockEntry = {
    readonly id: string;
    readonly corner: DockCorner;
    readonly visible: boolean;
    readonly content: ReactNode;
    readonly title?: string;
    readonly defaultWidth?: number;
    readonly defaultHeight?: number;
    readonly resetToken?: number;
};

export type ResizableDockGridProps = {
    readonly entries: ResizableDockEntry[];
    readonly onPanelSizeChange?: (panelId: string, width: number, height: number) => void;
    readonly onPanelClose?: (panelId: string) => void;
};

export function ResizableDockGrid({ entries, onPanelSizeChange, onPanelClose }: ResizableDockGridProps) {
    const layoutIdRef = useRef(`dock-${Math.random().toString(36).slice(2)}`);
    const layoutId = layoutIdRef.current;
    const [expandedPanel, setExpandedPanel] = useState<string | null>(null);
    const initializeLayout = useResizableDockLayoutStore((state) => state.initializeLayout);
    const focusPanelInStore = useResizableDockLayoutStore((state) => state.focusPanel);
    const movePanelGroupBy = useResizableDockLayoutStore((state) => state.movePanelGroupBy);
    const resizePanel = useResizableDockLayoutStore((state) => state.resizePanel);
    const addAnchorLinkToStore = useResizableDockLayoutStore((state) => state.addAnchorLink);
    const unlinkPanelFromStore = useResizableDockLayoutStore((state) => state.unlinkPanel);
    const toggleResizeLockInStore = useResizableDockLayoutStore((state) => state.toggleResizeLock);
    const layout = useResizableDockLayoutStore((state) => state.layouts[layoutId]);

    useEffect(() => {
        const toSeeds = () => entries.map((entry) => ({
            id: entry.id,
            corner: entry.corner,
            defaultWidth: entry.defaultWidth,
            defaultHeight: entry.defaultHeight,
            resetToken: entry.resetToken,
        }));

        const initialize = () => {
            initializeLayout(layoutId, toSeeds(), {
                width: window.innerWidth,
                height: window.innerHeight,
            });
        };

        initialize();
        window.addEventListener('resize', initialize);
        return () => {
            window.removeEventListener('resize', initialize);
        };
    }, [entries, initializeLayout, layoutId]);

    const focusPanel = (panelId: string) => {
        focusPanelInStore(layoutId, panelId);
    };

    const addAnchorLink = (a: string, b: string, sourceSide: AnchorSide) => {
        addAnchorLinkToStore(layoutId, a, b, sourceSide);
    };

    const unlinkPanel = (panelId: string) => {
        unlinkPanelFromStore(layoutId, panelId);
    };

    const getAnchorGroupMembers = (panelId: string): string[] => {
        if (!layout) {
            return [panelId];
        }
        const visited = new Set<string>();
        const queue = [panelId];
        while (queue.length > 0) {
            const current = queue.shift()!;
            if (visited.has(current)) {
                continue;
            }
            visited.add(current);
            for (const neighbor of layout.anchorLinks[current] ?? []) {
                if (!visited.has(neighbor.id)) {
                    queue.push(neighbor.id);
                }
            }
        }
        return [...visited];
    };

    const handlePanelMove = (panelId: string, nextPosition: { x: number; y: number; }, phase: 'move' | 'end') => {
        const panel = layout?.panels[panelId];
        if (!panel) {
            return;
        }
        movePanelGroupBy(layoutId, panelId, {
            x: nextPosition.x - panel.x,
            y: nextPosition.y - panel.y,
        });
        if (phase === 'end') {
            // no-op marker for parity with panel callback lifecycle
        }
    };

    const handlePanelResize = (panelId: string, rect: { x: number; y: number; width: number; height: number }) => {
        resizePanel(layoutId, panelId, rect);
        onPanelSizeChange?.(panelId, rect.width, rect.height);
    };

    const toggleGroupResizeLock = (panelId: string) => {
        toggleResizeLockInStore(layoutId, panelId);
    };

    const getGroupColor = (members: string[]): string | undefined => {
        if (members.length <= 1) {
            return undefined;
        }
        const key = members.slice().sort().join('|');
        let hash = 0;
        for (let i = 0; i < key.length; i += 1) {
            hash = ((hash << 5) - hash + key.charCodeAt(i)) | 0;
        }
        const paletteIndex = Math.abs(hash) % groupPalette.length;
        return groupPalette[paletteIndex];
    };

    const getAnchorSides = (panelId: string): AnchorSide[] => {
        const links: AnchorLink[] = layout?.anchorLinks[panelId] ?? [];
        return [...new Set(links.map((entry) => entry.side))];
    };

    const getPanelZIndex = (panelId: string) => {
        const panel = layout?.panels[panelId];
        if (!panel) {
            return 10;
        }
        return panel.zIndex;
    };

    const visibleEntries = useMemo(
        () => entries.filter((entry) => entry.visible),
        [entries],
    );

    const dockStyle: CSSProperties = {
        position: 'absolute',
        inset: 0,
        pointerEvents: 'none',
        overflow: 'hidden',
    };

    return (
        <>
            <div style={dockStyle}>
                {visibleEntries.map((entry) => {
                    const panel = layout?.panels[entry.id];
                    if (!panel) {
                        return null;
                    }

                    const groupMembers = getAnchorGroupMembers(entry.id);
                    const groupSize = groupMembers.length;
                    const groupColor = getGroupColor(groupMembers);

                    return (
                        <ResizablePanel
                            key={entry.id}
                            id={entry.id}
                            title={entry.title}
                            x={panel.x}
                            y={panel.y}
                            width={panel.width}
                            height={panel.height}
                            minWidth={280}
                            minHeight={200}
                            zIndex={getPanelZIndex(entry.id)}
                            isCollapsed={false}
                            onFocus={() => focusPanel(entry.id)}
                            isAnchored={groupSize > 1}
                            groupSize={groupSize}
                            onUnlinkAll={() => unlinkPanel(entry.id)}
                            onAnchorCommit={(targetId, sourceSide) => addAnchorLink(entry.id, targetId, sourceSide)}
                            onMove={(id, nextPosition, phase) => handlePanelMove(id, nextPosition, phase)}
                            onResize={(id, rect) => handlePanelResize(id, rect)}
                            snapIgnoreIds={groupMembers}
                            anchorSides={getAnchorSides(entry.id)}
                            groupColor={groupColor}
                            isGroupResizeLocked={layout?.resizeLockedByPanel[entry.id] ?? false}
                            onToggleGroupResizeLock={() => toggleGroupResizeLock(entry.id)}
                            onCollapse={() => onPanelClose?.(entry.id)}
                            onExpand={() => setExpandedPanel(entry.id)}
                        >
                            {entry.content}
                        </ResizablePanel>
                    );
                })}
            </div>

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
