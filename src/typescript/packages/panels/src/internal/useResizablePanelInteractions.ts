import {useEffect} from 'react';

import {usePanelDragSnap} from './usePanelDragSnap';
import {usePanelResize} from './usePanelResize';
import {useRafBufferedDispatch} from './useRafBufferedDispatch';
import {type UseResizablePanelInteractionsOptions,} from './useResizablePanelInteractionContracts';

export {type ResizablePanelInteractionCallbacks, type ResizablePanelInteractionConstraintsInput, type ResizablePanelInteractionGeometryInput, type ResizablePanelInteractionGroupingInput, type ResizablePanelInteractionIdentityInput, type ResizablePanelInteractionRefsInput, type UseResizablePanelInteractionsOptions,} from './useResizablePanelInteractionContracts';

export function useResizablePanelInteractions({
  id,
  x,
  y,
  width,
  height,
  minWidth,
  minHeight,
  groupSize,
  snapIgnoreIds,
  containerRef,
  onMove,
  onResize,
  onAnchorCommit,
  onFocus,
}: UseResizablePanelInteractionsOptions) {
  const moveDispatch =
      useRafBufferedDispatch<{x: number; y: number}>((payload) => {
        onMove?.(id, payload, 'move');
      });

  const resizeDispatch = useRafBufferedDispatch<
      {x: number; y: number; width: number; height: number;}>((payload) => {
    onResize?.(id, payload);
  });

  const {isDragging, ghostRect, handleHeaderMouseDown, clearDragState} =
      usePanelDragSnap({
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
      });

  const {activeResize, startResize} = usePanelResize({
    x,
    y,
    width,
    height,
    minWidth,
    minHeight,
    containerRef,
    clearDragState,
    onFocus,
    resizeDispatch,
  });

  useEffect(() => {
    return () => {
      moveDispatch.flush();
      resizeDispatch.flush();
    };
  }, [moveDispatch, resizeDispatch]);

  return {
    isDragging,
    ghostRect,
    handleHeaderMouseDown,
    activeResize,
    startResize,
  };
}
