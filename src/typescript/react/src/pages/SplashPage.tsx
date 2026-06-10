import type { CSSProperties } from "react";
import { useNavigate } from "react-router-dom";

const shellStyle: CSSProperties = {
    minHeight: "100dvh",
    display: "grid",
    placeItems: "center",
    background: "radial-gradient(circle at 20% 20%, rgba(16, 185, 129, 0.24), transparent 35%), linear-gradient(155deg, #03060f, #09162a 60%, #0c2136)",
    color: "#e2e8f0",
    fontFamily: '"Rajdhani", "Segoe UI", sans-serif',
    padding: 24,
};

const panelStyle: CSSProperties = {
    width: "min(960px, 100%)",
    borderRadius: 24,
    border: "1px solid rgba(45, 212, 191, 0.35)",
    background: "linear-gradient(170deg, rgba(7, 22, 37, 0.88), rgba(4, 10, 19, 0.96))",
    padding: "30px clamp(18px, 3vw, 36px)",
    boxShadow: "0 28px 100px rgba(2, 6, 23, 0.65)",
};

export function SplashPage() {
    const navigate = useNavigate();

    return (
        <main style={shellStyle}>
            <section style={panelStyle}>
                <p style={{ margin: 0, letterSpacing: "0.14em", textTransform: "uppercase", color: "#5eead4", fontWeight: 700 }}>Boot Sequence</p>
                <h1 style={{ margin: "10px 0 14px", fontSize: "clamp(2.2rem, 6vw, 4rem)", lineHeight: 1 }}>Banana Engine</h1>
                <p style={{ margin: 0, lineHeight: 1.7, color: "#cbd5e1", maxWidth: 780 }}>
                    Welcome to the notebook-rendered gameplay client. This staged entry flow now mirrors a coherent game startup path before entering live notebooks.
                </p>
                <div style={{ marginTop: 20, display: "flex", gap: 12, flexWrap: "wrap" }}>
                    <button
                        type="button"
                        onClick={() => navigate("/banana-engine")}
                        style={{
                            borderRadius: 12,
                            border: "1px solid rgba(20, 184, 166, 0.7)",
                            background: "linear-gradient(135deg, #0f766e, #0e7490)",
                            color: "#f8fafc",
                            fontWeight: 700,
                            padding: "12px 18px",
                            cursor: "pointer",
                        }}
                    >
                        Enter Banana Engine
                    </button>
                </div>
            </section>
        </main>
    );
}
