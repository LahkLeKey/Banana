import type { CSSProperties } from "react";

type NotebookHealthPanelProps = {
    readonly loading: boolean;
    readonly manifestError: string | null;
    readonly notebookError: string | null;
    readonly manifestSource: string;
    readonly notebookSource: string;
};

const cardStyle: CSSProperties = {
    borderRadius: 18,
    border: "1px solid rgba(45, 212, 191, 0.22)",
    padding: 18,
    background: "linear-gradient(170deg, rgba(8, 21, 33, 0.92), rgba(5, 14, 24, 0.96))",
    fontFamily: '"IBM Plex Sans", "Segoe UI", sans-serif',
};

export function NotebookHealthPanel({ loading, manifestError, notebookError, manifestSource, notebookSource }: NotebookHealthPanelProps) {
    return (
        <article style={cardStyle}>
            <h2 style={{ margin: "0 0 8px", fontSize: 18, letterSpacing: "0.04em", textTransform: "uppercase" }}>Relay Diagnostics</h2>
            {loading ? <p style={{ margin: 0, color: "#cbd5e1" }}>Syncing manifest and notebook payload...</p> : null}
            {manifestError ? <p style={{ margin: 0, color: "#fca5a5" }}>{manifestError}</p> : null}
            {notebookError ? <p style={{ margin: "8px 0 0", color: "#fca5a5" }}>{notebookError}</p> : null}
            {!loading && !manifestError && !notebookError ? (
                <p style={{ margin: "8px 0 0", color: "#cbd5e1", lineHeight: 1.6 }}>
                    Transport healthy. Mission dataset is available and API-first fallback routing is operational.
                </p>
            ) : null}
            {!loading && (manifestSource.length > 0 || notebookSource.length > 0) ? (
                <p style={{ margin: "8px 0 0", color: "#93c5fd", lineHeight: 1.6 }}>
                    {manifestSource.length > 0 ? `Manifest source: ${manifestSource}. ` : ""}
                    {notebookSource.length > 0 ? `Notebook source: ${notebookSource}.` : ""}
                </p>
            ) : null}
        </article>
    );
}
