import type { CSSProperties } from "react";

type MainMenuPanelProps = {
    readonly indexedFileCount: number;
    readonly notebookCellCount: number;
    readonly manifestAvailable: boolean;
    readonly activeMode: "mission" | "explorer" | "diagnostics";
    readonly onChangeMode: (mode: "mission" | "explorer" | "diagnostics") => void;
};

const containerStyle: CSSProperties = {
    borderRadius: 20,
    border: "1px solid rgba(45, 212, 191, 0.28)",
    padding: 22,
    background: "linear-gradient(170deg, rgba(7, 25, 32, 0.9), rgba(3, 10, 20, 0.95))",
    boxShadow: "0 20px 70px rgba(2, 8, 23, 0.55)",
    fontFamily: '"Rajdhani", "Segoe UI", sans-serif',
};

const actionPrimaryStyle: CSSProperties = {
    display: "inline-flex",
    alignItems: "center",
    justifyContent: "center",
    padding: "13px 16px",
    borderRadius: 12,
    background: "linear-gradient(135deg, #0e7490, #0f766e)",
    color: "#f8fafc",
    textDecoration: "none",
    fontWeight: 700,
};

const actionSecondaryStyle: CSSProperties = {
    display: "inline-flex",
    alignItems: "center",
    justifyContent: "center",
    padding: "13px 16px",
    borderRadius: 12,
    border: "1px solid rgba(148, 163, 184, 0.38)",
    color: "#e2e8f0",
    textDecoration: "none",
    fontWeight: 700,
};

export function MainMenuPanel({ indexedFileCount, notebookCellCount, manifestAvailable, activeMode, onChangeMode }: MainMenuPanelProps) {
    const modeButton = (mode: "mission" | "explorer" | "diagnostics", label: string) => (
        <button
            type="button"
            onClick={() => onChangeMode(mode)}
            style={{
                padding: "10px 12px",
                borderRadius: 10,
                border: activeMode === mode ? "1px solid rgba(45, 212, 191, 0.7)" : "1px solid rgba(148, 163, 184, 0.35)",
                background: activeMode === mode ? "rgba(20, 184, 166, 0.2)" : "rgba(15, 23, 42, 0.6)",
                color: activeMode === mode ? "#5eead4" : "#e2e8f0",
                fontWeight: 700,
                letterSpacing: "0.04em",
                cursor: "pointer",
            }}
        >
            {label}
        </button>
    );

    return (
        <section style={containerStyle}>
            <p style={{ margin: 0, letterSpacing: "0.12em", textTransform: "uppercase", color: "#67e8f9", fontSize: 12, fontWeight: 700 }}>
                Banana Command Uplink
            </p>
            <h1 style={{ margin: "10px 0 14px", fontSize: "clamp(2.2rem, 5vw, 4.2rem)", lineHeight: 1 }}>
                Overworld Runtime Bridge
            </h1>
            <p style={{ margin: 0, lineHeight: 1.7, color: "#cbd5e1", maxWidth: 980 }}>
                Tactical client shell for notebook-driven world simulation. Use mission mode for active runtime, explorer for source intel, and diagnostics for transport health.
            </p>

            <div style={{ display: "flex", flexWrap: "wrap", gap: 10, marginTop: 14 }}>
                {modeButton("mission", "Mission Brief")}
                {modeButton("explorer", "Source Explorer")}
                {modeButton("diagnostics", "Diagnostics")}
            </div>

            <div style={{ display: "grid", gridTemplateColumns: "repeat(3, minmax(0, 1fr))", gap: 12, marginTop: 16 }}>
                <article style={{ borderRadius: 12, border: "1px solid rgba(148, 163, 184, 0.25)", background: "rgba(8, 13, 28, 0.7)", padding: 14 }}>
                    <div style={{ fontSize: 12, color: "#93c5fd", textTransform: "uppercase", letterSpacing: "0.08em" }}>Map Sectors</div>
                    <div style={{ marginTop: 6, fontSize: 28, fontWeight: 700 }}>{indexedFileCount}</div>
                </article>
                <article style={{ borderRadius: 12, border: "1px solid rgba(148, 163, 184, 0.25)", background: "rgba(8, 13, 28, 0.7)", padding: 14 }}>
                    <div style={{ fontSize: 12, color: "#93c5fd", textTransform: "uppercase", letterSpacing: "0.08em" }}>Runtime Cells</div>
                    <div style={{ marginTop: 6, fontSize: 28, fontWeight: 700 }}>{notebookCellCount}</div>
                </article>
                <article style={{ borderRadius: 12, border: "1px solid rgba(148, 163, 184, 0.25)", background: "rgba(8, 13, 28, 0.7)", padding: 14 }}>
                    <div style={{ fontSize: 12, color: "#93c5fd", textTransform: "uppercase", letterSpacing: "0.08em" }}>Connection</div>
                    <div style={{ marginTop: 8, fontSize: 16, color: manifestAvailable ? "#86efac" : "#fca5a5", fontWeight: 700 }}>
                        {manifestAvailable ? "Synchronized" : "Awaiting Sync"}
                    </div>
                </article>
            </div>

            <div style={{ display: "flex", flexWrap: "wrap", gap: 12, marginTop: 18 }}>
                <a href="/notebooks/native-c-catalog.ipynb" style={actionPrimaryStyle}>Download Notebook Payload</a>
                <a href="/notebooks/catalog-index.json" style={actionSecondaryStyle}>Open Notebook Manifest</a>
                <a href="/marketing" style={actionSecondaryStyle}>Legacy Marketing</a>
                <a href="/login" style={actionSecondaryStyle}>Account Login</a>
            </div>
        </section>
    );
}
