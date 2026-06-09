import type { CSSProperties } from "react";

type NotebookHealthPanelProps = {
    readonly loading: boolean;
    readonly manifestError: string | null;
    readonly notebookError: string | null;
    readonly manifestSource: string;
    readonly notebookSource: string;
};

const cardStyle: CSSProperties = {
    borderRadius: 14,
    border: "1px solid rgba(148, 163, 184, 0.22)",
    padding: 18,
    background: "rgba(15, 23, 42, 0.55)",
};

export function NotebookHealthPanel({ loading, manifestError, notebookError, manifestSource, notebookSource }: NotebookHealthPanelProps) {
    return (
        <article style={cardStyle}>
            <h2 style={{ margin: "0 0 8px", fontSize: 18 }}>Data & API Orchestration Health</h2>
            {loading ? <p style={{ margin: 0, color: "#cbd5e1" }}>Loading notebook manifest and payload...</p> : null}
            {manifestError ? <p style={{ margin: 0, color: "#fca5a5" }}>{manifestError}</p> : null}
            {notebookError ? <p style={{ margin: "8px 0 0", color: "#fca5a5" }}>{notebookError}</p> : null}
            {!loading && !manifestError && !notebookError ? (
                <p style={{ margin: "8px 0 0", color: "#cbd5e1", lineHeight: 1.6 }}>
                    Manifest and notebook payload loaded. API orchestration can layer on this shell while static notebook fallback remains active.
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
