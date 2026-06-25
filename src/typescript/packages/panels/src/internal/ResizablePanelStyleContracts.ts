import {type CSSProperties} from 'react';

export type ResizablePanelStyleAnchorSide = 'left'|'right'|'top'|'bottom';

export type ResizablePanelStyleGeometryInput = {
  x: number; y: number; width: number; height: number;
  zIndex?: number;
  hostMode?: 'viewport' | 'container';
};

export type ResizablePanelStyleStateInput = {
  isDragging: boolean; isResizing: boolean;
};

export type ResizablePanelStyleDockInput = {
  isAnchored: boolean;
  groupColor?: string; isGroupResizeLocked: boolean;
};

export type BuildResizablePanelStylesOptions = ResizablePanelStyleGeometryInput&
    ResizablePanelStyleStateInput&ResizablePanelStyleDockInput;

export type ResizablePanelSurfaceStyles = {
  containerStyle: CSSProperties; headerStyle: CSSProperties;
  contentStyle: CSSProperties;
};

export type ResizablePanelHandleStyles = {
  leftHandleStyle: CSSProperties; rightHandleStyle: CSSProperties;
  topHandleStyle: CSSProperties;
  bottomHandleStyle: CSSProperties;
  cornerGripStyle: CSSProperties;
};

export type ResizablePanelControlStyles = {
  unlinkButtonStyle: CSSProperties; lockResizeButtonStyle: CSSProperties;
  sideBadgeContainerStyle: CSSProperties;
  sideBadgeStyle: (side: ResizablePanelStyleAnchorSide) => CSSProperties;
};

export type ResizablePanelStyles = ResizablePanelSurfaceStyles&
    ResizablePanelHandleStyles&ResizablePanelControlStyles;
