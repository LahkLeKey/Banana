export function TelemetryActionCluster({
    mobile,
    overlayTop,
    isBootstrapActive,
    isRelationsDisabled,
    onBootstrap,
    onRelations,
}: {
    readonly mobile: boolean;
    readonly overlayTop: number;
    readonly isBootstrapActive: boolean;
    readonly isRelationsDisabled: boolean;
    readonly onBootstrap: () => void;
    readonly onRelations: () => void;
}) {
    const buttonSize = mobile ? 40 : 34;
    const fontSize = mobile ? 14 : 12;

    return (
        <div
            style={{
                position: 'absolute',
                right: mobile ? 8 : 16,
                top: mobile ? overlayTop - 52 : overlayTop,
                bottom: 'auto',
                display: 'flex',
                flexDirection: mobile ? 'row' : 'column',
                gap: 8,
                pointerEvents: 'auto',
                zIndex: 5,
            }}
        >
            <button
                type="button"
                onClick={onBootstrap}
                style={{
                    width: buttonSize,
                    height: buttonSize,
                    borderRadius: 999,
                    border: '1px solid rgba(45, 212, 191, 0.5)',
                    background: 'rgba(2, 10, 20, 0.55)',
                    color: '#99f6e4',
                    fontSize,
                    fontWeight: 700,
                    cursor: 'pointer',
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'center',
                }}
                title="Bootstrap"
                aria-label="Bootstrap"
            >
                ▶
            </button>
            <button
                type="button"
                disabled={isRelationsDisabled}
                onClick={onRelations}
                style={{
                    width: buttonSize,
                    height: buttonSize,
                    borderRadius: 999,
                    border: '1px solid rgba(56, 189, 248, 0.5)',
                    background: 'rgba(2, 10, 20, 0.55)',
                    color: '#bae6fd',
                    fontSize,
                    fontWeight: 700,
                    cursor: isRelationsDisabled ? 'not-allowed' : 'pointer',
                    opacity: isRelationsDisabled ? 0.5 : 1,
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'center',
                }}
                title="Relations"
                aria-label="Relations"
            >
                ⟲
            </button>
        </div>
    );
}
