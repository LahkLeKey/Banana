/**
 * Loading splash screen displayed while WASM engine initializes
 */
export interface SplashOverlayProps {
    visible: boolean;
}

export function SplashOverlay({ visible }: SplashOverlayProps) {
    if (!visible) {
        return null;
    }

    return (
        <>
            <style>{`
        @keyframes potassium-pulse {
          0%, 100% { opacity: 0.35; letter-spacing: 0.18em; }
          50%       { opacity: 1;    letter-spacing: 0.28em; }
        }
        @keyframes potassium-fadein {
          from { opacity: 0; transform: scale(0.97); }
          to   { opacity: 1; transform: scale(1); }
        }
      `}</style>
            <div
                style={{
                    position: "absolute",
                    inset: 0,
                    display: "flex",
                    flexDirection: "column",
                    alignItems: "center",
                    justifyContent: "center",
                    background: "radial-gradient(ellipse at 50% 60%, #1a1400 0%, #000 70%)",
                    zIndex: 2000,
                    animation: "potassium-fadein 0.4s ease both",
                    gap: 0,
                }}
            >
                <img
                    src="/splash.png"
                    alt="Banana Engine — Peel-Powered Performance"
                    style={{
                        width: "min(560px, 90vw)",
                        height: "auto",
                        imageRendering: "auto",
                        filter: "drop-shadow(0 0 40px rgba(210,160,0,0.35))",
                        userSelect: "none",
                        pointerEvents: "none",
                    }}
                />
                <div
                    style={{
                        marginTop: 8,
                        fontFamily: "'Arial Black', Impact, sans-serif",
                        fontSize: "clamp(11px, 1.8vw, 16px)",
                        fontWeight: 900,
                        color: "#d4a800",
                        letterSpacing: "0.18em",
                        textTransform: "uppercase",
                        animation: "potassium-pulse 1.6s ease-in-out infinite",
                    }}
                >
                    Initializing
                </div>
                <div
                    style={{
                        marginTop: 10,
                        fontFamily: "monospace",
                        fontSize: "clamp(9px, 1.1vw, 11px)",
                        color: "rgba(255,255,255,0.3)",
                        letterSpacing: "0.06em",
                        textTransform: "uppercase",
                    }}
                >
                    100% Potassium Powered · Natural Fuel Efficiency · Low Emissions*
                </div>
                <div
                    style={{
                        marginTop: 4,
                        fontFamily: "monospace",
                        fontSize: "clamp(8px, 0.9vw, 10px)",
                        color: "rgba(255,255,255,0.15)",
                        letterSpacing: "0.05em",
                    }}
                >
                    *Exhaust may contain potassium
                </div>
            </div>
        </>
    );
}
