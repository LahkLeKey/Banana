import {type ResizablePanelStageElementProps, type ResizablePanelStageElements} from './ResizablePanelStage';

export type ResizablePanelViewModelGeometryInput = {
  x: number; y: number; width: number; height: number;
  zIndex?: number;
  hostMode?: 'viewport' | 'container';
};

export type ResizablePanelViewModelStateInput = {
  isDragging: boolean; isResizing: boolean;
};

export type ResizablePanelViewModelDockInput = {
  isAnchored: boolean;
  groupColor?: string; isGroupResizeLocked: boolean;
};

export type ResizablePanelViewModelStageInput = {
  stageElements?: ResizablePanelStageElements;
  stageElementProps?: ResizablePanelStageElementProps;
};

export type UseResizablePanelViewModelOptions =
    ResizablePanelViewModelGeometryInput&ResizablePanelViewModelStateInput&
    ResizablePanelViewModelDockInput&ResizablePanelViewModelStageInput;
