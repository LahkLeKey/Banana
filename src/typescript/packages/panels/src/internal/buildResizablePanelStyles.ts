import {type CSSProperties} from 'react';

import {type BuildResizablePanelStylesOptions, type ResizablePanelStyleAnchorSide, type ResizablePanelStyles,} from './ResizablePanelStyleContracts';

export function buildResizablePanelStyles({
  x,
  y,
  width,
  height,
  zIndex,
  isDragging,
  isResizing,
  isAnchored,
  groupColor,
  isGroupResizeLocked,
}: BuildResizablePanelStylesOptions): ResizablePanelStyles {
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
    willChange: isDragging || isResizing ? 'transform, width, height' :
                                           undefined,
    contain: 'layout paint style',
    boxShadow: isAnchored ?
        `0 0 0 1px ${
            groupColor ??
            'rgba(34, 211, 238, 0.65)'}, 0 12px 24px rgba(8, 47, 73, 0.35)` :
        undefined,
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
    background:
        'linear-gradient(135deg, transparent 0 36%, rgba(148, 163, 184, 0.65) 36% 42%, transparent 42% 56%, rgba(148, 163, 184, 0.65) 56% 62%, transparent 62% 76%, rgba(148, 163, 184, 0.65) 76% 82%, transparent 82% 100%)',
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

  const sideBadgeStyle =
      (side: ResizablePanelStyleAnchorSide): CSSProperties => {
        void side;
        return {
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
        };
      };

  return {
    containerStyle,
    headerStyle,
    contentStyle,
    leftHandleStyle,
    rightHandleStyle,
    topHandleStyle,
    bottomHandleStyle,
    cornerGripStyle,
    unlinkButtonStyle,
    lockResizeButtonStyle,
    sideBadgeContainerStyle,
    sideBadgeStyle,
  };
}
