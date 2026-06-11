import { useRef, useEffect, useState, type ReactNode, type CSSProperties } from 'react';

export type ResizablePanelProps = {
    readonly id: string;
    readonly children: ReactNode;
    readonly title?: string;
    readonly minWidth?: number;
    readonly minHeight?: number;
    readonly defaultWidth?: number;
    readonly defaultHeight?: number;
    readonly onResize?: (width: number, height: number) => void;
    readonly onCollapse?: () => void;
    readonly onExpand?: () => void;
    readonly isCollapsed?: boolean;
    readonly corner?: 'top-left' | 'top-right' | 'bottom-left' | 'bottom-right';
};

export function ResizablePanel({
    id,
    children,
    title,
    minWidth = 280,
    minHeight = 200,
    defaultWidth = 360,
    defaultHeight = 400,
    onResize,
    onCollapse,
    onExpand,
    isCollapsed = false,
    corner = 'top-left',
}: ResizablePanelProps) {
    const containerRef = useRef<HTMLDivElement>(null);
    const [width, setWidth] = useState(defaultWidth);
    const [height, setHeight] = useState(defaultHeight);
    const [isResizing, setIsResizing] = useState<'horizontal' | 'vertical' | null>(null);

    const startResize = (direction: 'horizontal' | 'vertical') => {
        setIsResizing(direction);
    };

    const getViewportLimits = () => {
        const horizontalMargin = 40;
        const verticalMargin = 140;
        const maxWidth = Math.max(minWidth, window.innerWidth - horizontalMargin);
        const maxHeight = Math.max(minHeight, window.innerHeight - verticalMargin);
        return { maxWidth, maxHeight };
    };

    useEffect(() => {
        if (!isResizing || !containerRef.current) return;

        const handleMouseMove = (e: MouseEvent) => {
            e.preventDefault();

            const rect = containerRef.current?.getBoundingClientRect();
            if (!rect) {
                return;
            }

            const { maxWidth, maxHeight } = getViewportLimits();

            if (isResizing === 'horizontal') {
                const isRight = corner === 'top-right' || corner === 'bottom-right';
                const newWidth = isRight
                    ? rect.right - e.clientX
                    : e.clientX - rect.left;
                const constrainedWidth = Math.max(minWidth, Math.min(maxWidth, newWidth));
                setWidth(constrainedWidth);
                onResize?.(constrainedWidth, height);
            } else if (isResizing === 'vertical') {
                const isBottom = corner === 'bottom-left' || corner === 'bottom-right';
                const newHeight = isBottom
                    ? rect.bottom - e.clientY
                    : e.clientY - rect.top;
                const constrainedHeight = Math.max(minHeight, Math.min(maxHeight, newHeight));
                setHeight(constrainedHeight);
                onResize?.(width, constrainedHeight);
            }
        };

        const handleMouseUp = () => {
            setIsResizing(null);
        };

        document.addEventListener('mousemove', handleMouseMove);
        document.addEventListener('mouseup', handleMouseUp);

        return () => {
            document.removeEventListener('mousemove', handleMouseMove);
            document.removeEventListener('mouseup', handleMouseUp);
        };
    }, [isResizing, width, height, minWidth, minHeight, onResize, corner]);

    const containerStyle: CSSProperties = {
        width,
        height,
        display: 'flex',
        flexDirection: 'column',
        background: 'rgba(7, 19, 34, 0.85)',
        border: '1px solid rgba(20, 184, 166, 0.2)',
        borderRadius: '6px',
        overflow: 'hidden',
        position: 'relative',
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
        cursor: 'grab',
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
        cursor: isResizing === 'horizontal' ? 'ew-resize' : 'ns-resize',
        zIndex: 10,
    };

    const isRight = corner === 'top-right' || corner === 'bottom-right';
    const isBottom = corner === 'bottom-left' || corner === 'bottom-right';

    const horizontalHandleStyle: CSSProperties = {
        ...resizeHandleStyle,
        ...(isRight ? { left: 0 } : { right: 0 }),
        top: 0,
        bottom: 0,
        width: '4px',
    };

    const verticalHandleStyle: CSSProperties = {
        ...resizeHandleStyle,
        ...(isBottom ? { top: 0 } : { bottom: 0 }),
        left: 0,
        right: 0,
        height: '4px',
    };

    if (isCollapsed) {
        return (
            <div style={{
                width: '40px',
                height: '40px',
                background: 'rgba(7, 19, 34, 0.9)',
                border: '1px solid rgba(20, 184, 166, 0.2)',
                borderRadius: '4px',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'center',
                cursor: 'pointer',
                fontSize: '11px',
                color: 'rgba(226, 232, 240, 0.6)',
                overflow: 'hidden',
            }}>
                {id.charAt(0).toUpperCase()}
            </div>
        );
    }

    return (
        <div ref={containerRef} style={containerStyle}>
            {title && (
                <div style={headerStyle}>
                    <span>{title}</span>
                    <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
                        {onExpand && (
                            <button
                                onClick={onExpand}
                                style={{
                                    background: 'none',
                                    border: 'none',
                                    color: 'rgba(148, 163, 184, 0.8)',
                                    cursor: 'pointer',
                                    padding: '2px 4px',
                                    display: 'flex',
                                    alignItems: 'center',
                                    justifyContent: 'center',
                                    fontSize: '11px',
                                    lineHeight: 1,
                                }}
                                title="Expand panel"
                            >
                                □
                            </button>
                        )}
                        {onCollapse && (
                            <button
                                onClick={onCollapse}
                                style={{
                                    background: 'none',
                                    border: 'none',
                                    color: 'rgba(226, 232, 240, 0.5)',
                                    cursor: 'pointer',
                                    padding: '2px',
                                    display: 'flex',
                                    alignItems: 'center',
                                    justifyContent: 'center',
                                }}
                                title="Collapse panel"
                            >
                                ×
                            </button>
                        )}
                    </div>
                </div>
            )}
            <div style={contentStyle}>{children}</div>
            <div
                style={horizontalHandleStyle}
                onMouseDown={() => startResize('horizontal')}
                onTouchStart={() => startResize('horizontal')}
            />
            <div
                style={verticalHandleStyle}
                onMouseDown={() => startResize('vertical')}
                onTouchStart={() => startResize('vertical')}
            />
        </div>
    );
}
