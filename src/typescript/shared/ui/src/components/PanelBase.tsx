import { type ReactNode, type CSSProperties } from 'react';
import { composePanelStages, type PanelStageStyles } from './PanelPipeline';

export type PanelBaseStage = 'container' | 'header' | 'headerAction' | 'content' | 'footer';

export type PanelBaseProps = {
    readonly title?: string;
    readonly children: ReactNode;
    readonly variant?: 'default' | 'compact' | 'wide';
    readonly headerAction?: ReactNode;
    readonly footer?: ReactNode;
    readonly isScrollable?: boolean;
    readonly padding?: string;
    readonly gap?: string;
    readonly className?: string;
    readonly stageStyles?: PanelStageStyles<PanelBaseStage>;
};

/**
 * PanelBase is the foundation component for all resizable/collapsible panels.
 * Provides consistent styling, layout, and interactive patterns.
 * All notebook and dock panels should extend or wrap this component.
 */
export function PanelBase({
    title,
    children,
    variant = 'default',
    headerAction,
    footer,
    isScrollable = true,
    padding = '8px',
    gap = '8px',
    className = '',
    stageStyles,
}: PanelBaseProps) {
    const pipelineStyles = composePanelStages<PanelBaseStage>(
        {
            container: {
                display: 'flex',
                flexDirection: 'column',
                height: '100%',
                minHeight: 0,
            },
            header: {
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'space-between',
                padding: '8px 12px',
                borderBottom: '1px solid rgba(20, 184, 166, 0.15)',
                backgroundColor: 'rgba(0, 0, 0, 0.1)',
                fontSize: '11px',
                fontWeight: 700,
                color: 'rgba(226, 232, 240, 0.8)',
                textTransform: 'uppercase',
                letterSpacing: '0.5px',
                flexShrink: 0,
            },
            headerAction: {
                display: 'flex',
                gap: '4px',
            },
            content: {
                flex: 1,
                overflow: isScrollable ? 'auto' : 'hidden',
                minHeight: 0,
                padding,
                display: 'flex',
                flexDirection: 'column',
                gap,
                fontSize: '12px',
                color: 'rgba(226, 232, 240, 0.85)',
                lineHeight: '1.5',
            },
            footer: {
                padding: '8px 12px',
                borderTop: '1px solid rgba(20, 184, 166, 0.15)',
                backgroundColor: 'rgba(0, 0, 0, 0.1)',
                fontSize: '11px',
                color: 'rgba(226, 232, 240, 0.6)',
                flexShrink: 0,
            },
        },
        stageStyles,
    );

    const variantClasses = {
        default: '',
        compact: 'px-2 py-1 text-xs',
        wide: 'px-4 py-3 text-base',
    };

    return (
        <div style={pipelineStyles.container} className={`panel-base ${variantClasses[variant]} ${className}`}>
            {title && (
                <div style={pipelineStyles.header}>
                    <span>{title}</span>
                    {headerAction && <div style={pipelineStyles.headerAction}>{headerAction}</div>}
                </div>
            )}
            <div style={pipelineStyles.content}>{children}</div>
            {footer && <div style={pipelineStyles.footer}>{footer}</div>}
        </div>
    );
}
