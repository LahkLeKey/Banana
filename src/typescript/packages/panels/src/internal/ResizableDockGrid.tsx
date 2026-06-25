import { useEffect, useMemo, useRef, useState, type ReactNode, type CSSProperties, type HTMLAttributes } from 'react';
import { ResizablePanel, type AnchorSide } from './ResizablePanel';
import { PanelOverlay } from './PanelOverlay';
import type { PanelIntrinsicElement } from './PanelIntrinsicElement';
import { useResizableDockLayoutStore, type DockCorner } from './useResizableDockLayoutStore';

type AnchorLink = { id: string; side: AnchorSide; };
type DockLayoutStoreState = ReturnType<typeof useResizableDockLayoutStore.getState>;

const groupPalette = ['#22d3ee', '#f59e0b', '#a78bfa', '#34d399', '#f472b6', '#60a5fa'];

type DockPanelStage = 'container' | 'header' | 'content';
type DockPanelStageElements = Partial<Record<DockPanelStage, PanelIntrinsicElement>>;
type DockPanelStageElementProps = Partial<Record<DockPanelStage, HTMLAttributes<HTMLElement>>>;

export type ResizableDockEntryIdentity = {
    readonly id: string;
    readonly corner: DockCorner;
    readonly visible: boolean;
    readonly title?: string;
};

export type ResizableDockEntryLayout = {
    readonly defaultWidth?: number;
    readonly defaultHeight?: number;
    readonly resetToken?: number;
};

export type ResizableDockEntryRender = {
    readonly content: ReactNode;
    readonly stageElements?: DockPanelStageElements;
    readonly stageElementProps?: DockPanelStageElementProps;
};

export type ResizableDockEntry =
    ResizableDockEntryIdentity &
    ResizableDockEntryLayout &
    ResizableDockEntryRender;

export type ResizableDockGridCallbacks = {
    readonly onPanelSizeChange?: (panelId: string, width: number, height: number) => void;
    readonly onPanelClose?: (panelId: string) => void;
};

export type ResizableDockInitialAnchorLink = {
    readonly sourceId: string;
    readonly targetId: string;
    readonly sourceSide: AnchorSide;
};

export type ResizableDockGridProps = ResizableDockGridCallbacks & {
    readonly entries: ResizableDockEntry[];
    readonly hostMode?: 'viewport' | 'container';
    readonly initialAnchorLinks?: readonly ResizableDockInitialAnchorLink[];
};

const EMPTY_INITIAL_ANCHOR_LINKS: readonly ResizableDockInitialAnchorLink[] = [];

export function ResizableDockGrid({
    entries,
    onPanelSizeChange,
    onPanelClose,
    hostMode = 'viewport',
    initialAnchorLinks,
}: ResizableDockGridProps) {
    const layoutIdRef = useRef(`dock-${Math.random().toString(36).slice(2)}`);
    const layoutId = layoutIdRef.current;
    const dockRootRef = useRef<HTMLDivElement>(null);
    const lastInitializationSignatureRef = useRef<string>('');
    const [expandedPanel, setExpandedPanel] = useState<string | null>(null);
    const initializeLayout = useResizableDockLayoutStore((state: DockLayoutStoreState) => state.initializeLayout);
    const focusPanelInStore = useResizableDockLayoutStore((state: DockLayoutStoreState) => state.focusPanel);
    const movePanelGroupBy = useResizableDockLayoutStore((state: DockLayoutStoreState) => state.movePanelGroupBy);
    const resizePanel = useResizableDockLayoutStore((state: DockLayoutStoreState) => state.resizePanel);
    const addAnchorLinkToStore = useResizableDockLayoutStore((state: DockLayoutStoreState) => state.addAnchorLink);
    const unlinkPanelFromStore = useResizableDockLayoutStore((state: DockLayoutStoreState) => state.unlinkPanel);
    const toggleResizeLockInStore = useResizableDockLayoutStore((state: DockLayoutStoreState) => state.toggleResizeLock);
    const layout = useResizableDockLayoutStore((state: DockLayoutStoreState) => state.layouts[layoutId]);

    const anchorLinks = initialAnchorLinks ?? EMPTY_INITIAL_ANCHOR_LINKS;
    const entrySeedKey = entries.map((entry) =>
        `${entry.id}:${entry.corner}:${entry.defaultWidth ?? ''}:${entry.defaultHeight ?? ''}:${entry.resetToken ?? 0}`,
    ).join('|');
    const initialAnchorLinksKey = anchorLinks.map((link) =>
        `${link.sourceId}:${link.targetId}:${link.sourceSide}`,
    ).join('|');

    const entrySeeds = useMemo(() => entries.map((entry) => ({
        id: entry.id,
        corner: entry.corner,
        defaultWidth: entry.defaultWidth,
        defaultHeight: entry.defaultHeight,
        resetToken: entry.resetToken,
    })), [entrySeedKey]);

    useEffect(() => {
        const initialize = () => {
            const bounds = hostMode === 'container'
                ? dockRootRef.current?.getBoundingClientRect()
                : null;

            const viewport = {
                width: Math.round(bounds?.width ?? window.innerWidth),
                height: Math.round(bounds?.height ?? window.innerHeight),
            };
            const initializationSignature = `${layoutId}:${entrySeedKey}:${initialAnchorLinksKey}:${viewport.width}:${viewport.height}`;
            if (initializationSignature === lastInitializationSignatureRef.current) {
                return;
            }

            lastInitializationSignatureRef.current = initializationSignature;
            initializeLayout(layoutId, entrySeeds, viewport);
            anchorLinks.forEach(({ sourceId, targetId, sourceSide }) => {
                addAnchorLinkToStore(layoutId, sourceId, targetId, sourceSide);
            });
        };

        initialize();
        window.addEventListener('resize', initialize);
        return () => {
            window.removeEventListener('resize', initialize);
        };
    }, [addAnchorLinkToStore, entrySeedKey, entrySeeds, hostMode, initializeLayout, initialAnchorLinksKey, layoutId]);

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
            <div ref={dockRootRef} style={dockStyle}>
                {visibleEntries.map((entry) => {
                    const panel = layout?.panels[entry.id];
                    if (!panel) {
                        return null;
                    }

                    const titleElementId = `${layoutId}-panel-title-${entry.id}`;

                    const groupMembers = getAnchorGroupMembers(entry.id);
                    const groupSize = groupMembers.length;
                    const groupColor = getGroupColor(groupMembers);

                    return (
                        <ResizablePanel
                            key={entry.id}
                            id={entry.id}
                            title={entry.title}
                            hostMode={hostMode}
                            interactionScope={layoutId}
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
                            titleElementId={entry.title ? titleElementId : undefined}
                            stageElements={{
                                container: 'section',
                                header: 'header',
                                content: 'section',
                                ...entry.stageElements,
                            }}
                            stageElementProps={{
                                ...entry.stageElementProps,
                                container: {
                                    role: 'region',
                                    'aria-labelledby': entry.title ? titleElementId : undefined,
                                    'aria-label': entry.title ? `${entry.title} panel` : `${entry.id} panel`,
                                    ...entry.stageElementProps?.container,
                                },
                            }}
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
                    chrome={{ movable: true, resizable: true, dockable: true }}
                    stageStyles={{
                        backdrop: { background: 'rgba(2, 6, 23, 0.64)' },
                        window: {
                            borderRadius: '12px',
                            border: '1px solid rgba(45, 212, 191, 0.3)',
                            boxShadow: '0 24px 72px rgba(2, 6, 23, 0.78)',
                        },
                        content: { fontSize: '12px' },
                    }}
                >
                    {entries.find((e) => e.id === expandedPanel)?.content}
                </PanelOverlay>
            )}
        </>
    );
}
