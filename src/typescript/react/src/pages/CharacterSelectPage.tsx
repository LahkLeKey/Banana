import { useState, type CSSProperties } from "react";
import { useNavigate } from "react-router-dom";
import { storeSelectedCharacter } from "../lib/gameClientFlow";

const CHARACTER_OPTIONS = ["Ari the Mapper", "Nova the Relay", "Kite the Systems Scout"];

const shellStyle: CSSProperties = {
    minHeight: "100dvh",
    display: "grid",
    placeItems: "center",
    background: "radial-gradient(circle at 80% 20%, rgba(14, 165, 233, 0.22), transparent 36%), linear-gradient(155deg, #020712, #071525 58%, #0c1f36)",
    color: "#e2e8f0",
    fontFamily: '"Rajdhani", "Segoe UI", sans-serif',
    padding: 24,
};

const panelStyle: CSSProperties = {
    width: "min(980px, 100%)",
    borderRadius: 24,
    border: "1px solid rgba(56, 189, 248, 0.35)",
    background: "linear-gradient(170deg, rgba(8, 21, 35, 0.9), rgba(4, 10, 19, 0.96))",
    padding: "30px clamp(18px, 3vw, 36px)",
};

export function CharacterSelectPage() {
    const navigate = useNavigate();
    const [selectedCharacter, setSelectedCharacter] = useState(CHARACTER_OPTIONS[0]);

    const handleConfirm = () => {
        storeSelectedCharacter(selectedCharacter);
        navigate("/notebooks");
    };

    return (
        <main style={shellStyle}>
            <section style={panelStyle}>
                <p style={{ margin: 0, letterSpacing: "0.12em", textTransform: "uppercase", color: "#7dd3fc", fontWeight: 700 }}>Character Select</p>
                <h1 style={{ margin: "10px 0 12px", fontSize: "clamp(2rem, 5vw, 3.2rem)", lineHeight: 1.1 }}>Choose Your Operator</h1>
                <p style={{ margin: 0, color: "#cbd5e1", lineHeight: 1.7 }}>Pick an operator profile to continue into the notebook-rendered world client.</p>

                <div style={{ display: "grid", gap: 10, marginTop: 18 }}>
                    {CHARACTER_OPTIONS.map((option) => (
                        <button
                            key={option}
                            type="button"
                            onClick={() => setSelectedCharacter(option)}
                            style={{
                                textAlign: "left",
                                borderRadius: 12,
                                border: selectedCharacter === option ? "1px solid rgba(20, 184, 166, 0.7)" : "1px solid rgba(148, 163, 184, 0.35)",
                                background: selectedCharacter === option ? "rgba(20, 184, 166, 0.2)" : "rgba(15, 23, 42, 0.45)",
                                color: "#e2e8f0",
                                fontWeight: 700,
                                padding: "12px 14px",
                                cursor: "pointer",
                            }}
                        >
                            {option}
                        </button>
                    ))}
                </div>

                <div style={{ marginTop: 18, display: "flex", gap: 12, flexWrap: "wrap" }}>
                    <button
                        type="button"
                        onClick={handleConfirm}
                        style={{ borderRadius: 12, border: "1px solid rgba(20, 184, 166, 0.7)", background: "linear-gradient(135deg, #0f766e, #0e7490)", color: "#f8fafc", fontWeight: 700, padding: "12px 18px", cursor: "pointer" }}
                    >
                        Confirm and Launch Notebooks
                    </button>
                    <button
                        type="button"
                        onClick={() => navigate("/game-main-menu")}
                        style={{ borderRadius: 12, border: "1px solid rgba(148, 163, 184, 0.45)", background: "rgba(15, 23, 42, 0.45)", color: "#e2e8f0", fontWeight: 700, padding: "12px 18px", cursor: "pointer" }}
                    >
                        Back to Main Menu
                    </button>
                </div>
            </section>
        </main>
    );
}
