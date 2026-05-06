import { useEffect, useMemo, useRef, useState } from "react";
import { resolveApiBaseUrl } from "../lib/api";
import { buildEndpointBootstrapCells, extractEndpointOperations, type EndpointOperation } from "../lib/dataScienceBootstrap";
import { NOTEBOOK_LIBRARY, PYODIDE_INDEX, PYODIDE_SCRIPT, RUNTIME_CONTRACTS, STORAGE_KEY, starterCells } from "../lib/dsConstants";
import { fromNotebookJson, parseRichOutput, toNotebookJson, withNotebookDefaults } from "../lib/dsNotebook";
import type { EndpointCatalogState, NotebookCell, NotebookLibraryEntry, PyodideInstance, RuntimeState } from "../lib/dsTypes";
import { AddCellRow } from "../components/ds/AddCellRow";
import { AdvancedTools } from "../components/ds/AdvancedTools";
import { CellEditor } from "../components/ds/CellEditor";
import { CellStack } from "../components/ds/CellStack";
import { NotebookCatalog } from "../components/ds/NotebookCatalog";
import { NotebookReportView } from "../components/ds/NotebookReportView";
import { NotebookToolbar } from "../components/ds/NotebookToolbar";
import { OperationsRail } from "../components/ds/OperationsRail";

export function DataSciencePage() {
    const [cells, setCells] = useState<NotebookCell[]>(withNotebookDefaults(starterCells));
    const [runtimeState, setRuntimeState] = useState<RuntimeState>("idle");
    const [runtimeMessage, setRuntimeMessage] = useState<string>("Python runtime not loaded yet.");
    const [importText, setImportText] = useState<string>("");
    const [catalogState, setCatalogState] = useState<EndpointCatalogState>("idle");
    const [endpointOperations, setEndpointOperations] = useState<EndpointOperation[]>([]);
    const [showApiTools, setShowApiTools] = useState(false);
    const [endpointFilter, setEndpointFilter] = useState("");
    const [showLibrary, setShowLibrary] = useState(false);
    const [showAdvancedTools, setShowAdvancedTools] = useState(false);
    const [showCellStack, setShowCellStack] = useState(false);
    const [reportMode, setReportMode] = useState(false);
    const [activeNotebookLabel, setActiveNotebookLabel] = useState("Starter Notebook");
    const [activeCellId, setActiveCellId] = useState<number | null>(starterCells[0]?.id ?? null);
    const [pyodide, setPyodide] = useState<PyodideInstance | null>(null);
    const execCounterRef = useRef(0);

    // Restore local draft on mount
    useEffect(() => {
        const saved = localStorage.getItem(STORAGE_KEY);
        if (saved) {
            const restored = fromNotebookJson(saved);
            if (restored && restored.length > 0) {
                setCells(withNotebookDefaults(restored));
                setActiveNotebookLabel("Local Draft");
                setActiveCellId(restored[0]?.id ?? null);
            }
        }
    }, []);

    useEffect(() => { void refreshEndpointCatalog(); }, []);

    // Keep activeCellId pointing at a real cell
    useEffect(() => {
        if (cells.length === 0) { setActiveCellId(null); return; }
        if (!cells.some((c) => c.id === activeCellId)) setActiveCellId(cells[0]?.id ?? null);
    }, [activeCellId, cells]);

    const nextId = useMemo(() => cells.reduce((max, c) => Math.max(max, c.id), 0) + 1, [cells]);
    const executedCount = useMemo(() => cells.filter((c) => c.execCount != null).length, [cells]);
    const filteredOperations = useMemo(() => {
        const q = endpointFilter.trim().toLowerCase();
        if (!q) return endpointOperations;
        return endpointOperations.filter((op) => `${op.method} ${op.path} ${op.summary}`.toLowerCase().includes(q));
    }, [endpointFilter, endpointOperations]);

    function flattenNotebookText(value: string) {
        return value.replace(/[\r\n\t]+/g, " ").replace(/\s{2,}/g, " ").trim();
    }

    // -------------------------------------------------------------------------
    // Cell management
    // -------------------------------------------------------------------------
    function updateCell(id: number, patch: Partial<NotebookCell>) {
        setCells((prev) => prev.map((c) => (c.id === id ? { ...c, ...patch } : c)));
    }

    function addCell(kind: "python" | "markdown") {
        setCells((prev) => [
            ...prev,
            {
                id: nextId,
                kind,
                source: kind === "markdown" ? "## Notes" : "print('hello from Banana DS Notebook')",
                previewMode: kind === "markdown",
            },
        ]);
        setActiveCellId(nextId);
    }

    function removeCell(id: number) { setCells((prev) => prev.filter((c) => c.id !== id)); }

    function moveCell(id: number, direction: "up" | "down") {
        setCells((prev) => {
            const idx = prev.findIndex((c) => c.id === id);
            if (idx < 0) return prev;
            const next = direction === "up" ? idx - 1 : idx + 1;
            if (next < 0 || next >= prev.length) return prev;
            const arr = [...prev];
            [arr[idx], arr[next]] = [arr[next], arr[idx]];
            return arr;
        });
    }

    function duplicateCell(id: number) {
        setCells((prev) => {
            const idx = prev.findIndex((c) => c.id === id);
            if (idx < 0) return prev;
            const src = prev[idx];
            const newCell = { ...src, id: Date.now(), execCount: undefined, output: undefined, error: undefined, richOutput: undefined, running: false };
            const arr = [...prev];
            arr.splice(idx + 1, 0, newCell);
            return arr;
        });
    }

    function clearOutputs() {
        setCells((prev) => prev.map((c) => ({ ...c, output: undefined, error: undefined, richOutput: undefined, execCount: undefined, running: false })));
        execCounterRef.current = 0;
        setRuntimeMessage("Outputs cleared.");
    }

    // -------------------------------------------------------------------------
    // Notebook persistence
    // -------------------------------------------------------------------------
    function resetStarterNotebook() {
        setCells(withNotebookDefaults(starterCells));
        setImportText("");
        setActiveNotebookLabel("Starter Notebook");
        setRuntimeMessage("Starter notebook loaded.");
        setActiveCellId(starterCells[0]?.id ?? null);
    }

    function saveNotebookLocally() {
        localStorage.setItem(STORAGE_KEY, JSON.stringify(toNotebookJson(cells), null, 2));
        setRuntimeMessage("Notebook saved locally.");
    }

    function exportNotebookFile() {
        const payload = JSON.stringify(toNotebookJson(cells), null, 2);
        const blob = new Blob([payload], { type: "application/json" });
        const link = document.createElement("a");
        link.href = URL.createObjectURL(blob);
        link.download = "banana-ds-lab.ipynb";
        link.click();
        URL.revokeObjectURL(link.href);
    }

    function importNotebookFromText() {
        const restored = fromNotebookJson(importText);
        if (!restored || restored.length === 0) { setRuntimeMessage("Import failed: invalid notebook JSON."); return; }
        setCells(withNotebookDefaults(restored));
        setActiveNotebookLabel("Imported Notebook");
        setRuntimeMessage("Notebook imported from JSON.");
        setActiveCellId(restored[0]?.id ?? null);
    }

    // -------------------------------------------------------------------------
    // Endpoint catalog
    // -------------------------------------------------------------------------
    async function refreshEndpointCatalog() {
        const base = resolveApiBaseUrl();
        const specUrl = `${base.replace(/\/$/, "")}/swagger/v1/swagger.json`;
        setCatalogState("loading");
        try {
            const response = await fetch(specUrl);
            if (!response.ok) throw new Error(`OpenAPI fetch failed: ${response.status}`);
            const document = await response.json();
            const operations = extractEndpointOperations(document);
            setEndpointOperations(operations);
            setCatalogState("ready");
        } catch {
            setCatalogState("error");
            setEndpointOperations([]);
        }
    }

    function loadEndpointBootstrapWorkspace() {
        if (endpointOperations.length === 0) { setRuntimeMessage("No endpoint catalog loaded yet. Refresh catalog first."); return; }
        const bootstrapCells = buildEndpointBootstrapCells(endpointOperations, resolveApiBaseUrl());
        const rebuiltCells: NotebookCell[] = bootstrapCells.map((cell, idx) => ({
            id: idx + 1, kind: cell.kind, source: cell.source, previewMode: cell.kind === "markdown",
        }));
        setCells(withNotebookDefaults(rebuiltCells));
        setActiveNotebookLabel("API Bootstrap Workspace");
        setRuntimeMessage(`Bootstrap workspace loaded with ${rebuiltCells.length} cells.`);
        setActiveCellId(rebuiltCells[0]?.id ?? null);
    }

    function injectEndpointCell(op: EndpointOperation) {
        const base = resolveApiBaseUrl().replace(/\/$/, "");
        const hasBody = ["POST", "PUT", "PATCH"].includes(op.method);
        const id = Date.now();
        const safeSummary = flattenNotebookText(op.summary);
        const lines = [
            `# ${op.method} ${op.path} - ${safeSummary}`,
            "import json",
            "from js import XMLHttpRequest",
            `API_BASE = '${base}'`,
            "def call_endpoint(method, path, payload=None):",
            "    url = f\"{API_BASE}{path}\"",
            "    try:",
            "        xhr = XMLHttpRequest.new()",
            "        xhr.open(method.upper(), url, False)",
            "        xhr.setRequestHeader('Accept', 'application/json')",
            "        body = None",
            "        if payload is not None:",
            "            xhr.setRequestHeader('Content-Type', 'application/json')",
            "            body = json.dumps(payload)",
            "        xhr.send(body)",
            "        return {'ok': 200 <= int(xhr.status) < 300, 'status': int(xhr.status), 'body': str(xhr.responseText)}",
            "    except Exception as ex:",
            "        return {'ok': False, 'status': None, 'body': str(ex)}",
            hasBody ? `payload = {}\nresult = call_endpoint('${op.method}', '${op.path}', payload)` : `result = call_endpoint('${op.method}', '${op.path}')`,
            "print('status:', result['status'])",
            "print(result['body'][:2000])",
        ];
        setCells((prev) => [...prev, { id, kind: "python", source: lines.join("\n") }]);
        setRuntimeMessage(`Injected cell for ${op.method} ${op.path}`);
        setActiveCellId(id);
    }

    function addScaffoldCell(source: string) {
        const id = Date.now();
        setCells((prev) => [...prev, { id, kind: "python", source }]);
        setActiveCellId(id);
    }

    // -------------------------------------------------------------------------
    // Python runtime (Pyodide)
    // -------------------------------------------------------------------------
    async function ensureRuntime(): Promise<PyodideInstance> {
        if (pyodide) return pyodide;
        setRuntimeState("loading");
        setRuntimeMessage("Loading Python runtime (Pyodide)…");
        if (!window.loadPyodide) {
            await new Promise<void>((resolve, reject) => {
                const script = document.createElement("script");
                script.src = PYODIDE_SCRIPT;
                script.async = true;
                script.onload = () => resolve();
                script.onerror = () => reject(new Error("Failed to load Pyodide script."));
                document.head.appendChild(script);
            });
        }
        if (!window.loadPyodide) throw new Error("Pyodide bootstrap unavailable after script load.");
        const runtime = await window.loadPyodide({ indexURL: PYODIDE_INDEX });
        setPyodide(runtime);
        setRuntimeState("ready");
        setRuntimeMessage("Python runtime ready.");
        return runtime;
    }

    async function runCell(cell: NotebookCell) {
        if (cell.kind !== "python") return;
        setActiveCellId(cell.id);
        updateCell(cell.id, { running: true, error: undefined });
        try {
            const runtime = await ensureRuntime();
            runtime.globals.set("banana_cell_code", cell.source);
            const raw = await runtime.runPythonAsync(`
import io
import contextlib
import traceback
import json

_stdout = io.StringIO()
_tb = None

try:
    with contextlib.redirect_stdout(_stdout):
        exec(banana_cell_code, globals())
except Exception:
    _tb = traceback.format_exc()

json.dumps({"stdout": _stdout.getvalue(), "traceback": _tb})
`);
            const parsed = JSON.parse(String(raw)) as { stdout?: string; traceback?: string | null };
            const parsedOutput = parseRichOutput(parsed.stdout || "");
            const count = ++execCounterRef.current;
            updateCell(cell.id, {
                running: false,
                output: parsedOutput.cleanStdout || "(no stdout)",
                error: parsed.traceback || undefined,
                richOutput: parsedOutput.richOutput,
                execCount: count,
            });
        } catch (error) {
            setRuntimeState("error");
            setRuntimeMessage(error instanceof Error ? error.message : String(error));
            updateCell(cell.id, { running: false, error: error instanceof Error ? error.message : String(error), richOutput: undefined });
        }
    }

    async function runAll() {
        for (const cell of cells) {
            if (cell.kind === "python") await runCell(cell);
        }
    }

    async function loadNotebookFromLibrary(entry: NotebookLibraryEntry) {
        setRuntimeMessage(`Loading ${entry.label}…`);
        try {
            const response = await fetch(entry.path);
            if (!response.ok) throw new Error(`HTTP ${response.status}`);
            const raw = await response.text();
            const restored = fromNotebookJson(raw);
            if (!restored || restored.length === 0) throw new Error("Invalid notebook JSON");
            setCells(withNotebookDefaults(restored));
            setActiveNotebookLabel(entry.label);
            setRuntimeMessage(`Loaded: ${entry.label} (${restored.length} cells)`);
            setActiveCellId(restored[0]?.id ?? null);
        } catch (error) {
            setRuntimeMessage(`Failed to load ${entry.label}: ${error instanceof Error ? error.message : String(error)}`);
        }
    }

    // -------------------------------------------------------------------------
    // Layout — light-first notebook theme, component-driven
    // -------------------------------------------------------------------------
    return (
        <div className="space-y-3">
            <NotebookToolbar
                notebookLabel={activeNotebookLabel}
                cellCount={cells.length}
                executedCount={executedCount}
                runtimeState={runtimeState}
                runtimeMessage={runtimeMessage}
                showLibrary={showLibrary}
                onRunAll={() => void runAll()}
                onSave={saveNotebookLocally}
                onExport={exportNotebookFile}
                onReset={resetStarterNotebook}
                onToggleCatalog={() => setShowLibrary(true)}
                onWarmRuntime={() => void ensureRuntime().catch(() => undefined)}
            />

            <div className="space-y-3 min-w-0">
                <section className="space-y-3 rounded-xl border border-slate-200 bg-white p-3 min-w-0">
                    <div className="flex flex-wrap items-center justify-between gap-2 rounded-lg border border-slate-200 bg-white px-3 py-2">
                        <p className="text-xs font-semibold uppercase tracking-wider text-slate-500">
                            Notebook Workspace
                        </p>
                        <div className="flex items-center gap-2">
                            <button
                                type="button"
                                onClick={() => setShowLibrary((v) => !v)}
                                className="rounded border border-slate-200 px-2 py-1 text-[11px] font-semibold text-slate-600 hover:bg-slate-50"
                                data-testid="notebook-catalog-toggle"
                            >
                                {showLibrary ? "Close Catalog" : "Notebook Catalog"}
                            </button>
                            <button
                                type="button"
                                onClick={() => setShowCellStack((v) => !v)}
                                className="rounded border border-slate-200 px-2 py-1 text-[11px] font-semibold text-slate-600 hover:bg-slate-50"
                            >
                                {showCellStack ? "Close Cell Stack" : "Cell Stack"}
                            </button>
                            <button
                                type="button"
                                onClick={() => setReportMode((v) => !v)}
                                className={`rounded border px-2 py-1 text-[11px] font-semibold transition-colors ${reportMode ? "border-indigo-300 bg-indigo-50 text-indigo-700" : "border-slate-200 text-slate-600 hover:bg-slate-50"}`}
                                title="Toggle report mode — collapses editors and shows outputs inline"
                            >
                                {reportMode ? "✕ Report" : "Report"}
                            </button>
                            <button
                                type="button"
                                onClick={clearOutputs}
                                className="rounded border border-slate-200 px-2 py-1 text-[11px] font-semibold text-slate-500 hover:border-red-200 hover:bg-red-50 hover:text-red-600 transition-colors"
                                title="Clear all cell outputs"
                            >
                                Clear Outputs
                            </button>
                        </div>
                    </div>

                    {showLibrary && (
                        <NotebookCatalog
                            activeNotebookLabel={activeNotebookLabel}
                            library={NOTEBOOK_LIBRARY}
                            onLoadNotebook={(entry) => void loadNotebookFromLibrary(entry)}
                            onClose={() => setShowLibrary(false)}
                            onAddPythonCell={() => addCell("python")}
                            onAddMarkdownCell={() => addCell("markdown")}
                            onToggleAdvancedTools={() => setShowAdvancedTools((v) => !v)}
                            showAdvancedTools={showAdvancedTools}
                        />
                    )}

                    <main className="min-w-0 space-y-3">
                        {showAdvancedTools && (
                            <AdvancedTools
                                catalogState={catalogState}
                                endpointOperations={endpointOperations}
                                endpointFilter={endpointFilter}
                                onFilterChange={setEndpointFilter}
                                filteredOperations={filteredOperations}
                                showApiTools={showApiTools}
                                onToggleApiTools={() => setShowApiTools((v) => !v)}
                                onRefreshCatalog={() => void refreshEndpointCatalog()}
                                onInjectEndpoint={injectEndpointCell}
                                onAddScaffold={addScaffoldCell}
                                onLoadApiBootstrap={loadEndpointBootstrapWorkspace}
                            />
                        )}

                        {reportMode ? (
                            <NotebookReportView cells={cells} />
                        ) : (
                            <div className="space-y-2">
                                {cells.length === 0 ? (
                                    <div className="flex min-h-[280px] items-center justify-center rounded-lg border border-dashed border-slate-200 bg-white text-sm text-slate-400">
                                        Add a cell below to start.
                                    </div>
                                ) : (
                                    cells.map((cell, index) => (
                                        <CellEditor
                                            key={cell.id}
                                            cell={cell}
                                            cellIndex={index}
                                            isActive={cell.id === activeCellId}
                                            onActivate={() => setActiveCellId(cell.id)}
                                            onUpdateSource={(source) => updateCell(cell.id, { source })}
                                            onRun={() => void runCell(cell)}
                                            onDelete={() => removeCell(cell.id)}
                                            onTogglePreview={() => updateCell(cell.id, { previewMode: !(cell.previewMode ?? true) })}
                                            onMoveUp={index > 0 ? () => moveCell(cell.id, "up") : undefined}
                                            onMoveDown={index < cells.length - 1 ? () => moveCell(cell.id, "down") : undefined}
                                            onDuplicate={() => duplicateCell(cell.id)}
                                        />
                                    ))
                                )}

                                <AddCellRow
                                    onAddPython={() => addCell("python")}
                                    onAddMarkdown={() => addCell("markdown")}
                                />
                            </div>
                        )}
                    </main>
                </section>

                <section>
                    <OperationsRail
                        runtimeState={runtimeState}
                        runtimeMessage={runtimeMessage}
                        importText={importText}
                        onImportTextChange={setImportText}
                        onImport={importNotebookFromText}
                        contracts={RUNTIME_CONTRACTS}
                    />
                </section>
            </div>
        </div>
    );
}
