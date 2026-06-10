import type { CSSProperties } from "react";
import { clearStoredAuthSession, readStoredAuthSession } from "@banana/ui";
import { useNavigate } from "react-router-dom";
import { clearSelectedCharacter } from "../lib/gameClientFlow";

const shellStyle: CSSProperties = {
    minHeight: "100dvh",
    display: "grid",
    placeItems: "center",
    background: "radial-gradient(circle at 15% 14%, rgba(20, 184, 166, 0.2), transparent 30%), linear-gradient(155deg, #030712, #0a1628 55%, #0f243c)",
    color: "#e2e8f0",
    fontFamily: '"Rajdhani", "Segoe UI", sans-serif',
    padding: 24,
};

const panelStyle: CSSProperties = {
    width: "min(1020px, 100%)",
    borderRadius: 24,
    border: "1px solid rgba(45, 212, 191, 0.35)",
    background: "linear-gradient(170deg, rgba(7, 22, 37, 0.9), rgba(4, 10, 19, 0.96))",
    padding: "30px clamp(18px, 3vw, 36px)",
};

export function GameMainMenuPage() {
    const navigate = useNavigate();
    const session = readStoredAuthSession();

    const handleSignOut = () => {
        clearStoredAuthSession();
        clearSelectedCharacter();
        navigate("/login", { replace: true });
    };

    return (
        <main style={shellStyle}>
            <section style={panelStyle}>
                <p style={{ margin: 0, letterSpacing: "0.12em", textTransform: "uppercase", color: "#5eead4", fontWeight: 700 }}>Game Main Menu</p>
                <h1 style={{ margin: "10px 0 12px", fontSize: "clamp(2rem, 5vw, 3.4rem)", lineHeight: 1.1 }}>Operation Notebook Tide</h1>
                <p style={{ margin: 0, lineHeight: 1.7, color: "#cbd5e1" }}>
                    Active operator token detected. Proceed to character selection before entering notebook sectors.
                </p>
                <p style={{ margin: "10px 0 0", color: "#93c5fd" }}>Session: {session?.steamId || "guest"}</p>

                <div style={{ marginTop: 20, display: "flex", flexWrap: "wrap", gap: 12 }}>
                    <button
                        type="button"
                        onClick={() => navigate("/select-character")}
                        style={{ borderRadius: 12, border: "1px solid rgba(20, 184, 166, 0.7)", background: "linear-gradient(135deg, #0f766e, #0e7490)", color: "#f8fafc", fontWeight: 700, padding: "12px 18px", cursor: "pointer" }}
                    >
                        Select Character
                    </button>
                    <button
                        type="button"
                        onClick={() => navigate("/notebooks")}
                        style={{ borderRadius: 12, border: "1px solid rgba(56, 189, 248, 0.65)", background: "rgba(8, 47, 73, 0.6)", color: "#e0f2fe", fontWeight: 700, padding: "12px 18px", cursor: "pointer" }}
                    >
                        Enter Notebooks
                    </button>
                    <button
                        type="button"
                        onClick={handleSignOut}
                        style={{ borderRadius: 12, border: "1px solid rgba(248, 113, 113, 0.5)", background: "rgba(69, 10, 10, 0.55)", color: "#fecaca", fontWeight: 700, padding: "12px 18px", cursor: "pointer" }}
                    >
                        Sign Out
                    </button>
                </div>
            </section>
        </main>
    );
}
