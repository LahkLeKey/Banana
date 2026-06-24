import {type MouseEvent as ReactMouseEvent, type RefObject, useEffect, useRef, useState} from 'react';

import type {RafBufferedDispatch} from './useRafBufferedDispatch';

type ResizeDirection = 'left'|'right'|'top'|'bottom'|'bottom-right';

type InteractionOrigin = {
  mouseX: number; mouseY: number; x: number; y: number; width: number;
  height: number;
  panelRect: DOMRect;
};

type UsePanelResizeOptions = {
  x: number; y: number; width: number; height: number; minWidth: number;
  minHeight: number;
  containerRef: RefObject<HTMLElement|null>;
  clearDragState: () => void;
  onFocus?: () => void;
  resizeDispatch: Pick<
      RafBufferedDispatch<
          {x: number; y: number; width: number; height: number}>,
      'schedule'|'flush'>;
};

type UsePanelResizeResult = {
  activeResize: ResizeDirection|null;
  startResize:
      (direction: ResizeDirection, event: ReactMouseEvent<HTMLDivElement>) =>
          void;
};

export function usePanelResize({
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
}: UsePanelResizeOptions): UsePanelResizeResult {
  const [activeResize, setActiveResize] = useState<ResizeDirection|null>(null);
  const resizeOriginRef = useRef<InteractionOrigin|null>(null);
  const viewportPadding = 8;

  const startResize =
      (direction: ResizeDirection, event: ReactMouseEvent<HTMLDivElement>) => {
        event.preventDefault();
        event.stopPropagation();
        clearDragState();
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

  useEffect(
      () => {
        if (!activeResize || !containerRef.current) {
          return;
        }

        const resizeOrigin = resizeOriginRef.current as InteractionOrigin;

        const handleMouseMove = (event: MouseEvent) => {
          event.preventDefault();

          const maxWidth =
              Math.max(minWidth, window.innerWidth - (viewportPadding * 2));
          const maxHeight =
              Math.max(minHeight, window.innerHeight - (viewportPadding * 2));
          const dx = event.clientX - resizeOrigin.mouseX;
          const dy = event.clientY - resizeOrigin.mouseY;

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
            nextWidth = Math.max(
                minWidth, Math.min(maxWidth, candidateRight - originLeft));
          }

          if (activeResize === 'left') {
            const minLeft = viewportPadding;
            const maxLeft = originRight - minWidth;
            nextLeft = Math.max(minLeft, Math.min(maxLeft, originLeft + dx));
            nextWidth =
                Math.max(minWidth, Math.min(maxWidth, originRight - nextLeft));
            nextX = resizeOrigin.x + (nextLeft - originLeft);
          }

          if (activeResize === 'bottom' || activeResize === 'bottom-right') {
            const maxBottom = window.innerHeight - viewportPadding;
            const candidateBottom = Math.min(maxBottom, originBottom + dy);
            nextHeight = Math.max(
                minHeight, Math.min(maxHeight, candidateBottom - originTop));
          }

          if (activeResize === 'top') {
            const minTop = viewportPadding;
            const maxTop = originBottom - minHeight;
            nextTop = Math.max(minTop, Math.min(maxTop, originTop + dy));
            nextHeight = Math.max(
                minHeight, Math.min(maxHeight, originBottom - nextTop));
            nextY = resizeOrigin.y + (nextTop - originTop);
          }

          resizeDispatch.schedule({
            x: nextX,
            y: nextY,
            width: nextWidth,
            height: nextHeight,
          });
        };

        const handleMouseUp = () => {
          resizeDispatch.flush(true);
          setActiveResize(null);
          resizeOriginRef.current = null;
          clearDragState();
        };

        document.body.style.userSelect = 'none';
        document.addEventListener('mousemove', handleMouseMove);
        document.addEventListener('mouseup', handleMouseUp);

        return () => {
          document.body.style.userSelect = '';
          document.removeEventListener('mousemove', handleMouseMove);
          document.removeEventListener('mouseup', handleMouseUp);
          resizeDispatch.flush();
        };
      },
      [
        activeResize, clearDragState, containerRef, minHeight, minWidth,
        resizeDispatch
      ]);

  return {
    activeResize,
    startResize,
  };
}
