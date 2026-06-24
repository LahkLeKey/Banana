import { useCallback, useMemo, useRef, type ReactNode } from 'react';
import { buildPanelBehaviorPipeline } from './PanelVariantPipeline';
import { CollapsedLauncher, ResizeHandles, type ResizeDirection } from './ResizablePanelChrome';
import { ResizablePanelHeader } from './ResizablePanelHeader';
import { ResizablePanelHost } from './ResizablePanelHost';
import { assignPanelContainerRef } from './ResizablePanelRefs';
import {
    type ResizablePanelStageElementProps,
    type ResizablePanelStageElements,
} from './ResizablePanelStage';
import { useResizablePanelInteractions } from './useResizablePanelInteractions';
import { useResizablePanelViewModel } from './useResizablePanelViewModel';

export type AnchorSide = 'left' | 'right' | 'top' | 'bottom';

type PanelMoveHandler = (panelId: string, nextPosition: { x: number; y: number }, phase: 'move' | 'end') => void;
type PanelResizeHandler = (panelId: string, rect: { x: number; y: number; width: number; height: number }) => void;

export type ResizablePanelIdentityProps = {
    readonly id: string;
    readonly children: ReactNode;
    readonly title?: string;
    readonly titleElementId?: string;
};

export type ResizablePanelLayoutProps = {
    readonly x: number;
    readonly y: number;
    readonly width: number;
    readonly height: number;
    readonly minWidth?: number;
    readonly minHeight?: number;
    readonly zIndex?: number;
    readonly isCollapsed?: boolean;
    readonly corner?: 'top-left' | 'top-right' | 'bottom-left' | 'bottom-right';
};

export type ResizablePanelInteractionProps = {
    readonly onMove?: PanelMoveHandler;
    readonly onResize?: PanelResizeHandler;
    readonly onCollapse?: () => void;
    readonly onExpand?: () => void;
    readonly onFocus?: () => void;
};

export type ResizablePanelDockingProps = {
    readonly onUnlinkAll?: () => void;
    readonly onAnchorCommit?: (targetId: string, sourceSide: AnchorSide) => void;
    readonly snapIgnoreIds?: readonly string[];
    readonly isAnchored?: boolean;
    readonly groupSize?: number;
    readonly anchorSides?: readonly AnchorSide[];
    readonly groupColor?: string;
    readonly isGroupResizeLocked?: boolean;
    readonly onToggleGroupResizeLock?: () => void;
};

export type ResizablePanelStageOverrides = {
    readonly stageElements?: ResizablePanelStageElements;
    readonly stageElementProps?: ResizablePanelStageElementProps;
};

export type ResizablePanelProps =
    ResizablePanelIdentityProps &
    ResizablePanelLayoutProps &
    ResizablePanelInteractionProps &
    ResizablePanelDockingProps &
    ResizablePanelStageOverrides;

export function ResizablePanel({
    id,
    children,
    title,
    x,
    y,
    width,
    height,
    minWidth = 280,
    minHeight = 200,
    onMove,
    onResize,
    onCollapse,
    onExpand,
    onFocus,
    onUnlinkAll,
    onAnchorCommit,
    snapIgnoreIds = [],
    isAnchored = false,
    groupSize = 1,
    anchorSides = [],
    groupColor,
    isGroupResizeLocked = false,
    onToggleGroupResizeLock,
    isCollapsed = false,
    zIndex,
    stageElements,
    stageElementProps,
    titleElementId,
}: ResizablePanelProps) {
    const containerRef = useRef<HTMLElement>(null);
    const behaviorPipeline = useMemo(
        () => buildPanelBehaviorPipeline({
            movable: Boolean(onMove),
            resizable: Boolean(onResize),
            dockable: Boolean(onAnchorCommit),
            modal: false,
        }),
        [onAnchorCommit, onMove, onResize],
    );

    const interactionOptions = {
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
    };

    const {
        isDragging,
        ghostRect,
        handleHeaderMouseDown,
        activeResize,
        startResize,
    } = useResizablePanelInteractions(interactionOptions);

    const viewModelOptions = {
        x,
        y,
        width,
        height,
        zIndex,
        isDragging,
        isResizing: Boolean(activeResize),
        isAnchored,
        groupColor,
        isGroupResizeLocked,
        stageElements,
        stageElementProps,
    };

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
        ContainerElement,
        HeaderElement,
        ContentElement,
        getStageProps,
    } = useResizablePanelViewModel(viewModelOptions);
    const setContainerRef = useCallback((node: Element | null) => {
        assignPanelContainerRef(containerRef, node);
    }, []);

    const containerStageProps = getStageProps('container', {
        style: containerStyle,
        onMouseDownCapture: onFocus,
    });

    const contentStageProps = getStageProps('content', {
        style: contentStyle,
    });

    const headerBaseProps = {
        id,
        titleElementId,
        anchorSides,
        isAnchored,
        groupSize,
        isGroupResizeLocked,
        onHeaderMouseDown: handleHeaderMouseDown,
        onUnlinkAll,
        onToggleGroupResizeLock,
        onExpand,
        onCollapse,
        sideBadgeContainerStyle,
        sideBadgeStyle,
        unlinkButtonStyle,
        lockResizeButtonStyle,
        HeaderElement,
    };

    const resizeHandlesProps = {
        leftHandleStyle,
        rightHandleStyle,
        topHandleStyle,
        bottomHandleStyle,
        cornerGripStyle,
        onStartResize: startResize,
    };

    if (isCollapsed) {
        return <CollapsedLauncher id={id} x={x} y={y} onExpand={onExpand} />;
    }

    return (
        <ResizablePanelHost isDragging={isDragging} ghostRect={ghostRect} zIndex={zIndex}>
            <ContainerElement
                ref={setContainerRef}
                data-resizable-panel="true"
                data-panel-id={id}
                data-panel-behavior-pipeline={behaviorPipeline.layers.join('>')}
                {...containerStageProps}
            >
                {title ? (
                    <ResizablePanelHeader
                        {...headerBaseProps}
                        title={title}
                        getHeaderProps={(baseProps) =>
                            getStageProps('header', {
                                style: headerStyle,
                                ...baseProps,
                            })}
                    />
                ) : null}
                <ContentElement {...contentStageProps}>
                    {children}
                </ContentElement>
                <ResizeHandles {...resizeHandlesProps} />
            </ContainerElement>
        </ResizablePanelHost>
    );
}
