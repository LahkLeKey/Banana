import {type MouseEvent as ReactMouseEvent, type RefObject, useEffect, useRef, useState} from 'react';

import type {RafBufferedDispatch} from './useRafBufferedDispatch';

export type DragSnapAnchorSide = 'left'|'right'|'top'|'bottom';

type InteractionOrigin = {
  mouseX: number; mouseY: number; x: number; y: number; width: number;
  height: number;
  panelRect: DOMRect;
};

type SnapCandidate = {
  targetId: string; sourceSide: DragSnapAnchorSide;
  targetSide: DragSnapAnchorSide;
  snappedLeft: number;
  snappedTop: number;
  distance: number;
};

export type DragGhostRect = {
  left: number; top: number; width: number; height: number;
};

type UsePanelDragSnapOptions = {
  id: string; x: number; y: number; width: number; height: number;
  groupSize: number;
  snapIgnoreIds: readonly string[];
  containerRef: RefObject<HTMLElement|null>;
  moveDispatch: Pick<
      RafBufferedDispatch<{x: number; y: number}>,
      'pendingRef'|'schedule'|'flush'>;
  onMove?: (panelId: string, nextPosition: {x: number; y: number},
            phase: 'move'|'end') => void;
  onAnchorCommit?: (targetId: string, sourceSide: DragSnapAnchorSide) => void;
};

type UsePanelDragSnapResult = {
  isDragging: boolean; ghostRect: DragGhostRect | null;
  handleHeaderMouseDown: (event: ReactMouseEvent<HTMLElement>) => void;
  clearDragState: () => void;
};

export function usePanelDragSnap({
  id,
  x,
  y,
  width,
  height,
  groupSize,
  snapIgnoreIds,
  containerRef,
  moveDispatch,
  onMove,
  onAnchorCommit,
}: UsePanelDragSnapOptions): UsePanelDragSnapResult {
  const dragOriginRef = useRef<InteractionOrigin|null>(null);
  const [isDragging, setIsDragging] = useState(false);
  const [snapCandidate, setSnapCandidate] = useState<SnapCandidate|null>(null);
  const [ghostRect, setGhostRect] = useState<DragGhostRect|null>(null);
  const snapLockRef = useRef<SnapCandidate|null>(null);
  const viewportPadding = 8;
  const snapThreshold = 16;
  const snapReleaseThreshold = 30;
  const snapActivationThreshold = 4;

  const clampLeft = (left: number, panelWidth: number) => {
    const maxLeft = Math.max(
        viewportPadding, window.innerWidth - viewportPadding - panelWidth);
    return Math.max(viewportPadding, Math.min(maxLeft, left));
  };

  const clampTop = (top: number, panelHeight: number) => {
    const maxTop = Math.max(
        viewportPadding, window.innerHeight - viewportPadding - panelHeight);
    return Math.max(viewportPadding, Math.min(maxTop, top));
  };

  const resolveSnapCandidate =
      (rect: {left: number; top: number; width: number; height: number},
       preferredAxis: 'horizontal'|'vertical',
       previousCandidate: SnapCandidate|null): SnapCandidate|null => {
        const candidates = Array
                               .from(document.querySelectorAll<HTMLElement>(
                                   '[data-resizable-panel="true"]'))
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

        let best: (SnapCandidate&{score: number})|null = null;
        let sticky: SnapCandidate|null = null;
        const sourceRight = rect.left + rect.width;
        const sourceBottom = rect.top + rect.height;

        for (const element of candidates) {
          const targetId = element.dataset.panelId;
          if (!targetId) {
            continue;
          }

          const targetRect = element.getBoundingClientRect();
          const verticalOverlap = Math.min(sourceBottom, targetRect.bottom) -
              Math.max(rect.top, targetRect.top);
          const horizontalOverlap = Math.min(sourceRight, targetRect.right) -
              Math.max(rect.left, targetRect.left);

          const checks: SnapCandidate[] = [];

          if (verticalOverlap >
              Math.min(rect.height, targetRect.height) * 0.25) {
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

          if (horizontalOverlap >
              Math.min(rect.width, targetRect.width) * 0.25) {
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
            if (previousCandidate &&
                candidate.targetId === previousCandidate.targetId &&
                candidate.sourceSide === previousCandidate.sourceSide &&
                candidate.distance <= snapThreshold + 10) {
              sticky = candidate;
            }

            const isHorizontal = candidate.sourceSide === 'left' ||
                candidate.sourceSide === 'right';
            const axisBias = preferredAxis === 'horizontal' ?
                (isHorizontal ? 0.76 : 1.24) :
                (isHorizontal ? 1.24 : 0.76);
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

  const resolveLockedCandidate =
      (rect: {left: number; top: number; width: number; height: number},
       locked: SnapCandidate): SnapCandidate|null => {
        const target = document.querySelector<HTMLElement>(
            `[data-resizable-panel="true"][data-panel-id="${
                locked.targetId}"]`);
        if (!target) {
          return null;
        }

        const targetRect = target.getBoundingClientRect();
        const sourceRight = rect.left + rect.width;
        const sourceBottom = rect.top + rect.height;
        const overlapY = Math.min(sourceBottom, targetRect.bottom) -
            Math.max(rect.top, targetRect.top);
        const overlapX = Math.min(sourceRight, targetRect.right) -
            Math.max(rect.left, targetRect.left);

        let distance = Number.POSITIVE_INFINITY;
        let snappedLeft = rect.left;
        let snappedTop = rect.top;

        if (locked.sourceSide === 'right' &&
            overlapY > Math.min(rect.height, targetRect.height) * 0.2) {
          distance = Math.abs(rect.left - targetRect.right);
          snappedLeft = targetRect.right;
        } else if (
            locked.sourceSide === 'left' &&
            overlapY > Math.min(rect.height, targetRect.height) * 0.2) {
          distance = Math.abs(sourceRight - targetRect.left);
          snappedLeft = targetRect.left - rect.width;
        } else if (
            locked.sourceSide === 'bottom' &&
            overlapX > Math.min(rect.width, targetRect.width) * 0.2) {
          distance = Math.abs(rect.top - targetRect.bottom);
          snappedTop = targetRect.bottom;
        } else if (
            locked.sourceSide === 'top' &&
            overlapX > Math.min(rect.width, targetRect.width) * 0.2) {
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

  const clearDragState = () => {
    setIsDragging(false);
    dragOriginRef.current = null;
    snapLockRef.current = null;
    setGhostRect(null);
    setSnapCandidate(null);
  };

  useEffect(
      () => {
        const dragOrigin = dragOriginRef.current;
        if (!isDragging || !dragOrigin) {
          return;
        }

        const handleMouseMove = (event: MouseEvent) => {
          event.preventDefault();
          const candidateLeft = clampLeft(
              dragOrigin.panelRect.left + (event.clientX - dragOrigin.mouseX),
              width);
          const candidateTop = clampTop(
              dragOrigin.panelRect.top + (event.clientY - dragOrigin.mouseY),
              height);

          const isGroupedDrag = groupSize > 1;

          if (!isGroupedDrag) {
            const dragDx = event.clientX - dragOrigin.mouseX;
            const dragDy = event.clientY - dragOrigin.mouseY;
            const movementDistance = Math.hypot(dragDx, dragDy);
            if (movementDistance >= snapActivationThreshold) {
              const preferredAxis = Math.abs(dragDx) >= Math.abs(dragDy) ?
                  'horizontal' :
                  'vertical';

              const candidate = resolveSnapCandidate(
                  {
                    left: candidateLeft,
                    top: candidateTop,
                    width,
                    height,
                  },
                  preferredAxis, snapCandidate);

              const locked = snapLockRef.current;
              const lockedCandidate = locked ?
                  resolveLockedCandidate(
                      {left: candidateLeft, top: candidateTop, width, height},
                      locked) :
                  null;

              let effectiveCandidate = candidate;
              if (lockedCandidate &&
                  lockedCandidate.distance <= snapReleaseThreshold) {
                effectiveCandidate = lockedCandidate;
              } else if (locked) {
                snapLockRef.current = null;
              }

              if (!snapLockRef.current && effectiveCandidate &&
                  effectiveCandidate.distance <= snapThreshold) {
                snapLockRef.current = effectiveCandidate;
              }

              setSnapCandidate(effectiveCandidate);
              setGhostRect(
                  effectiveCandidate ? {
                    left: effectiveCandidate.snappedLeft,
                    top: effectiveCandidate.snappedTop,
                    width,
                    height,
                  } :
                                       null);
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
          moveDispatch.schedule(nextOffset);
        };

        const handleMouseUp = () => {
          const activeDragOrigin = dragOriginRef.current;
          let finalOffset = moveDispatch.pendingRef.current ?? {x, y};
          if (activeDragOrigin && snapCandidate && groupSize <= 1) {
            finalOffset = {
              x: activeDragOrigin.x +
                  (snapCandidate.snappedLeft - activeDragOrigin.panelRect.left),
              y: activeDragOrigin.y +
                  (snapCandidate.snappedTop - activeDragOrigin.panelRect.top),
            };
          }

          if (activeDragOrigin) {
            moveDispatch.flush();
            onMove?.(id, finalOffset, 'end');
          }
          if (snapCandidate && groupSize <= 1) {
            onAnchorCommit?.(snapCandidate.targetId, snapCandidate.sourceSide);
          }
          clearDragState();
        };

        document.body.style.userSelect = 'none';
        document.addEventListener('mousemove', handleMouseMove);
        document.addEventListener('mouseup', handleMouseUp);

        return () => {
          document.body.style.userSelect = '';
          document.removeEventListener('mousemove', handleMouseMove);
          document.removeEventListener('mouseup', handleMouseUp);
          moveDispatch.flush();
        };
      },
      [
        groupSize, height, id, isDragging, moveDispatch, onAnchorCommit, onMove,
        snapCandidate, width, x, y
      ]);

  const handleHeaderMouseDown = (event: ReactMouseEvent<HTMLElement>) => {
    const target = event.target;
    if (target instanceof Element && target.closest('button')) {
      return;
    }

    event.preventDefault();
    const panelRect =
        containerRef.current?.getBoundingClientRect() as DOMRect | null;
    if (!panelRect) {
      dragOriginRef.current = null;
      return;
    }

    dragOriginRef.current = {
      mouseX: event.clientX,
      mouseY: event.clientY,
      x,
      y,
      width,
      height,
      panelRect,
    };

    snapLockRef.current = null;
    setSnapCandidate(null);
    setGhostRect(null);
    setIsDragging(true);
  };

  return {
    isDragging,
    ghostRect,
    handleHeaderMouseDown,
    clearDragState,
  };
}
