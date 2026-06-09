import type { CSSProperties } from "react";

const shellStyle: CSSProperties = {
    minHeight: "100dvh",
    background:
        "radial-gradient(circle at 10% 12%, rgba(14, 165, 233, 0.16), transparent 30%), radial-gradient(circle at 84% 16%, rgba(34, 197, 94, 0.14), transparent 32%), linear-gradient(145deg, #04070d 0%, #0a1524 55%, #0f2033 100%)",
    color: "#e2e8f0",
    display: "grid",
    placeItems: "center",
    padding: "32px 20px",
};

const panelStyle: CSSProperties = {
    width: "min(100%, 980px)",
    border: "1px solid rgba(148, 163, 184, 0.28)",
    borderRadius: 24,
    padding: 28,
    background: "rgba(5, 11, 22, 0.78)",
    boxShadow: "0 24px 90px rgba(2, 6, 23, 0.52)",
};

const titleStyle: CSSProperties = {
    margin: "10px 0 14px",
    fontSize: "clamp(2rem, 4.7vw, 3.6rem)",
    lineHeight: 1,
};

const linkRowStyle: CSSProperties = {
    display: "flex",
    flexWrap: "wrap",
    gap: 12,
    marginTop: 22,
};

const cardStyle: CSSProperties = {
    marginTop: 18,
    borderRadius: 14,
    border: "1px solid rgba(148, 163, 184, 0.22)",
    padding: 16,
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

export function DataSciencePlaygroundPage() {
    return (
        <main style={shellStyle}>
            <section style={panelStyle}>
                <p style={{ margin: 0, letterSpacing: "0.12em", textTransform: "uppercase", color: "#67e8f9", fontSize: 12, fontWeight: 700 }}>
                    Banana Interactive Lab
                </p>
                <h1 style={titleStyle}>Data Science Playground Mode</h1>
                <p style={{ margin: 0, lineHeight: 1.7, color: "#cbd5e1", maxWidth: 780 }}>
                    The web frontend is now focused on notebook-driven prototyping. Generate and iterate on native ABI/runtime ideas through Jupyter artifacts instead of treating this deployment as a pure marketing surface.
                </p>

                <div style={linkRowStyle}>
                    <a href="/notebooks/native-c-catalog.ipynb" style={actionPrimaryStyle}>
                        Open Generated Native Notebook
                    </a>
                    <a href="/marketing" style={actionSecondaryStyle}>
                        View Legacy Marketing Page
                    </a>
                    <a href="/login" style={actionSecondaryStyle}>
                        Account Login
                    </a>
                </div>

                <article style={cardStyle}>
                    <h2 style={{ margin: "0 0 8px", fontSize: 18 }}>Notebook Generation Command</h2>
                    <pre style={{ margin: 0, whiteSpace: "pre-wrap", color: "#a5f3fc" }}>
                        bash scripts/scaffold-abi-notebook-workflow.sh
                    </pre>
                </article>
            </section>
        </main>
    );
}
