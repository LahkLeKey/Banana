import type { CSSProperties } from "react";

type NotebookExplorerPanelProps = {
    readonly files: string[];
    readonly query: string;
    readonly selectedFile: string;
    readonly onChangeQuery: (query: string) => void;
    readonly onSelectFile: (file: string) => void;
    readonly fileCountLabel: string;
    readonly maxLinesLabel: string;
    readonly sourceRootLabel: string;
};

const panelStyle: CSSProperties = {
    borderRadius: 18,
    border: "1px solid rgba(45, 212, 191, 0.26)",
    padding: 18,
    background: "linear-gradient(170deg, rgba(8, 21, 33, 0.92), rgba(4, 11, 22, 0.95))",
    position: "sticky",
    top: 12,
    alignSelf: "start",
    height: "fit-content",
    fontFamily: '"IBM Plex Sans", "Segoe UI", sans-serif',
};

const inputStyle: CSSProperties = {
    width: "100%",
    borderRadius: 10,
    border: "1px solid rgba(45, 212, 191, 0.35)",
    background: "rgba(6, 15, 26, 0.9)",
    color: "#e2e8f0",
    padding: "10px 11px",
    marginTop: 12,
};

export function NotebookExplorerPanel(props: NotebookExplorerPanelProps) {
    const {
        files,
        query,
        selectedFile,
        onChangeQuery,
        onSelectFile,
        fileCountLabel,
        maxLinesLabel,
        sourceRootLabel,
    } = props;

    return (
        <aside style={panelStyle}>
            <h2 style={{ margin: 0, fontSize: 18, letterSpacing: "0.04em", textTransform: "uppercase" }}>World Codex</h2>
            <p style={{ margin: "8px 0 0", color: "#cbd5e1", lineHeight: 1.6 }}>
                Rebuild mission dataset:
            </p>
            <pre style={{ margin: "10px 0 0", whiteSpace: "pre-wrap", color: "#a5f3fc" }}>
                bash scripts/scaffold-abi-notebook-workflow.sh
            </pre>

            <div style={{ marginTop: 12, fontSize: 13, color: "#a5b4fc" }}>{fileCountLabel}</div>
            <div style={{ marginTop: 4, fontSize: 13, color: "#a5b4fc" }}>{maxLinesLabel}</div>
            <div style={{ marginTop: 4, fontSize: 13, color: "#a5b4fc" }}>{sourceRootLabel}</div>

            <input
                type="text"
                value={query}
                onChange={(event) => onChangeQuery(event.target.value)}
                placeholder="Search sectors (e.g. gameplay_service)"
                style={inputStyle}
            />

            <div style={{ marginTop: 12, maxHeight: 460, overflow: "auto", border: "1px solid rgba(45, 212, 191, 0.2)", borderRadius: 10 }}>
                {files.map((file) => (
                    <button
                        key={file}
                        type="button"
                        onClick={() => onSelectFile(file)}
                        style={{
                            display: "block",
                            width: "100%",
                            textAlign: "left",
                            padding: "10px 11px",
                            border: "none",
                            borderBottom: "1px solid rgba(148, 163, 184, 0.1)",
                            background: file === selectedFile ? "rgba(20, 184, 166, 0.24)" : "transparent",
                            color: "#e2e8f0",
                            cursor: "pointer",
                            fontSize: 12,
                            fontFamily: '"IBM Plex Mono", Consolas, monospace',
                        }}
                    >
                        {file}
                    </button>
                ))}
                {files.length === 0 ? (
                    <div style={{ padding: "10px 11px", color: "#cbd5e1", fontSize: 13 }}>No sectors match this query.</div>
                ) : null}
            </div>
        </aside>
    );
}
