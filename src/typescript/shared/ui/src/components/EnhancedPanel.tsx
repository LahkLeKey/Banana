import { type ReactNode, type CSSProperties } from 'react';
import { PanelBase, type PanelBaseProps } from './PanelBase';

export type EnhancedPanelProps = PanelBaseProps & {
    readonly headerTools?: ReactNode;
    readonly sections?: Array<{
        readonly title?: string;
        readonly content: ReactNode;
    }>;
    readonly status?: 'idle' | 'loading' | 'error' | 'success';
    readonly statusMessage?: string;
};

/**
 * EnhancedPanel builds on PanelBase to provide additional features:
 * - Section grouping with collapsible areas
 * - Status indicators and messages
 * - Header tools (buttons, controls)
 * Recommended for complex panels with multiple views/sections.
 */
export function EnhancedPanel({
    title,
    children,
    headerTools,
    sections,
    status,
    statusMessage,
    variant = 'default',
    footer,
    isScrollable = true,
    padding = '8px',
    gap = '8px',
}: Partial<EnhancedPanelProps> & Required<Pick<EnhancedPanelProps, 'title' | 'status'>>) {
    const statusColors: Record<string, string> = {
        idle: 'rgba(226, 232, 240, 0.5)',
        loading: 'rgba(20, 184, 166, 0.6)',
        error: 'rgba(244, 63, 94, 0.7)',
        success: 'rgba(34, 197, 94, 0.7)',
    };

    const statusBgColors: Record<string, string> = {
        idle: 'transparent',
        loading: 'rgba(20, 184, 166, 0.1)',
        error: 'rgba(244, 63, 94, 0.1)',
        success: 'rgba(34, 197, 94, 0.1)',
    };

    const statusMessageStyle: CSSProperties = {
        padding: '6px 10px',
        borderRadius: '4px',
        fontSize: '11px',
        color: status ? statusColors[status] : 'transparent',
        background: status ? statusBgColors[status] : 'transparent',
        borderLeft: `3px solid ${status ? statusColors[status] : 'transparent'}`,
        transition: 'all 0.2s ease',
    };

    const sectionStyle: CSSProperties = {
        display: 'flex',
        flexDirection: 'column',
        gap: '4px',
        padding: '8px',
        backgroundColor: 'rgba(7, 19, 34, 0.3)',
        borderRadius: '4px',
        borderLeft: '2px solid rgba(20, 184, 166, 0.2)',
    };

    const sectionTitleStyle: CSSProperties = {
        fontSize: '10px',
        fontWeight: 700,
        textTransform: 'uppercase',
        letterSpacing: '0.3px',
        color: 'rgba(226, 232, 240, 0.6)',
    };

    const headerActionStyle: CSSProperties = {
        display: 'flex',
        gap: '4px',
        alignItems: 'center',
    };

    const content = sections ? (
        <div style={{ display: 'flex', flexDirection: 'column', gap }}>
            {sections.map((section, idx) => (
                <div key={idx} style={sectionStyle}>
                    {section.title && <div style={sectionTitleStyle}>{section.title}</div>}
                    <div>{section.content}</div>
                </div>
            ))}
        </div>
    ) : (
        children
    );

    const statusFooter = statusMessage ? (
        <div style={statusMessageStyle}>{statusMessage}</div>
    ) : null;

    return (
        <PanelBase
            title={title}
            variant={variant}
            headerAction={
                headerTools && (
                    <div style={headerActionStyle}>{headerTools}</div>
                )
            }
            footer={footer || statusFooter}
            isScrollable={isScrollable}
            padding={padding}
            gap={gap}
        >
            {content}
        </PanelBase>
    );
}
