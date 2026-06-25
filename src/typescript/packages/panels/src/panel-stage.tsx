import type { CSSProperties, ReactNode } from 'react';

export type PanelStageProps = {
    readonly children: ReactNode;
    readonly className?: string;
    readonly style?: CSSProperties;
    readonly minHeight?: number | string;
    readonly padded?: boolean;
    readonly clipped?: boolean;
    readonly role?: string;
    readonly label?: string;
};

export function PanelStage({
    children,
    className,
    style,
    minHeight = '100%',
    padded = true,
    clipped = true,
    role = 'region',
    label = 'Panel stage',
}: PanelStageProps) {
    const mergedStyle: CSSProperties = {
        position: 'absolute',
        inset: 0,
        minHeight,
        overflow: clipped ? 'hidden' : 'visible',
        padding: padded ? 8 : 0,
        pointerEvents: 'auto',
        ...style,
    };

    return (
        <div
            data-panel-stage="true"
            role={role}
            aria-label={label}
            className={className}
            style={mergedStyle}
        >
            {children}
        </div>
    );
}
