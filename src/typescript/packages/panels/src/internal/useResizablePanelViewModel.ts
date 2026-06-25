import {type HTMLAttributes, useMemo} from 'react';

import {getResizablePanelStageProps, type ResizablePanelStage, resolveResizablePanelStageElements,} from './ResizablePanelStage';
import {buildResizablePanelStyles} from './ResizablePanelStyles';
import {type UseResizablePanelViewModelOptions} from './useResizablePanelViewModelContracts';

export {type ResizablePanelViewModelDockInput, type ResizablePanelViewModelGeometryInput, type ResizablePanelViewModelStageInput, type ResizablePanelViewModelStateInput, type UseResizablePanelViewModelOptions,} from './useResizablePanelViewModelContracts';

export function useResizablePanelViewModel({
  x,
  y,
  width,
  height,
  zIndex,
  hostMode,
  isDragging,
  isResizing,
  isAnchored,
  groupColor,
  isGroupResizeLocked,
  stageElements,
  stageElementProps,
}: UseResizablePanelViewModelOptions) {
  const {
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
  } =
      useMemo(
          () => buildResizablePanelStyles({
            x,
            y,
            width,
            height,
            zIndex,
            hostMode,
            isDragging,
            isResizing,
            isAnchored,
            groupColor,
            isGroupResizeLocked,
          }),
          [
            groupColor,
            height,
            isAnchored,
            isDragging,
            isGroupResizeLocked,
            isResizing,
            width,
            x,
            y,
            zIndex,
            hostMode,
          ]);

  const resolvedStageElements = useMemo(
      () => resolveResizablePanelStageElements(stageElements), [stageElements]);

  const ContainerElement = resolvedStageElements.container;
  const HeaderElement = resolvedStageElements.header;
  const ContentElement = resolvedStageElements.content;

  const getStageProps =
      (stage: ResizablePanelStage, baseProps: HTMLAttributes<HTMLElement>) =>
          getResizablePanelStageProps(stage, baseProps, stageElementProps);

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
    ContainerElement,
    HeaderElement,
    ContentElement,
    getStageProps,
  };
}
