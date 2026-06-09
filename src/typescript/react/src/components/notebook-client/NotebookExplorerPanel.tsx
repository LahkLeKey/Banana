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
    borderRadius: 14,
    border: "1px solid rgba(148, 163, 184, 0.22)",
    padding: 18,
    background: "rgba(15, 23, 42, 0.55)",
    position: "sticky",
    top: 12,
    alignSelf: "start",
    height: "fit-content",
};

const inputStyle: CSSProperties = {
    width: "100%",
    borderRadius: 10,
    border: "1px solid rgba(148, 163, 184, 0.38)",
    background: "rgba(15, 23, 42, 0.62)",
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
            <h2 style={{ margin: 0, fontSize: 18 }}>Notebook Explorer</h2>
            <p style={{ margin: "8px 0 0", color: "#cbd5e1", lineHeight: 1.6 }}>
                Regenerate notebook assets:
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
                placeholder="Filter files (e.g. gameplay_service)"
                style={inputStyle}
            />

            <div style={{ marginTop: 12, maxHeight: 460, overflow: "auto", border: "1px solid rgba(148, 163, 184, 0.22)", borderRadius: 10 }}>
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
                            borderBottom: "1px solid rgba(148, 163, 184, 0.14)",
                            background: file === selectedFile ? "rgba(34, 211, 238, 0.18)" : "transparent",
                            color: "#e2e8f0",
                            cursor: "pointer",
                            fontSize: 12,
                        }}
                    >
                        {file}
                    </button>
                ))}
                {files.length === 0 ? (
                    <div style={{ padding: "10px 11px", color: "#cbd5e1", fontSize: 13 }}>No files match this filter.</div>
                ) : null}
            </div>
        </aside>
    );
}
