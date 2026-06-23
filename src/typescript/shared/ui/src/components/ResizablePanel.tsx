import { useEffect, useMemo, useRef, useState, type CSSProperties, type MouseEvent as ReactMouseEvent, type ReactNode } from 'react';
import { buildPanelBehaviorPipeline } from './PanelVariantPipeline';

export type ResizablePanelProps = {
    readonly id: string;
    readonly children: ReactNode;
    readonly title?: string;
    readonly x: number;
    readonly y: number;
    readonly width: number;
    readonly height: number;
    readonly minWidth?: number;
    readonly minHeight?: number;
    readonly onMove?: (panelId: string, nextPosition: { x: number; y: number }, phase: 'move' | 'end') => void;
    readonly onResize?: (panelId: string, rect: { x: number; y: number; width: number; height: number }) => void;
    readonly onCollapse?: () => void;
    readonly onExpand?: () => void;
    readonly onFocus?: () => void;
    readonly onUnlinkAll?: () => void;
    readonly onAnchorCommit?: (targetId: string, sourceSide: AnchorSide) => void;
    readonly snapIgnoreIds?: readonly string[];
    readonly isAnchored?: boolean;
    readonly groupSize?: number;
    readonly anchorSides?: readonly AnchorSide[];
    readonly groupColor?: string;
    readonly isGroupResizeLocked?: boolean;
    readonly onToggleGroupResizeLock?: () => void;
    readonly isCollapsed?: boolean;
    readonly corner?: 'top-left' | 'top-right' | 'bottom-left' | 'bottom-right';
    readonly zIndex?: number;
};

type ResizeDirection = 'left' | 'right' | 'top' | 'bottom' | 'bottom-right';
export type AnchorSide = 'left' | 'right' | 'top' | 'bottom';

type InteractionOrigin = {
    mouseX: number;
    mouseY: number;
    x: number;
    y: number;
    width: number;
    height: number;
    panelRect: DOMRect;
};

type SnapCandidate = {
    targetId: string;
    sourceSide: AnchorSide;
    targetSide: AnchorSide;
    snappedLeft: number;
    snappedTop: number;
    distance: number;
};

export function ResizablePanel({
    id,
    children,
    title,
    x,
    y,
    width,
    height,
    minWidth = 280,
    minHeight = 200,
    onMove,
    onResize,
    onCollapse,
    onExpand,
    onFocus,
    onUnlinkAll,
    onAnchorCommit,
    snapIgnoreIds = [],
    isAnchored = false,
    groupSize = 1,
    anchorSides = [],
    groupColor,
    isGroupResizeLocked = false,
    onToggleGroupResizeLock,
    isCollapsed = false,
    zIndex,
}: ResizablePanelProps) {
    const containerRef = useRef<HTMLDivElement>(null);
    const moveRafRef = useRef<number | null>(null);
    const resizeRafRef = useRef<number | null>(null);
    const pendingMoveRef = useRef<{ x: number; y: number } | null>(null);
    const pendingResizeRef = useRef<{ x: number; y: number; width: number; height: number } | null>(null);
    const [activeResize, setActiveResize] = useState<ResizeDirection | null>(null);
    const [isDragging, setIsDragging] = useState(false);
    const dragOriginRef = useRef<InteractionOrigin | null>(null);
    const resizeOriginRef = useRef<InteractionOrigin | null>(null);
    const [snapCandidate, setSnapCandidate] = useState<SnapCandidate | null>(null);
    const [ghostRect, setGhostRect] = useState<{ left: number; top: number; width: number; height: number } | null>(null);
    const snapLockRef = useRef<SnapCandidate | null>(null);
    const viewportPadding = 8;
    const snapThreshold = 16;
    const snapReleaseThreshold = 30;
    const snapActivationThreshold = 4;
    const behaviorPipeline = useMemo(
        () => buildPanelBehaviorPipeline({
            movable: Boolean(onMove),
            resizable: Boolean(onResize),
            dockable: Boolean(onAnchorCommit),
            modal: false,
        }),
        [onAnchorCommit, onMove, onResize],
    );

    const scheduleMoveUpdate = (nextPosition: { x: number; y: number }) => {
        pendingMoveRef.current = nextPosition;
        if (moveRafRef.current !== null) {
            return;
        }
        moveRafRef.current = window.requestAnimationFrame(() => {
            moveRafRef.current = null;
            const payload = pendingMoveRef.current;
            if (!payload) {
                return;
            }
            pendingMoveRef.current = null;
            onMove?.(id, payload, 'move');
        });
    };

    const flushMoveUpdate = () => {
        if (moveRafRef.current !== null) {
            window.cancelAnimationFrame(moveRafRef.current);
            moveRafRef.current = null;
        }
        pendingMoveRef.current = null;
    };

    const scheduleResizeUpdate = (rect: { x: number; y: number; width: number; height: number }) => {
        pendingResizeRef.current = rect;
        if (resizeRafRef.current !== null) {
            return;
        }
        resizeRafRef.current = window.requestAnimationFrame(() => {
            resizeRafRef.current = null;
            const payload = pendingResizeRef.current;
            if (!payload) {
                return;
            }
            pendingResizeRef.current = null;
            onResize?.(id, payload);
        });
    };

    const flushResizeUpdate = (commitPending: boolean = false) => {
        if (resizeRafRef.current !== null) {
            window.cancelAnimationFrame(resizeRafRef.current);
            resizeRafRef.current = null;
        }
        const payload = pendingResizeRef.current;
        pendingResizeRef.current = null;
        if (commitPending && payload) {
            onResize?.(id, payload);
        }
    };

    const startResize = (direction: ResizeDirection, event: ReactMouseEvent<HTMLDivElement>) => {
        event.preventDefault();
        event.stopPropagation();
        setIsDragging(false);
        snapLockRef.current = null;
        setSnapCandidate(null);
        setGhostRect(null);
        onFocus?.();
        const panelRect = containerRef.current?.getBoundingClientRect();
        if (!panelRect) {
            return;
        }
        resizeOriginRef.current = {
            mouseX: event.clientX,
            mouseY: event.clientY,
            x,
            y,
            width,
            height,
            panelRect,
        };
        setActiveResize(direction);
    };

    const getViewportLimits = () => {
        const maxWidth = Math.max(minWidth, window.innerWidth - (viewportPadding * 2));
        const maxHeight = Math.max(minHeight, window.innerHeight - (viewportPadding * 2));
        return { maxWidth, maxHeight };
    };

    const clampLeft = (left: number, panelWidth: number) => {
        const maxLeft = Math.max(viewportPadding, window.innerWidth - viewportPadding - panelWidth);
        return Math.max(viewportPadding, Math.min(maxLeft, left));
    };

    const clampTop = (top: number, panelHeight: number) => {
        const maxTop = Math.max(viewportPadding, window.innerHeight - viewportPadding - panelHeight);
        return Math.max(viewportPadding, Math.min(maxTop, top));
    };

    const resolveSnapCandidate = (
        rect: { left: number; top: number; width: number; height: number },
        preferredAxis: 'horizontal' | 'vertical',
        previousCandidate: SnapCandidate | null,
    ): SnapCandidate | null => {
        if (typeof document === 'undefined') {
            return null;
        }

        const candidates = Array.from(document.querySelectorAll<HTMLElement>('[data-resizable-panel="true"]'))
            .filter((element) => {
                const panelId = element.dataset.panelId;
                if (!panelId) {
                    return false;
                }
                if (panelId === id) {
                    return false;
                }
                return !snapIgnoreIds.includes(panelId);
            });

        let best: (SnapCandidate & { score: number }) | null = null;
        let sticky: SnapCandidate | null = null;
        const sourceRight = rect.left + rect.width;
        const sourceBottom = rect.top + rect.height;

        for (const element of candidates) {
            const targetId = element.dataset.panelId;
            if (!targetId) {
                continue;
            }

            const targetRect = element.getBoundingClientRect();
            const verticalOverlap = Math.min(sourceBottom, targetRect.bottom) - Math.max(rect.top, targetRect.top);
            const horizontalOverlap = Math.min(sourceRight, targetRect.right) - Math.max(rect.left, targetRect.left);

            const checks: Array<SnapCandidate | null> = [];

            if (verticalOverlap > Math.min(rect.height, targetRect.height) * 0.25) {
                const toLeft = Math.abs(rect.left - targetRect.right);
                if (toLeft <= snapThreshold) {
                    checks.push({
                        targetId,
                        sourceSide: 'right',
                        targetSide: 'left',
                        snappedLeft: targetRect.right,
                        snappedTop: rect.top,
                        distance: toLeft,
                    });
                }

                const toRight = Math.abs(sourceRight - targetRect.left);
                if (toRight <= snapThreshold) {
                    checks.push({
                        targetId,
                        sourceSide: 'left',
                        targetSide: 'right',
                        snappedLeft: targetRect.left - rect.width,
                        snappedTop: rect.top,
                        distance: toRight,
                    });
                }
            }

            if (horizontalOverlap > Math.min(rect.width, targetRect.width) * 0.25) {
                const toTop = Math.abs(rect.top - targetRect.bottom);
                if (toTop <= snapThreshold) {
                    checks.push({
                        targetId,
                        sourceSide: 'bottom',
                        targetSide: 'top',
                        snappedLeft: rect.left,
                        snappedTop: targetRect.bottom,
                        distance: toTop,
                    });
                }

                const toBottom = Math.abs(sourceBottom - targetRect.top);
                if (toBottom <= snapThreshold) {
                    checks.push({
                        targetId,
                        sourceSide: 'top',
                        targetSide: 'bottom',
                        snappedLeft: rect.left,
                        snappedTop: targetRect.top - rect.height,
                        distance: toBottom,
                    });
                }
            }

            for (const candidate of checks) {
                if (!candidate) {
                    continue;
                }

                if (previousCandidate &&
                    candidate.targetId === previousCandidate.targetId &&
                    candidate.sourceSide === previousCandidate.sourceSide &&
                    candidate.distance <= snapThreshold + 10) {
                    sticky = candidate;
                }

                const isHorizontal = candidate.sourceSide === 'left' || candidate.sourceSide === 'right';
                const axisBias = preferredAxis === 'horizontal'
                    ? (isHorizontal ? 0.76 : 1.24)
                    : (isHorizontal ? 1.24 : 0.76);
                const score = candidate.distance * axisBias;

                if (!best || score < best.score) {
                    best = {
                        ...candidate,
                        score,
                    };
                }
            }
        }

        const resolved = sticky ?? best;
        if (!resolved) {
            return null;
        }

        return {
            targetId: resolved.targetId,
            sourceSide: resolved.sourceSide,
            targetSide: resolved.targetSide,
            snappedLeft: clampLeft(resolved.snappedLeft, rect.width),
            snappedTop: clampTop(resolved.snappedTop, rect.height),
            distance: resolved.distance,
        };
    };

    const resolveLockedCandidate = (
        rect: { left: number; top: number; width: number; height: number },
        locked: SnapCandidate,
    ): SnapCandidate | null => {
        if (typeof document === 'undefined') {
            return null;
        }

        const target = document.querySelector<HTMLElement>(`[data-resizable-panel="true"][data-panel-id="${locked.targetId}"]`);
        if (!target) {
            return null;
        }

        const targetRect = target.getBoundingClientRect();
        const sourceRight = rect.left + rect.width;
        const sourceBottom = rect.top + rect.height;
        const overlapY = Math.min(sourceBottom, targetRect.bottom) - Math.max(rect.top, targetRect.top);
        const overlapX = Math.min(sourceRight, targetRect.right) - Math.max(rect.left, targetRect.left);

        let distance = Number.POSITIVE_INFINITY;
        let snappedLeft = rect.left;
        let snappedTop = rect.top;

        if (locked.sourceSide === 'right' && overlapY > Math.min(rect.height, targetRect.height) * 0.2) {
            distance = Math.abs(rect.left - targetRect.right);
            snappedLeft = targetRect.right;
        } else if (locked.sourceSide === 'left' && overlapY > Math.min(rect.height, targetRect.height) * 0.2) {
            distance = Math.abs(sourceRight - targetRect.left);
            snappedLeft = targetRect.left - rect.width;
        } else if (locked.sourceSide === 'bottom' && overlapX > Math.min(rect.width, targetRect.width) * 0.2) {
            distance = Math.abs(rect.top - targetRect.bottom);
            snappedTop = targetRect.bottom;
        } else if (locked.sourceSide === 'top' && overlapX > Math.min(rect.width, targetRect.width) * 0.2) {
            distance = Math.abs(sourceBottom - targetRect.top);
            snappedTop = targetRect.top - rect.height;
        }

        if (!Number.isFinite(distance)) {
            return null;
        }

        return {
            ...locked,
            snappedLeft: clampLeft(snappedLeft, rect.width),
            snappedTop: clampTop(snappedTop, rect.height),
            distance,
        };
    };

    useEffect(() => {
        if (!activeResize || !containerRef.current) return;

        const resizeOrigin = resizeOriginRef.current;
        if (!resizeOrigin) {
            return;
        }

        const handleMouseMove = (e: MouseEvent) => {
            e.preventDefault();

            const { maxWidth, maxHeight } = getViewportLimits();
            const dx = e.clientX - resizeOrigin.mouseX;
            const dy = e.clientY - resizeOrigin.mouseY;

            let nextWidth = resizeOrigin.width;
            let nextHeight = resizeOrigin.height;
            let nextX = resizeOrigin.x;
            let nextY = resizeOrigin.y;

            const originLeft = resizeOrigin.panelRect.left;
            const originTop = resizeOrigin.panelRect.top;
            const originRight = resizeOrigin.panelRect.right;
            const originBottom = resizeOrigin.panelRect.bottom;

            let nextLeft = originLeft;
            let nextTop = originTop;

            if (activeResize === 'right' || activeResize === 'bottom-right') {
                const maxRight = window.innerWidth - viewportPadding;
                const candidateRight = Math.min(maxRight, originRight + dx);
                nextWidth = Math.max(minWidth, Math.min(maxWidth, candidateRight - originLeft));
            }

            if (activeResize === 'left') {
                const minLeft = viewportPadding;
                const maxLeft = originRight - minWidth;
                nextLeft = Math.max(minLeft, Math.min(maxLeft, originLeft + dx));
                nextWidth = Math.max(minWidth, Math.min(maxWidth, originRight - nextLeft));
                nextX = resizeOrigin.x + (nextLeft - originLeft);
            }

            if (activeResize === 'bottom' || activeResize === 'bottom-right') {
                const maxBottom = window.innerHeight - viewportPadding;
                const candidateBottom = Math.min(maxBottom, originBottom + dy);
                nextHeight = Math.max(minHeight, Math.min(maxHeight, candidateBottom - originTop));
            }

            if (activeResize === 'top') {
                const minTop = viewportPadding;
                const maxTop = originBottom - minHeight;
                nextTop = Math.max(minTop, Math.min(maxTop, originTop + dy));
                nextHeight = Math.max(minHeight, Math.min(maxHeight, originBottom - nextTop));
                nextY = resizeOrigin.y + (nextTop - originTop);
            }

            scheduleResizeUpdate({
                x: nextX,
                y: nextY,
                width: nextWidth,
                height: nextHeight,
            });
        };

        const handleMouseUp = () => {
            // Commit the latest buffered resize so mouseup never drops final size.
            flushResizeUpdate(true);
            setActiveResize(null);
            resizeOriginRef.current = null;
            setGhostRect(null);
            setSnapCandidate(null);
        };

        document.body.style.userSelect = 'none';
        document.addEventListener('mousemove', handleMouseMove);
        document.addEventListener('mouseup', handleMouseUp);

        return () => {
            document.body.style.userSelect = '';
            document.removeEventListener('mousemove', handleMouseMove);
            document.removeEventListener('mouseup', handleMouseUp);
            flushResizeUpdate();
        };
    }, [activeResize, id, minHeight, minWidth, onResize]);

    useEffect(() => {
        if (!isDragging) {
            return;
        }

        const dragOrigin = dragOriginRef.current;
        if (!dragOrigin) {
            return;
        }

        const handleMouseMove = (event: MouseEvent) => {
            event.preventDefault();
            const candidateLeft = clampLeft(dragOrigin.panelRect.left + (event.clientX - dragOrigin.mouseX), width);
            const candidateTop = clampTop(dragOrigin.panelRect.top + (event.clientY - dragOrigin.mouseY), height);

            const isGroupedDrag = groupSize > 1;

            if (!isGroupedDrag) {
                const dragDx = event.clientX - dragOrigin.mouseX;
                const dragDy = event.clientY - dragOrigin.mouseY;
                const movementDistance = Math.hypot(dragDx, dragDy);
                if (movementDistance >= snapActivationThreshold) {
                    const preferredAxis = Math.abs(dragDx) >= Math.abs(dragDy) ? 'horizontal' : 'vertical';

                    const candidate = resolveSnapCandidate({
                        left: candidateLeft,
                        top: candidateTop,
                        width,
                        height,
                    }, preferredAxis, snapCandidate);

                    const locked = snapLockRef.current;
                    const lockedCandidate = locked
                        ? resolveLockedCandidate({ left: candidateLeft, top: candidateTop, width, height }, locked)
                        : null;

                    let effectiveCandidate = candidate;
                    if (lockedCandidate && lockedCandidate.distance <= snapReleaseThreshold) {
                        effectiveCandidate = lockedCandidate;
                    } else if (locked) {
                        snapLockRef.current = null;
                    }

                    if (!snapLockRef.current && effectiveCandidate && effectiveCandidate.distance <= snapThreshold) {
                        snapLockRef.current = effectiveCandidate;
                    }

                    setSnapCandidate(effectiveCandidate);
                    setGhostRect(effectiveCandidate ? { left: effectiveCandidate.snappedLeft, top: effectiveCandidate.snappedTop, width, height } : null);
                } else {
                    snapLockRef.current = null;
                    setSnapCandidate(null);
                    setGhostRect(null);
                }
            } else {
                snapLockRef.current = null;
                setSnapCandidate(null);
                setGhostRect(null);
            }

            const nextOffset = {
                x: dragOrigin.x + (candidateLeft - dragOrigin.panelRect.left),
                y: dragOrigin.y + (candidateTop - dragOrigin.panelRect.top),
            };
            scheduleMoveUpdate(nextOffset);
        };

        const handleMouseUp = () => {
            const dragOrigin = dragOriginRef.current;
            let finalOffset = pendingMoveRef.current ?? { x, y };
            if (dragOrigin && snapCandidate && groupSize <= 1) {
                finalOffset = {
                    x: dragOrigin.x + (snapCandidate.snappedLeft - dragOrigin.panelRect.left),
                    y: dragOrigin.y + (snapCandidate.snappedTop - dragOrigin.panelRect.top),
                };
            }
            if (dragOrigin) {
                flushMoveUpdate();
                onMove?.(id, finalOffset, 'end');
            }
            if (snapCandidate && groupSize <= 1) {
                onAnchorCommit?.(snapCandidate.targetId, snapCandidate.sourceSide);
            }
            setIsDragging(false);
            dragOriginRef.current = null;
            snapLockRef.current = null;
            setGhostRect(null);
            setSnapCandidate(null);
        };

        document.body.style.userSelect = 'none';
        document.addEventListener('mousemove', handleMouseMove);
        document.addEventListener('mouseup', handleMouseUp);

        return () => {
            document.body.style.userSelect = '';
            document.removeEventListener('mousemove', handleMouseMove);
            document.removeEventListener('mouseup', handleMouseUp);
            flushMoveUpdate();
        };
    }, [groupSize, height, id, isDragging, onAnchorCommit, onMove, snapCandidate, width, x, y]);

    useEffect(() => {
        return () => {
            flushMoveUpdate();
            flushResizeUpdate();
        };
    }, []);

    const handleHeaderMouseDown = (event: ReactMouseEvent<HTMLDivElement>) => {
        const target = event.target;
        if (target instanceof Element && target.closest('button')) {
            return;
        }
        event.preventDefault();
        dragOriginRef.current = {
            mouseX: event.clientX,
            mouseY: event.clientY,
            x,
            y,
            width,
            height,
            panelRect: containerRef.current?.getBoundingClientRect() as DOMRect,
        };

        if (!dragOriginRef.current.panelRect) {
            dragOriginRef.current = null;
            return;
        }
        snapLockRef.current = null;
        setSnapCandidate(null);
        setGhostRect(null);
        setIsDragging(true);
    };

    const containerStyle: CSSProperties = {
        width,
        height,
        flex: '0 0 auto',
        display: 'flex',
        flexDirection: 'column',
        background: 'rgba(7, 19, 34, 0.85)',
        border: '1px solid rgba(20, 184, 166, 0.2)',
        borderRadius: '6px',
        overflow: 'hidden',
        position: 'fixed',
        left: 0,
        top: 0,
        transform: `translate3d(${x}px, ${y}px, 0)`,
        zIndex: zIndex ?? 1,
        pointerEvents: 'auto',
        willChange: isDragging || activeResize ? 'transform, width, height' : undefined,
        contain: 'layout paint style',
        boxShadow: isAnchored
            ? `0 0 0 1px ${groupColor ?? 'rgba(34, 211, 238, 0.65)'}, 0 12px 24px rgba(8, 47, 73, 0.35)`
            : undefined,
    };

    const headerStyle: CSSProperties = {
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'space-between',
        padding: '8px 12px',
        borderBottom: '1px solid rgba(20, 184, 166, 0.15)',
        fontSize: '12px',
        fontWeight: 600,
        color: 'rgba(226, 232, 240, 0.7)',
        textTransform: 'uppercase',
        letterSpacing: '0.5px',
        cursor: isDragging ? 'grabbing' : 'grab',
        userSelect: 'none',
    };

    const contentStyle: CSSProperties = {
        flex: 1,
        overflow: 'auto',
        minHeight: 0,
        padding: '8px',
        fontSize: '12px',
        color: 'rgba(226, 232, 240, 0.8)',
    };

    const resizeHandleStyle: CSSProperties = {
        position: 'absolute',
        background: 'transparent',
        zIndex: 10,
    };

    const leftHandleStyle: CSSProperties = {
        ...resizeHandleStyle,
        left: 0,
        top: 0,
        bottom: 0,
        width: '6px',
        cursor: 'ew-resize',
    };

    const rightHandleStyle: CSSProperties = {
        ...resizeHandleStyle,
        right: 0,
        top: 0,
        bottom: 0,
        width: '6px',
        cursor: 'ew-resize',
    };

    const topHandleStyle: CSSProperties = {
        ...resizeHandleStyle,
        top: 0,
        left: 0,
        right: 0,
        height: '6px',
        cursor: 'ns-resize',
    };

    const bottomHandleStyle: CSSProperties = {
        ...resizeHandleStyle,
        bottom: 0,
        left: 0,
        right: 0,
        height: '6px',
        cursor: 'ns-resize',
    };

    const cornerGripStyle: CSSProperties = {
        position: 'absolute',
        right: 2,
        bottom: 2,
        width: 16,
        height: 16,
        border: 'none',
        borderRadius: 3,
        background: 'linear-gradient(135deg, transparent 0 36%, rgba(148, 163, 184, 0.65) 36% 42%, transparent 42% 56%, rgba(148, 163, 184, 0.65) 56% 62%, transparent 62% 76%, rgba(148, 163, 184, 0.65) 76% 82%, transparent 82% 100%)',
        cursor: 'nwse-resize',
        zIndex: 11,
    };

    const unlinkButtonStyle: CSSProperties = {
        border: 'none',
        background: 'none',
        color: isAnchored ? '#67e8f9' : 'rgba(148, 163, 184, 0.75)',
        cursor: 'pointer',
        padding: '2px 4px',
        display: 'inline-flex',
        alignItems: 'center',
        justifyContent: 'center',
        fontSize: '11px',
        lineHeight: 1,
    };

    const lockResizeButtonStyle: CSSProperties = {
        border: 'none',
        background: 'none',
        color: isGroupResizeLocked ? '#a7f3d0' : 'rgba(148, 163, 184, 0.75)',
        cursor: 'pointer',
        padding: '2px 4px',
        display: 'inline-flex',
        alignItems: 'center',
        justifyContent: 'center',
        fontSize: '11px',
        lineHeight: 1,
    };

    const sideBadgeContainerStyle: CSSProperties = {
        display: 'inline-flex',
        gap: 4,
        alignItems: 'center',
        marginLeft: 8,
    };

    const sideBadgeStyle = (side: AnchorSide): CSSProperties => ({
        minWidth: 16,
        height: 16,
        borderRadius: 999,
        border: `1px solid ${groupColor ?? 'rgba(56, 189, 248, 0.6)'}`,
        background: 'rgba(8, 47, 73, 0.35)',
        color: '#bae6fd',
        fontSize: 9,
        fontWeight: 700,
        display: 'inline-flex',
        alignItems: 'center',
        justifyContent: 'center',
        padding: '0 4px',
        textTransform: 'uppercase',
    });

    if (isCollapsed) {
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
                }}
            >
                {id.charAt(0).toUpperCase()}
            </div>
        );
    }

    return (
        <>
            {ghostRect && isDragging ? (
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
            ) : null}
            <div
                ref={containerRef}
                data-resizable-panel="true"
                data-panel-id={id}
                data-panel-behavior-pipeline={behaviorPipeline.layers.join('>')}
                style={containerStyle}
                onMouseDownCapture={onFocus}
            >
                {title && (
                    <div style={headerStyle} onMouseDown={handleHeaderMouseDown}>
                        <span style={{ display: 'inline-flex', alignItems: 'center' }}>
                            {title}
                            {anchorSides.length > 0 ? (
                                <span style={sideBadgeContainerStyle}>
                                    {anchorSides.map((side) => (
                                        <span key={`${id}-${side}`} style={sideBadgeStyle(side)}>{side.charAt(0)}</span>
                                    ))}
                                </span>
                            ) : null}
                        </span>
                        <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
                            {isAnchored && onUnlinkAll ? (
                                <button
                                    onClick={onUnlinkAll}
                                    style={unlinkButtonStyle}
                                    title={`Unlink ${groupSize > 1 ? `group (${groupSize})` : 'panel'}`}
                                >
                                    ⛓
                                </button>
                            ) : null}
                            {isAnchored && onToggleGroupResizeLock ? (
                                <button
                                    onClick={onToggleGroupResizeLock}
                                    style={lockResizeButtonStyle}
                                    title={isGroupResizeLocked ? 'Unlock group resize' : 'Lock group resize'}
                                >
                                    {isGroupResizeLocked ? '⇲' : '⇱'}
                                </button>
                            ) : null}
                            {onExpand && (
                                <button
                                    onClick={onExpand}
                                    style={{
                                        background: 'none',
                                        border: 'none',
                                        color: 'rgba(148, 163, 184, 0.8)',
                                        cursor: 'pointer',
                                        padding: '2px 4px',
                                        display: 'flex',
                                        alignItems: 'center',
                                        justifyContent: 'center',
                                        fontSize: '11px',
                                        lineHeight: 1,
                                    }}
                                    title="Expand panel"
                                >
                                    □
                                </button>
                            )}
                            {onCollapse && (
                                <button
                                    onClick={onCollapse}
                                    style={{
                                        background: 'none',
                                        border: 'none',
                                        color: 'rgba(226, 232, 240, 0.5)',
                                        cursor: 'pointer',
                                        padding: '2px',
                                        display: 'flex',
                                        alignItems: 'center',
                                        justifyContent: 'center',
                                    }}
                                    title="Collapse panel"
                                >
                                    ×
                                </button>
                            )}
                        </div>
                    </div>
                )}
                <div style={contentStyle}>{children}</div>
                <div style={leftHandleStyle} onMouseDown={(event) => startResize('left', event)} />
                <div style={rightHandleStyle} onMouseDown={(event) => startResize('right', event)} />
                <div style={topHandleStyle} onMouseDown={(event) => startResize('top', event)} />
                <div style={bottomHandleStyle} onMouseDown={(event) => startResize('bottom', event)} />
                <div style={cornerGripStyle} onMouseDown={(event) => startResize('bottom-right', event)} title="Resize panel" />
            </div>
        </>
    );
}
