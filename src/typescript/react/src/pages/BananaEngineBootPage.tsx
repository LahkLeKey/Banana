import { useEffect, useState, type CSSProperties } from "react";
import { useNavigate } from "react-router-dom";

const shellStyle: CSSProperties = {
    minHeight: "100dvh",
    display: "grid",
    placeItems: "center",
    background: "radial-gradient(circle at 80% 18%, rgba(56, 189, 248, 0.2), transparent 36%), linear-gradient(155deg, #040915, #081628 60%, #10243f)",
    color: "#e2e8f0",
    fontFamily: '"Rajdhani", "Segoe UI", sans-serif',
    padding: 24,
};

const panelStyle: CSSProperties = {
    width: "min(900px, 100%)",
    borderRadius: 24,
    border: "1px solid rgba(56, 189, 248, 0.35)",
    background: "linear-gradient(170deg, rgba(8, 20, 34, 0.88), rgba(3, 10, 19, 0.96))",
    padding: "30px clamp(18px, 3vw, 36px)",
};

export function BananaEngineBootPage() {
    const navigate = useNavigate();
    const [status, setStatus] = useState("Loading world topology...");

    useEffect(() => {
        const timer = window.setTimeout(() => {
            setStatus("Engine ready. Authentication relay online.");
        }, 900);
        return () => window.clearTimeout(timer);
    }, []);

    return (
        <main style={shellStyle}>
            <section style={panelStyle}>
                <p style={{ margin: 0, letterSpacing: "0.12em", textTransform: "uppercase", color: "#7dd3fc", fontWeight: 700 }}>Runtime Bring-up</p>
                <h1 style={{ margin: "10px 0 12px", fontSize: "clamp(2rem, 5vw, 3.2rem)", lineHeight: 1.1 }}>Engine Relay</h1>
                <p style={{ margin: 0, color: "#cbd5e1", lineHeight: 1.7 }}>{status}</p>
                <div style={{ marginTop: 20, display: "flex", gap: 12, flexWrap: "wrap" }}>
                    <button
                        type="button"
                        onClick={() => navigate("/login")}
                        style={{
                            borderRadius: 12,
                            border: "1px solid rgba(56, 189, 248, 0.65)",
                            background: "linear-gradient(135deg, #0369a1, #0f766e)",
                            color: "#f8fafc",
                            fontWeight: 700,
                            padding: "12px 18px",
                            cursor: "pointer",
                        }}
                    >
                        Continue to Login
                    </button>
                    <button
                        type="button"
                        onClick={() => navigate("/")}
                        style={{
                            borderRadius: 12,
                            border: "1px solid rgba(148, 163, 184, 0.45)",
                            background: "rgba(15, 23, 42, 0.45)",
                            color: "#e2e8f0",
                            fontWeight: 700,
                            padding: "12px 18px",
                            cursor: "pointer",
                        }}
                    >
                        Back to Splash
                    </button>
                </div>
            </section>
        </main>
    );
}
