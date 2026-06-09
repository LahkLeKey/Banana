import type { CSSProperties } from "react";

type NotebookGameplaySurfaceProps = {
    readonly selectedFile: string;
    readonly selectedContent: string;
};

const cardStyle: CSSProperties = {
    borderRadius: 14,
    border: "1px solid rgba(148, 163, 184, 0.22)",
    padding: 18,
    background: "rgba(15, 23, 42, 0.55)",
};

const codeSurfaceStyle: CSSProperties = {
    width: "100%",
    minHeight: 520,
    borderRadius: 12,
    border: "1px solid rgba(148, 163, 184, 0.35)",
    background: "rgba(2, 6, 23, 0.76)",
    fontFamily: "ui-monospace, SFMono-Regular, Menlo, Consolas, monospace",
    fontSize: 13,
    lineHeight: 1.55,
    color: "#d1fae5",
    overflow: "auto",
    whiteSpace: "pre",
    padding: 14,
};

export function NotebookGameplaySurface({ selectedFile, selectedContent }: NotebookGameplaySurfaceProps) {
    return (
        <article style={cardStyle}>
            <h2 style={{ margin: "0 0 8px", fontSize: 18 }}>Notebook MMO Renderer</h2>
            <p style={{ margin: 0, color: "#cbd5e1", lineHeight: 1.6 }}>
                Selected notebook source cell rendered as the current gameplay client surface.
            </p>
            <div style={{ marginTop: 12, fontSize: 12, color: "#a5b4fc", wordBreak: "break-all" }}>
                Active file: {selectedFile || "none"}
            </div>
            <div style={{ ...codeSurfaceStyle, marginTop: 10 }}>
                {selectedContent}
            </div>
        </article>
    );
}
