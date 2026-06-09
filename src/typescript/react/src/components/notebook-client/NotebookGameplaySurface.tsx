import type { CSSProperties } from "react";

type NotebookGameplaySurfaceProps = {
    readonly selectedFile: string;
    readonly selectedContent: string;
};

const cardStyle: CSSProperties = {
    borderRadius: 18,
    border: "1px solid rgba(45, 212, 191, 0.3)",
    padding: 18,
    background: "linear-gradient(170deg, rgba(4, 16, 28, 0.92), rgba(2, 10, 18, 0.96))",
    boxShadow: "inset 0 1px 0 rgba(94, 234, 212, 0.2)",
};

const codeSurfaceStyle: CSSProperties = {
    width: "100%",
    minHeight: 520,
    borderRadius: 12,
    border: "1px solid rgba(45, 212, 191, 0.35)",
    background: "radial-gradient(circle at top left, rgba(20, 184, 166, 0.14), rgba(2, 6, 23, 0.95) 30%)",
    fontFamily: '"IBM Plex Mono", SFMono-Regular, Menlo, Consolas, monospace',
    fontSize: 13,
    lineHeight: 1.55,
    color: "#d1fae5",
    overflow: "auto",
    whiteSpace: "pre",
    padding: 14,
};

export function NotebookGameplaySurface({ selectedFile, selectedContent }: NotebookGameplaySurfaceProps) {
    const lineCount = selectedContent.split("\n").length;

    return (
        <article style={cardStyle}>
            <h2 style={{ margin: "0 0 8px", fontSize: 18, letterSpacing: "0.04em", textTransform: "uppercase" }}>Mission Viewport</h2>
            <p style={{ margin: 0, color: "#cbd5e1", lineHeight: 1.6 }}>
                Source-driven runtime feed for the current sector. Treat this as the playable client viewport backed by notebook cells.
            </p>
            <div style={{ marginTop: 12, display: "flex", flexWrap: "wrap", gap: 8 }}>
                <span style={{ fontSize: 12, color: "#5eead4", border: "1px solid rgba(45, 212, 191, 0.4)", padding: "4px 8px", borderRadius: 999 }}>
                    Sector: {selectedFile || "none"}
                </span>
                <span style={{ fontSize: 12, color: "#a5b4fc", border: "1px solid rgba(148, 163, 184, 0.35)", padding: "4px 8px", borderRadius: 999 }}>
                    Lines: {lineCount}
                </span>
                <span style={{ fontSize: 12, color: "#86efac", border: "1px solid rgba(34, 197, 94, 0.35)", padding: "4px 8px", borderRadius: 999 }}>
                    Renderer: notebook
                </span>
            </div>
            <div style={{ ...codeSurfaceStyle, marginTop: 10 }}>
                {selectedContent}
            </div>
        </article>
    );
}
