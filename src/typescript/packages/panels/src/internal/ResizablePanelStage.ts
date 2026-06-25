import {type CSSProperties, type HTMLAttributes} from 'react';

import type {PanelIntrinsicElement} from './PanelIntrinsicElement';

export type ResizablePanelStage = 'container'|'header'|'content';
export type ResizablePanelStageElements =
    Partial<Record<ResizablePanelStage, PanelIntrinsicElement>>;
export type ResizablePanelStageElementProps =
    Partial<Record<ResizablePanelStage, HTMLAttributes<HTMLElement>>>;

export function resolveResizablePanelStageElements(
    stageElements?: ResizablePanelStageElements):
    Record<ResizablePanelStage, PanelIntrinsicElement> {
  return {
    container: 'div',
    header: 'div',
    content: 'div',
    ...stageElements,
  };
}

export function getResizablePanelStageProps(
    stage: ResizablePanelStage, baseProps: HTMLAttributes<HTMLElement>,
    stageElementProps?: ResizablePanelStageElementProps):
    HTMLAttributes<HTMLElement> {
  const providedProps = stageElementProps?.[stage];
  if (!providedProps) {
    return baseProps;
  }

  const mergedClassName =
      [baseProps.className, providedProps.className].filter(Boolean).join(
          ' ') ||
      undefined;

  const mergedStyle = {
    ...(baseProps.style as CSSProperties | undefined),
    ...(providedProps.style as CSSProperties | undefined),
  };

  return {
    ...baseProps,
    ...providedProps,
    className: mergedClassName,
    style: mergedStyle,
  };
}
