import { useEffect, useMemo, useState, type CSSProperties } from "react";

type NotebookManifest = {
    generated_at_utc: string;
    source_root: string;
    notebook_path: string;
    file_count: number;
    max_lines_per_file: number;
    files: string[];
};

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
    width: "min(100%, 1180px)",
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
    borderRadius: 14,
    border: "1px solid rgba(148, 163, 184, 0.22)",
    padding: 16,
    background: "rgba(15, 23, 42, 0.55)",
};

const dashboardGridStyle: CSSProperties = {
    display: "grid",
    gridTemplateColumns: "minmax(240px, 290px) minmax(0, 1fr)",
    gap: 16,
    marginTop: 18,
};

const sidebarStyle: CSSProperties = {
    ...cardStyle,
    position: "sticky",
    top: 16,
    alignSelf: "start",
    height: "fit-content",
};

const notebookFrameStyle: CSSProperties = {
    width: "100%",
    minHeight: 420,
    borderRadius: 12,
    border: "1px solid rgba(148, 163, 184, 0.35)",
    background: "rgba(2, 6, 23, 0.7)",
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
    const [manifest, setManifest] = useState<NotebookManifest | null>(null);
    const [manifestError, setManifestError] = useState<string | null>(null);

    useEffect(() => {
        let cancelled = false;

        fetch("/notebooks/catalog-index.json")
            .then((response) => {
                if (!response.ok) {
                    throw new Error("manifest unavailable");
                }
                return response.json();
            })
            .then((payload: NotebookManifest) => {
                if (!cancelled) {
                    setManifest(payload);
                    setManifestError(null);
                }
            })
            .catch(() => {
                if (!cancelled) {
                    setManifest(null);
                    setManifestError("Notebook manifest not found yet. Run scaffold workflow.");
                }
            });

        return () => {
            cancelled = true;
        };
    }, []);

    const previewFiles = useMemo(() => manifest?.files.slice(0, 10) ?? [], [manifest]);

    return (
        <main style={shellStyle}>
            <section style={panelStyle}>
                <p style={{ margin: 0, letterSpacing: "0.12em", textTransform: "uppercase", color: "#67e8f9", fontSize: 12, fontWeight: 700 }}>
                    Banana Interactive Lab
                </p>
                <h1 style={titleStyle}>Data Science Playground Mode</h1>
                <p style={{ margin: 0, lineHeight: 1.7, color: "#cbd5e1", maxWidth: 780 }}>
                    This deployment is now a notebook-first experimentation surface. Think Streamlit-style workflow: generate data artifacts, inspect them in-browser, and iterate quickly against native ABI/runtime sources.
                </p>

                <div style={linkRowStyle}>
                    <a href="/notebooks/native-c-catalog.ipynb" style={actionPrimaryStyle}>
                        Download Native Notebook
                    </a>
                    <a href="/marketing" style={actionSecondaryStyle}>
                        View Legacy Marketing Page
                    </a>
                    <a href="/login" style={actionSecondaryStyle}>
                        Account Login
                    </a>
                </div>

                <section style={dashboardGridStyle}>
                    <aside style={sidebarStyle}>
                        <h2 style={{ margin: 0, fontSize: 18 }}>Notebook Control Panel</h2>
                        <p style={{ margin: "8px 0 0", color: "#cbd5e1", lineHeight: 1.6 }}>
                            Regenerate notebook assets:
                        </p>
                        <pre style={{ margin: "10px 0 0", whiteSpace: "pre-wrap", color: "#a5f3fc" }}>
                            bash scripts/scaffold-abi-notebook-workflow.sh
                        </pre>
                        <div style={{ marginTop: 12, fontSize: 13, color: "#a5b4fc" }}>
                            {manifest ? `Files indexed: ${manifest.file_count}` : "Manifest pending"}
                        </div>
                        <div style={{ marginTop: 4, fontSize: 13, color: "#a5b4fc" }}>
                            {manifest ? `Max lines/file: ${manifest.max_lines_per_file}` : "Run workflow to publish metadata"}
                        </div>
                        <div style={{ marginTop: 4, fontSize: 13, color: "#a5b4fc" }}>
                            {manifest ? `Source root: ${manifest.source_root}` : ""}
                        </div>
                    </aside>

                    <div style={{ display: "grid", gap: 16 }}>
                        <article style={cardStyle}>
                            <h2 style={{ margin: "0 0 8px", fontSize: 18 }}>Live Notebook Surface</h2>
                            <p style={{ margin: 0, color: "#cbd5e1", lineHeight: 1.6 }}>
                                Notebook files are published to `/notebooks/*` during deploy and local scaffold runs.
                            </p>
                            <iframe
                                title="Notebook JSON preview"
                                src="/notebooks/catalog-index.json"
                                style={{ ...notebookFrameStyle, marginTop: 12 }}
                            />
                        </article>

                        <article style={cardStyle}>
                            <h2 style={{ margin: "0 0 8px", fontSize: 18 }}>Indexed Native Sources</h2>
                            {manifestError ? (
                                <p style={{ margin: 0, color: "#fca5a5" }}>{manifestError}</p>
                            ) : null}
                            {previewFiles.length > 0 ? (
                                <ul style={{ margin: 0, paddingLeft: 18, color: "#cbd5e1", lineHeight: 1.55 }}>
                                    {previewFiles.map((file) => (
                                        <li key={file}>{file}</li>
                                    ))}
                                </ul>
                            ) : (
                                <p style={{ margin: 0, color: "#cbd5e1" }}>No indexed files yet.</p>
                            )}
                        </article>
                    </div>
                </section>
            </section>
        </main>
    );
}
