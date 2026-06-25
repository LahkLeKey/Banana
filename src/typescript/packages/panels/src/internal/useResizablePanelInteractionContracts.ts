import {type RefObject} from 'react';

import {type DragSnapAnchorSide} from './usePanelDragSnap';

type PanelMoveHandler =
    (panelId: string, nextPosition: {x: number; y: number},
     phase: 'move'|'end') => void;

type PanelResizeHandler =
    (panelId: string,
     rect: {x: number; y: number; width: number; height: number}) => void;

export type ResizablePanelInteractionIdentityInput = {
  id: string;
  interactionScope?: string;
};

export type ResizablePanelInteractionGeometryInput = {
  x: number; y: number; width: number; height: number;
  hostMode?: 'viewport' | 'container';
};

export type ResizablePanelInteractionConstraintsInput = {
  minWidth: number; minHeight: number;
};

export type ResizablePanelInteractionGroupingInput = {
  groupSize: number; snapIgnoreIds: readonly string[];
};

export type ResizablePanelInteractionRefsInput = {
  containerRef: RefObject<HTMLElement|null>;
};

export type ResizablePanelInteractionCallbacks = {
  onMove?: PanelMoveHandler;
  onResize?: PanelResizeHandler;
  onAnchorCommit?: (targetId: string, sourceSide: DragSnapAnchorSide) => void;
  onFocus?: () => void;
};

export type UseResizablePanelInteractionsOptions =
    ResizablePanelInteractionIdentityInput&
    ResizablePanelInteractionGeometryInput&
    ResizablePanelInteractionConstraintsInput&
    ResizablePanelInteractionGroupingInput&ResizablePanelInteractionRefsInput&
    ResizablePanelInteractionCallbacks;
