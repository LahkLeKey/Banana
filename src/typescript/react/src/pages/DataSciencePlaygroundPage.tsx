import { useEffect, useMemo, useState, type CSSProperties } from "react";
import { MainMenuPanel } from "../components/notebook-client/MainMenuPanel";
import { NotebookExplorerPanel } from "../components/notebook-client/NotebookExplorerPanel";
import { NotebookGameplaySurface } from "../components/notebook-client/NotebookGameplaySurface";
import { NotebookOperationsPanel } from "../components/notebook-client/NotebookOperationsPanel";
import { useNotebookClient } from "../lib/notebook-client/useNotebookClient";

const shellStyle: CSSProperties = {
    minHeight: "100dvh",
    background:
        "radial-gradient(circle at 8% 10%, rgba(20, 184, 166, 0.22), transparent 28%), radial-gradient(circle at 88% 14%, rgba(14, 165, 233, 0.16), transparent 34%), linear-gradient(155deg, #02050c 0%, #071322 45%, #0a1b2d 100%)",
    color: "#e2e8f0",
    padding: "28px clamp(16px, 2.2vw, 36px)",
    fontFamily: '"Rajdhani", "Segoe UI", sans-serif',
};

const panelStyle: CSSProperties = {
    width: "100%",
    maxWidth: 1440,
    margin: "0 auto",
    border: "1px solid rgba(45, 212, 191, 0.28)",
    borderRadius: 24,
    padding: "28px clamp(18px, 2.4vw, 36px)",
    background: "linear-gradient(170deg, rgba(3, 10, 19, 0.85), rgba(1, 7, 14, 0.95))",
    boxShadow: "0 24px 90px rgba(2, 6, 23, 0.52)",
    overflow: "hidden",
};

const missionStackStyle: CSSProperties = {
    display: "grid",
    gap: 0,
    marginTop: 18,
    gridTemplateColumns: "minmax(0, 1fr)",
    alignItems: "start",
};

const dashboardGridStyle: CSSProperties = {
    display: "grid",
    gridTemplateColumns: "minmax(320px, 360px) minmax(0, 1fr)",
    gap: 16,
    marginTop: 18,
    alignItems: "start",
};

const viewportPanelStyle: CSSProperties = {
    gridColumn: "1 / -1",
    position: 'relative',
};

export function DataSciencePlaygroundPage() {
    const { loading, manifest, manifestError, manifestSource, notebookError, notebookSource, cellLookup, notebookCellCount } = useNotebookClient();
    const [query, setQuery] = useState("");
    const [selectedFile, setSelectedFile] = useState<string>("");

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
    const notebookAvailabilityLabel = manifest ? "Artifacts loaded and indexed." : (loading ? "Loading notebook artifacts..." : "Artifacts unavailable. Run scaffold workflow.");

    return (
        <main style={shellStyle}>
            <section style={panelStyle}>
                <MainMenuPanel
                    indexedFileCount={manifest?.file_count ?? 0}
                    notebookCellCount={notebookCellCount}
                    manifestAvailable={Boolean(manifest)}
                />

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
                        notebookManifestHref="/notebooks/catalog-index.json"
                        notebookPayloadHref="/notebooks/native-c-catalog.ipynb"
                        notebookAvailabilityLabel={notebookAvailabilityLabel}
                    />

                    <div style={missionStackStyle}>
                        <div style={viewportPanelStyle}>
                            <NotebookGameplaySurface selectedFile={selectedFile} selectedContent={selectedContent} />
                            <NotebookOperationsPanel
                                selectedFile={selectedFile}
                                selectedContent={selectedContent}
                                indexedFileCount={manifest?.file_count ?? 0}
                                notebookCellCount={notebookCellCount}
                                loading={loading}
                                manifestError={manifestError}
                                notebookError={notebookError}
                                manifestSource={manifestSource}
                                notebookSource={notebookSource}
                                overlay
                            />
                        </div>
                    </div>
                </section>
            </section>
        </main>
    );
}
