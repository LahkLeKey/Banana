import type { CSSProperties } from "react";

type MainMenuPanelProps = {
    readonly indexedFileCount: number;
    readonly notebookCellCount: number;
    readonly manifestAvailable: boolean;
};

const containerStyle: CSSProperties = {
    borderRadius: 14,
    border: "1px solid rgba(148, 163, 184, 0.22)",
    padding: 18,
    background: "rgba(15, 23, 42, 0.55)",
};

const actionPrimaryStyle: CSSProperties = {
    display: "inline-flex",
    alignItems: "center",
    justifyContent: "center",
    padding: "13px 16px",
    borderRadius: 12,
    background: "linear-gradient(135deg, #0891b2, #0f766e)",
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

export function MainMenuPanel({ indexedFileCount, notebookCellCount, manifestAvailable }: MainMenuPanelProps) {
    return (
        <section style={containerStyle}>
            <p style={{ margin: 0, letterSpacing: "0.12em", textTransform: "uppercase", color: "#67e8f9", fontSize: 12, fontWeight: 700 }}>
                Banana Interactive Lab
            </p>
            <h1 style={{ margin: "10px 0 14px", fontSize: "clamp(2.2rem, 5vw, 4.2rem)", lineHeight: 1 }}>
                Notebook MMO Client
            </h1>
            <p style={{ margin: 0, lineHeight: 1.7, color: "#cbd5e1", maxWidth: 980 }}>
                Main menu shell for notebook-first gameplay rendering. Launch into source-backed MMO prototype flows and orchestrate API integration as data services evolve.
            </p>

            <div style={{ display: "grid", gridTemplateColumns: "repeat(3, minmax(0, 1fr))", gap: 12, marginTop: 16 }}>
                <article style={{ borderRadius: 12, border: "1px solid rgba(148, 163, 184, 0.25)", background: "rgba(8, 13, 28, 0.7)", padding: 14 }}>
                    <div style={{ fontSize: 12, color: "#93c5fd", textTransform: "uppercase", letterSpacing: "0.08em" }}>Indexed Files</div>
                    <div style={{ marginTop: 6, fontSize: 28, fontWeight: 700 }}>{indexedFileCount}</div>
                </article>
                <article style={{ borderRadius: 12, border: "1px solid rgba(148, 163, 184, 0.25)", background: "rgba(8, 13, 28, 0.7)", padding: 14 }}>
                    <div style={{ fontSize: 12, color: "#93c5fd", textTransform: "uppercase", letterSpacing: "0.08em" }}>Notebook Cells</div>
                    <div style={{ marginTop: 6, fontSize: 28, fontWeight: 700 }}>{notebookCellCount}</div>
                </article>
                <article style={{ borderRadius: 12, border: "1px solid rgba(148, 163, 184, 0.25)", background: "rgba(8, 13, 28, 0.7)", padding: 14 }}>
                    <div style={{ fontSize: 12, color: "#93c5fd", textTransform: "uppercase", letterSpacing: "0.08em" }}>Status</div>
                    <div style={{ marginTop: 8, fontSize: 16, color: manifestAvailable ? "#86efac" : "#fca5a5", fontWeight: 700 }}>
                        {manifestAvailable ? "Ready" : "Waiting"}
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
