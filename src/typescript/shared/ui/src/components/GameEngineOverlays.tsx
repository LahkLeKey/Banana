/**
 * Error overlay displayed when viewport becomes unavailable
 */
export interface ViewportErrorOverlayProps {
    error: string;
    onReload: () => void;
}

export function ViewportErrorOverlay({ error, onReload }: ViewportErrorOverlayProps) {
    return (
        <div
            style={{
                position: "absolute",
                inset: 0,
                display: "flex",
                alignItems: "center",
                justifyContent: "center",
                padding: 20,
                background:
                    "radial-gradient(circle at 50% 35%, rgba(48,22,22,0.95) 0%, rgba(12,8,8,0.98) 75%)",
                zIndex: 1500,
            }}
        >
            <div
                style={{
                    width: "min(92vw, 560px)",
                    border: "1px solid rgba(248, 113, 113, 0.4)",
                    borderRadius: 10,
                    backgroundColor: "rgba(17, 12, 12, 0.86)",
                    boxShadow: "0 18px 36px rgba(0, 0, 0, 0.5)",
                    padding: "18px 16px",
                    color: "rgba(254, 226, 226, 1)",
                    fontFamily: "monospace",
                }}
            >
                <div style={{ fontSize: 16, fontWeight: 700, marginBottom: 8 }}>
                    Viewport unavailable
                </div>
                <div style={{ fontSize: 13, lineHeight: 1.45, opacity: 0.95, marginBottom: 12 }}>
                    {error}
                </div>
                <button
                    type="button"
                    onClick={onReload}
                    style={{
                        border: "1px solid rgba(255, 255, 255, 0.3)",
                        borderRadius: 6,
                        background: "rgba(255, 255, 255, 0.08)",
                        color: "rgba(255, 255, 255, 0.95)",
                        padding: "8px 10px",
                        fontFamily: "monospace",
                        fontSize: 12,
                        cursor: "pointer",
                    }}
                >
                    Reload viewport
                </button>
            </div>
        </div>
    );
}

/**
 * Bottom-left error notification badge
 */
export interface ErrorBadgeProps {
    message: string;
}

export function ErrorBadge({ message }: ErrorBadgeProps) {
    return (
        <div
            style={{
                position: "absolute",
                bottom: 16,
                left: 16,
                maxWidth: 320,
                padding: 12,
                backgroundColor: "rgba(30, 24, 24, 0.8)",
                border: "1px solid rgba(239, 68, 68, 0.3)",
                borderRadius: 6,
                color: "rgba(254, 226, 226, 1)",
                fontSize: 14,
                fontFamily: "monospace",
                zIndex: 1000,
            }}
        >
            {message}
        </div>
    );
}

/**
 * Controls hint displayed at top center
 */
export interface ControlsHintProps { }

export function ControlsHint({ }: ControlsHintProps) {
    const hint =
        "ARPG Controls\nWASD / Arrow Keys: Move\nMobile: Hold to show radial\nRight Click: Action menu";

    return (
        <div
            style={{
                position: "absolute",
                top: 16,
                left: "50%",
                transform: "translateX(-50%)",
                padding: "10px 14px",
                background: "linear-gradient(180deg, rgba(17,17,24,0.82) 0%, rgba(9,9,12,0.72) 100%)",
                border: "1px solid rgba(255, 210, 102, 0.35)",
                borderRadius: 8,
                color: "rgba(255, 255, 255, 0.95)",
                fontSize: 12,
                fontFamily: "monospace",
                lineHeight: 1.45,
                zIndex: 1000,
                pointerEvents: "none",
                whiteSpace: "pre-line",
                textAlign: "center",
                letterSpacing: "0.03em",
                boxShadow: "0 8px 24px rgba(0,0,0,0.38)",
            }}
        >
            {hint}
        </div>
    );
}

/**
 * Interaction result message badge at bottom-right
 */
export interface InteractionMessageBadgeProps {
    message: string;
}

export function InteractionMessageBadge({ message }: InteractionMessageBadgeProps) {
    return (
        <div
            style={{
                position: "absolute",
                bottom: 16,
                right: 16,
                padding: "8px 10px",
                backgroundColor: "rgba(0, 0, 0, 0.6)",
                border: "1px solid rgba(255, 255, 255, 0.2)",
                borderRadius: 6,
                color: "rgba(255, 255, 255, 0.9)",
                fontSize: 12,
                fontFamily: "monospace",
                zIndex: 1000,
            }}
        >
            {message}
        </div>
    );
}

/**
 * Engine version debug badge at bottom-right
 */
export interface EngineVersionBadgeProps {
    version: string;
    messagePresent?: boolean;
}

export function EngineVersionBadge({ version, messagePresent = false }: EngineVersionBadgeProps) {
    return (
        <div
            style={{
                position: "absolute",
                bottom: messagePresent ? 72 : 16,
                right: 16,
                padding: "4px 8px",
                backgroundColor: "rgba(0, 0, 0, 0.4)",
                border: "1px solid rgba(255, 255, 255, 0.1)",
                borderRadius: 4,
                color: "rgba(255, 255, 255, 0.5)",
                fontSize: 10,
                fontFamily: "monospace",
                zIndex: 999,
                pointerEvents: "none",
                letterSpacing: "0.05em",
            }}
        >
            Engine v{version}
        </div>
    );
}
