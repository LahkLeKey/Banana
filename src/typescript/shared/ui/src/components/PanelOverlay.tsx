import { type ReactNode, type CSSProperties } from 'react';

export type PanelOverlayProps = {
    readonly isOpen: boolean;
    readonly title?: string;
    readonly children: ReactNode;
    readonly onClose: () => void;
    readonly maxWidth?: string;
    readonly maxHeight?: string;
};

export function PanelOverlay({
    isOpen,
    title,
    children,
    onClose,
    maxWidth = '70vw',
    maxHeight = '80dvh',
}: PanelOverlayProps) {
    if (!isOpen) return null;

    const backdropStyle: CSSProperties = {
        position: 'fixed',
        inset: 0,
        background: 'rgba(0, 0, 0, 0.6)',
        backdropFilter: 'blur(4px)',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        zIndex: 999,
        animation: 'fadeIn 0.2s ease-out',
    };

    const modalStyle: CSSProperties = {
        background: 'radial-gradient(circle at 20% 15%, rgba(14, 165, 233, 0.08), transparent 40%), linear-gradient(155deg, rgba(7, 19, 34, 0.95) 0%, rgba(10, 27, 45, 0.95) 100%)',
        border: '1px solid rgba(20, 184, 166, 0.25)',
        borderRadius: '8px',
        boxShadow: '0 20px 60px rgba(0, 0, 0, 0.8), inset 0 1px 0 rgba(20, 184, 166, 0.1)',
        maxWidth,
        maxHeight,
        width: '90vw',
        height: '90dvh',
        display: 'flex',
        flexDirection: 'column',
        animation: 'slideUp 0.3s ease-out',
        overflow: 'hidden',
    };

    const headerStyle: CSSProperties = {
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'space-between',
        padding: '16px 20px',
        borderBottom: '1px solid rgba(20, 184, 166, 0.15)',
        backgroundColor: 'rgba(0, 0, 0, 0.2)',
    };

    const titleStyle: CSSProperties = {
        fontSize: '14px',
        fontWeight: 700,
        color: 'rgba(226, 232, 240, 0.9)',
        textTransform: 'uppercase',
        letterSpacing: '0.5px',
    };

    const closeButtonStyle: CSSProperties = {
        background: 'none',
        border: 'none',
        color: 'rgba(226, 232, 240, 0.6)',
        fontSize: '20px',
        cursor: 'pointer',
        padding: '4px 8px',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        transition: 'color 0.2s',
    };

    const contentStyle: CSSProperties = {
        flex: 1,
        overflow: 'auto',
        padding: '16px 20px',
        color: 'rgba(226, 232, 240, 0.85)',
        fontSize: '13px',
        lineHeight: '1.6',
    };

    const styleSheet = document.createElement('style');
    styleSheet.textContent = `
        @keyframes fadeIn {
            from { opacity: 0; }
            to { opacity: 1; }
        }
        @keyframes slideUp {
            from { 
                transform: translateY(20px); 
                opacity: 0;
            }
            to { 
                transform: translateY(0); 
                opacity: 1;
            }
        }
    `;
    if (!document.head.querySelector('style[data-panel-overlay]')) {
        styleSheet.setAttribute('data-panel-overlay', '');
        document.head.appendChild(styleSheet);
    }

    return (
        <div style={backdropStyle} onClick={onClose}>
            <div
                style={modalStyle}
                onClick={(e) => e.stopPropagation()}
            >
                {title && (
                    <div style={headerStyle}>
                        <span style={titleStyle}>{title}</span>
                        <button
                            onClick={onClose}
                            style={closeButtonStyle}
                            onMouseEnter={(e) => {
                                if (e.currentTarget) e.currentTarget.style.color = 'rgba(226, 232, 240, 0.9)';
                            }}
                            onMouseLeave={(e) => {
                                if (e.currentTarget) e.currentTarget.style.color = 'rgba(226, 232, 240, 0.6)';
                            }}
                            title="Close overlay"
                        >
                            ×
                        </button>
                    </div>
                )}
                <div style={contentStyle}>{children}</div>
            </div>
        </div>
    );
}
