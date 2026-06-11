import { type ReactNode, type CSSProperties } from 'react';

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
}: PanelBaseProps) {
    const containerStyle: CSSProperties = {
        display: 'flex',
        flexDirection: 'column',
        height: '100%',
        minHeight: 0,
    };

    const headerStyle: CSSProperties = {
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
    };

    const contentStyle: CSSProperties = {
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
    };

    const footerStyle: CSSProperties = {
        padding: '8px 12px',
        borderTop: '1px solid rgba(20, 184, 166, 0.15)',
        backgroundColor: 'rgba(0, 0, 0, 0.1)',
        fontSize: '11px',
        color: 'rgba(226, 232, 240, 0.6)',
        flexShrink: 0,
    };

    const variantClasses = {
        default: '',
        compact: 'px-2 py-1 text-xs',
        wide: 'px-4 py-3 text-base',
    };

    return (
        <div style={containerStyle} className={`panel-base ${variantClasses[variant]} ${className}`}>
            {title && (
                <div style={headerStyle}>
                    <span>{title}</span>
                    {headerAction && <div style={{ display: 'flex', gap: '4px' }}>{headerAction}</div>}
                </div>
            )}
            <div style={contentStyle}>{children}</div>
            {footer && <div style={footerStyle}>{footer}</div>}
        </div>
    );
}
