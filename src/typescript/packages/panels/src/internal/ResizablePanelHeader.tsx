import { type CSSProperties, type HTMLAttributes, type MouseEvent as ReactMouseEvent } from 'react';

import type { PanelIntrinsicElement } from './PanelIntrinsicElement';

type AnchorSide = 'left' | 'right' | 'top' | 'bottom';

type AnchorBadgesProps = {
    id: string;
    anchorSides: readonly AnchorSide[];
    sideBadgeContainerStyle: CSSProperties;
    sideBadgeStyle: (side: AnchorSide) => CSSProperties;
};

function AnchorBadges(
    { id, anchorSides, sideBadgeContainerStyle, sideBadgeStyle }: AnchorBadgesProps) {
    if (anchorSides.length === 0) {
        return null;
    }

    return (
        <span style={sideBadgeContainerStyle}>
            {anchorSides.map((side) => (
                <span key={`${id}-${side}`} style={sideBadgeStyle(side)}>
                    {side.charAt(0)}
                </span>
            ))}
        </span>
    );
}

type PanelHeaderActionsProps = {
    isAnchored: boolean;
    groupSize: number;
    isGroupResizeLocked: boolean;
    onUnlinkAll?: () => void;
    onToggleGroupResizeLock?: () => void;
    onExpand?: () => void;
    onCollapse?: () => void;
    unlinkButtonStyle: CSSProperties;
    lockResizeButtonStyle: CSSProperties;
};

function PanelHeaderActions({
    isAnchored,
    groupSize,
    isGroupResizeLocked,
    onUnlinkAll,
    onToggleGroupResizeLock,
    onExpand,
    onCollapse,
    unlinkButtonStyle,
    lockResizeButtonStyle,
}: PanelHeaderActionsProps) {
    return (
        <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
            {isAnchored && onUnlinkAll ? (
                <button
                    onClick={onUnlinkAll}
                    style={unlinkButtonStyle}
                    title={`Unlink ${groupSize > 1 ? `group (${groupSize})` : 'panel'}`}>
                    ⛓
                </button>
            ) : null}
            {isAnchored && onToggleGroupResizeLock ? (
                <button
                    onClick={onToggleGroupResizeLock}
                    style={lockResizeButtonStyle}
                    title={isGroupResizeLocked ?
                        'Unlock group resize' :
                        'Lock group resize'}>
                    {isGroupResizeLocked ? '⇲' : '⇱'}
                </button>
            ) : null}
            {onExpand ? (
                <button
                    onClick={onExpand}
                    style={{
                        background: 'none',
                        border: 'none',
                        color: 'rgba(148, 163, 184, 0.8)',
                        cursor: 'pointer',
                        padding: '4px 6px',
                        display: 'flex',
                        alignItems: 'center',
                        justifyContent: 'center',
                        fontSize: '14px',
                        lineHeight: 1,
                    }}
                    title="Expand panel">
                    □
                </button>
            ) : null}
            {onCollapse ? (
                <button
                    onClick={onCollapse}
                    style={{
                        background: 'none',
                        border: 'none',
                        color: 'rgba(226, 232, 240, 0.5)',
                        cursor: 'pointer',
                        padding: '4px 6px',
                        display: 'flex',
                        alignItems: 'center',
                        justifyContent: 'center',
                        fontSize: '14px',
                    }}
                    title="Collapse panel">
                    ×
                </button>
            ) : null}
        </div>
    );
}

export type ResizablePanelHeaderProps = {
    id: string;
    title: string;
    titleElementId?: string;
    anchorSides: readonly AnchorSide[];
    isAnchored: boolean;
    groupSize: number;
    isGroupResizeLocked: boolean;
    onHeaderMouseDown: (event: ReactMouseEvent<HTMLElement>) => void;
    onUnlinkAll?: () => void;
    onToggleGroupResizeLock?: () => void;
    onExpand?: () => void;
    onCollapse?: () => void;
    sideBadgeContainerStyle: CSSProperties;
    sideBadgeStyle: (side: AnchorSide) => CSSProperties;
    unlinkButtonStyle: CSSProperties;
    lockResizeButtonStyle: CSSProperties;
    getHeaderProps: (baseProps: HTMLAttributes<HTMLElement>) => HTMLAttributes<HTMLElement>;
    HeaderElement: PanelIntrinsicElement;
};

export function ResizablePanelHeader({
    id,
    title,
    titleElementId,
    anchorSides,
    isAnchored,
    groupSize,
    isGroupResizeLocked,
    onHeaderMouseDown,
    onUnlinkAll,
    onToggleGroupResizeLock,
    onExpand,
    onCollapse,
    sideBadgeContainerStyle,
    sideBadgeStyle,
    unlinkButtonStyle,
    lockResizeButtonStyle,
    getHeaderProps,
    HeaderElement,
}: ResizablePanelHeaderProps) {
    return (
        <HeaderElement {...getHeaderProps({ onMouseDown: onHeaderMouseDown })}>
            <span id={titleElementId} style={{ display: 'inline-flex', alignItems: 'center' }}>
                {title}
                <AnchorBadges
                    id={id}
                    anchorSides={anchorSides}
                    sideBadgeContainerStyle={sideBadgeContainerStyle}
                    sideBadgeStyle={sideBadgeStyle}
                />
            </span>
            <PanelHeaderActions
                isAnchored={isAnchored}
                groupSize={groupSize}
                isGroupResizeLocked={isGroupResizeLocked}
                onUnlinkAll={onUnlinkAll}
                onToggleGroupResizeLock={onToggleGroupResizeLock}
                onExpand={onExpand}
                onCollapse={onCollapse}
                unlinkButtonStyle={unlinkButtonStyle}
                lockResizeButtonStyle={lockResizeButtonStyle}
            />
        </HeaderElement>
    );
}
