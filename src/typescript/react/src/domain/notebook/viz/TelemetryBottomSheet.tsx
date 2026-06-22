import type { ReactNode } from 'react';

type TelemetryBottomSheetProps = {
    readonly visible: boolean;
    readonly mobile: boolean;
    readonly overlayTop: number;
    readonly compact: boolean;
    readonly onClose?: () => void;
    readonly children: ReactNode;
};

export function TelemetryBottomSheet(props: TelemetryBottomSheetProps) {
    const { visible, mobile, overlayTop, compact, onClose, children } = props;

    if (!visible) {
        return null;
    }

    const style: React.CSSProperties = {
        position: 'absolute',
        right: mobile ? 8 : compact ? 16 : 60,
        left: mobile ? 8 : 'auto',
        top: mobile ? 'auto' : compact ? overlayTop + 122 : overlayTop,
        bottom: mobile ? 8 : 'auto',
        width: mobile ? 'auto' : 'min(340px, calc(100vw - 40px))',
        maxHeight: mobile ? '40vh' : 'none',
        overflowY: mobile ? 'auto' : 'visible',
        borderRadius: 12,
        border: '1px solid rgba(45, 212, 191, 0.24)',
        background: 'rgba(2, 10, 20, 0.72)',
        backdropFilter: 'blur(5px)',
        padding: 8,
        display: 'grid',
        gap: 8,
        pointerEvents: 'auto',
    } as const;

    const headerStyle: React.CSSProperties = {
        display: 'flex',
        justifyContent: 'space-between',
        alignItems: 'center',
        paddingBottom: 4,
        borderBottom: '1px solid rgba(148, 163, 184, 0.1)',
    };

    const labelStyle: React.CSSProperties = {
        fontSize: 10,
        color: '#94a3b8',
        textTransform: 'uppercase',
        letterSpacing: '0.06em',
    };

    const closeButtonStyle: React.CSSProperties = {
        background: 'none',
        border: 'none',
        color: '#94a3b8',
        cursor: 'pointer',
        fontSize: 14,
        padding: '0 4px',
    };

    return (
        <div style={style}>
            {mobile && onClose ? (
                <div style={headerStyle}>
                    <span style={labelStyle}>Details</span>
                    <button
                        type="button"
                        onClick={onClose}
                        style={closeButtonStyle}
                        aria-label="Close"
                    >
                        ×
                    </button>
                </div>
            ) : null}
            {children}
        </div>
    );
}
