import {useEffect} from 'react';

import {usePanelDragSnap} from './usePanelDragSnap';
import {usePanelResize} from './usePanelResize';
import {useRafBufferedDispatch} from './useRafBufferedDispatch';
import {type UseResizablePanelInteractionsOptions,} from './useResizablePanelInteractionContracts';

export {type ResizablePanelInteractionCallbacks, type ResizablePanelInteractionConstraintsInput, type ResizablePanelInteractionGeometryInput, type ResizablePanelInteractionGroupingInput, type ResizablePanelInteractionIdentityInput, type ResizablePanelInteractionRefsInput, type UseResizablePanelInteractionsOptions,} from './useResizablePanelInteractionContracts';

export function useResizablePanelInteractions({
  id,
  interactionScope,
  x,
  y,
  width,
  height,
  hostMode,
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
        interactionScope,
        x,
        y,
        width,
        height,
        hostMode,
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
    hostMode,
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
