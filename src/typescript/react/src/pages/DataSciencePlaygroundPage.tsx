import { useEffect, useMemo, useState, type CSSProperties } from "react";
import { MainMenuPanel } from "../components/notebook-client/MainMenuPanel";
import { NotebookExplorerPanel } from "../components/notebook-client/NotebookExplorerPanel";
import { NotebookGameplaySurface } from "../components/notebook-client/NotebookGameplaySurface";
import { NotebookHealthPanel } from "../components/notebook-client/NotebookHealthPanel";
import { useNotebookClient } from "../lib/notebook-client/useNotebookClient";

const shellStyle: CSSProperties = {
    minHeight: "100dvh",
    background:
        "radial-gradient(circle at 10% 12%, rgba(14, 165, 233, 0.16), transparent 30%), radial-gradient(circle at 84% 16%, rgba(34, 197, 94, 0.14), transparent 32%), linear-gradient(145deg, #04070d 0%, #0a1524 55%, #0f2033 100%)",
    color: "#e2e8f0",
    padding: "28px clamp(16px, 2.2vw, 36px)",
};

const panelStyle: CSSProperties = {
    width: "100%",
    border: "1px solid rgba(148, 163, 184, 0.28)",
    borderRadius: 24,
    padding: "28px clamp(18px, 2.4vw, 36px)",
    background: "rgba(5, 11, 22, 0.78)",
    boxShadow: "0 24px 90px rgba(2, 6, 23, 0.52)",
};

const dashboardGridStyle: CSSProperties = {
    display: "grid",
    gridTemplateColumns: "minmax(290px, 360px) minmax(0, 1fr)",
    gap: 16,
    marginTop: 18,
};

export function DataSciencePlaygroundPage() {
    const { loading, manifest, manifestError, manifestSource, notebookError, notebookSource, cellLookup, notebookCellCount } = useNotebookClient();
    const [query, setQuery] = useState("");
    const [selectedFile, setSelectedFile] = useState<string>("");
    const [mode, setMode] = useState<"mission" | "explorer" | "diagnostics">("mission");

    const filteredFiles = useMemo(() => {
        const files = manifest?.files ?? [];
        const normalizedQuery = query.trim().toLowerCase();
        if (!normalizedQuery) {
            return files;
        }
        return files.filter((file) => file.toLowerCase().includes(normalizedQuery));
    }, [manifest?.files, query]);

    useEffect(() => {
        if (filteredFiles.length === 0) {
            setSelectedFile("");
            return;
        }
        if (!selectedFile || !filteredFiles.includes(selectedFile)) {
            setSelectedFile(filteredFiles[0]);
        }
    }, [filteredFiles, selectedFile]);

    const selectedContent = selectedFile ? (cellLookup.get(selectedFile) ?? "No code cell available for this file.") : "Select a file from the notebook index.";
    const manifestLabel = manifest ? `Files indexed: ${manifest.file_count}` : "Manifest pending";
    const sourceRootLabel = manifest ? `Source root: ${manifest.source_root}` : "";
    const maxLinesLabel = manifest ? `Max lines/file: ${manifest.max_lines_per_file}` : "Run workflow to publish metadata";
    const sourceLabel = manifestSource.length > 0 ? `Manifest source: ${manifestSource}` : (notebookSource.length > 0 ? `Notebook source: ${notebookSource}` : "");

    return (
        <main style={shellStyle}>
            <section style={panelStyle}>
                <MainMenuPanel
                    indexedFileCount={manifest?.file_count ?? 0}
                    notebookCellCount={notebookCellCount}
                    manifestAvailable={Boolean(manifest)}
                    activeMode={mode}
                    onChangeMode={setMode}
                />

                {mode === "mission" ? (
                    <section style={{ display: "grid", gap: 16, marginTop: 18 }}>
                        <NotebookGameplaySurface selectedFile={selectedFile} selectedContent={selectedContent} />
                        <NotebookHealthPanel
                            loading={loading}
                            manifestError={manifestError}
                            notebookError={notebookError}
                            manifestSource={manifestSource}
                            notebookSource={notebookSource}
                        />
                    </section>
                ) : null}

                {mode === "explorer" ? (
                    <section style={dashboardGridStyle}>
                        <NotebookExplorerPanel
                            files={filteredFiles}
                            query={query}
                            selectedFile={selectedFile}
                            onChangeQuery={setQuery}
                            onSelectFile={setSelectedFile}
                            fileCountLabel={manifestLabel}
                            maxLinesLabel={sourceLabel.length > 0 ? `${maxLinesLabel} | ${sourceLabel}` : maxLinesLabel}
                            sourceRootLabel={sourceRootLabel}
                        />

                        <div style={{ display: "grid", gap: 16 }}>
                            <NotebookGameplaySurface selectedFile={selectedFile} selectedContent={selectedContent} />
                        </div>
                    </section>
                ) : null}

                {mode === "diagnostics" ? (
                    <section style={{ display: "grid", gap: 16, marginTop: 18 }}>
                        <NotebookHealthPanel
                            loading={loading}
                            manifestError={manifestError}
                            notebookError={notebookError}
                            manifestSource={manifestSource}
                            notebookSource={notebookSource}
                        />
                        <NotebookExplorerPanel
                            files={filteredFiles}
                            query={query}
                            selectedFile={selectedFile}
                            onChangeQuery={setQuery}
                            onSelectFile={setSelectedFile}
                            fileCountLabel={manifestLabel}
                            maxLinesLabel={sourceLabel.length > 0 ? `${maxLinesLabel} | ${sourceLabel}` : maxLinesLabel}
                            sourceRootLabel={sourceRootLabel}
                        />
                    </section>
                ) : null}
            </section>
        </main>
    );
}
