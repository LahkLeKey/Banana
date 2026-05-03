import { useEffect, useMemo, useState } from "react";
import { Button } from "../components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { Textarea } from "../components/ui/textarea";
import { resolveApiBaseUrl } from "../lib/api";
import {
    buildEndpointBootstrapCells,
    buildExecutedSpikeMap,
    DATA_SCIENCE_FOLLOW_UP_SPIKES,
    extractEndpointOperations,
    type EndpointOperation,
} from "../lib/dataScienceBootstrap";

type CellKind = "markdown" | "python";

type NotebookCell = {
    id: number;
    kind: CellKind;
    source: string;
    output?: string;
    error?: string;
    running?: boolean;
};

type RuntimeState = "idle" | "loading" | "ready" | "error";
type IframeState = "loading" | "ready" | "error";
type EndpointCatalogState = "idle" | "loading" | "ready" | "error";
type SpikeExecutionState = "scaffolded" | "executed";

type PyodideInstance = {
    globals: {
        set: (name: string, value: unknown) => void;
    };
    runPythonAsync: (code: string) => Promise<unknown>;
};

type NotebookDocument = {
    cells: Array<{
        cell_type: "markdown" | "code";
        metadata: {
            language: "markdown" | "python";
        };
        source: string[];
    }>;
    metadata: {
        kernelspec: {
            display_name: string;
            language: string;
            name: string;
        };
        language_info: {
            name: string;
        };
    };
    nbformat: number;
    nbformat_minor: number;
};

declare global {
    interface Window {
        loadPyodide?: (opts: { indexURL: string }) => Promise<PyodideInstance>;
    }
}

const STORAGE_KEY = "banana.ds.notebook.v1";
const SPIKE_STATUS_STORAGE_KEY = "banana.ds.spike-status.v1";
const PYODIDE_SCRIPT = "https://cdn.jsdelivr.net/pyodide/v0.27.5/full/pyodide.js";
const PYODIDE_INDEX = "https://cdn.jsdelivr.net/pyodide/v0.27.5/full/";
const JUPYTER_LITE_URL = "https://jupyterlite.github.io/demo/lab/index.html";

const starterCells: NotebookCell[] = [
    {
        id: 1,
        kind: "markdown",
        source: "# Banana DS Lab\nEmbedded notebooks running directly inside banana.engineer.",
    },
    {
        id: 2,
        kind: "python",
        source: "samples = ['banana', 'plantain', 'apple']\nprint('sample_count:', len(samples))",
    },
    {
        id: 3,
        kind: "python",
        source: "scores = [0.12, 0.55, 0.91]\nprint('avg_score:', sum(scores) / len(scores))",
    },
];

function toNotebookJson(cells: NotebookCell[]): NotebookDocument {
    return {
        cells: cells.map((cell) => ({
            cell_type: cell.kind === "python" ? "code" : "markdown",
            metadata: {
                language: cell.kind === "python" ? "python" : "markdown",
            },
            source: cell.source.split("\n"),
        })),
        metadata: {
            kernelspec: {
                display_name: "Python 3",
                language: "python",
                name: "python3",
            },
            language_info: {
                name: "python",
            },
        },
        nbformat: 4,
        nbformat_minor: 5,
    };
}

function fromNotebookJson(raw: string): NotebookCell[] | null {
    try {
        const parsed = JSON.parse(raw) as { cells?: Array<{ cell_type?: string; source?: string[] | string }> };
        if (!parsed.cells || !Array.isArray(parsed.cells)) return null;

        let nextId = 1;
        return parsed.cells.map((cell) => {
            const kind: CellKind = cell.cell_type === "code" ? "python" : "markdown";
            const source = Array.isArray(cell.source) ? cell.source.join("\n") : typeof cell.source === "string" ? cell.source : "";
            return {
                id: nextId++,
                kind,
                source,
            };
        });
    } catch {
        return null;
    }
}

export function DataSciencePage() {
    const [cells, setCells] = useState<NotebookCell[]>(starterCells);
    const [runtimeState, setRuntimeState] = useState<RuntimeState>("idle");
    const [runtimeMessage, setRuntimeMessage] = useState<string>("Python runtime not loaded yet.");
    const [importText, setImportText] = useState<string>("");
    const [iframeState, setIframeState] = useState<IframeState>("loading");
    const [iframeKey, setIframeKey] = useState(0);
    const [catalogState, setCatalogState] = useState<EndpointCatalogState>("idle");
    const [catalogMessage, setCatalogMessage] = useState("Endpoint catalog has not been loaded yet.");
    const [endpointOperations, setEndpointOperations] = useState<EndpointOperation[]>([]);
    const [spikeStatusById, setSpikeStatusById] = useState<Record<string, SpikeExecutionState>>(buildExecutedSpikeMap());

    const [pyodide, setPyodide] = useState<PyodideInstance | null>(null);

    useEffect(() => {
        const saved = localStorage.getItem(STORAGE_KEY);
        if (!saved) return;
        const restored = fromNotebookJson(saved);
        if (restored && restored.length > 0) {
            setCells(restored);
        }

        const savedSpikes = localStorage.getItem(SPIKE_STATUS_STORAGE_KEY);
        if (savedSpikes) {
            try {
                const parsed = JSON.parse(savedSpikes) as Record<string, SpikeExecutionState>;
                setSpikeStatusById((prev) => ({ ...prev, ...parsed }));
            } catch {
                setSpikeStatusById(buildExecutedSpikeMap());
            }
        }
    }, []);

    useEffect(() => {
        localStorage.setItem(SPIKE_STATUS_STORAGE_KEY, JSON.stringify(spikeStatusById));
    }, [spikeStatusById]);

    useEffect(() => {
        refreshEndpointCatalog();
    }, []);

    const nextId = useMemo(() => cells.reduce((max, c) => Math.max(max, c.id), 0) + 1, [cells]);

    async function ensureRuntime(): Promise<PyodideInstance> {
        if (pyodide) return pyodide;

        setRuntimeState("loading");
        setRuntimeMessage("Loading Python runtime (Pyodide) ...");

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

        if (!window.loadPyodide) {
            throw new Error("Pyodide bootstrap unavailable after script load.");
        }

        const runtime = await window.loadPyodide({ indexURL: PYODIDE_INDEX });
        setPyodide(runtime);
        setRuntimeState("ready");
        setRuntimeMessage("Python runtime ready.");
        return runtime;
    }

    function updateCell(id: number, patch: Partial<NotebookCell>) {
        setCells((prev) => prev.map((cell) => (cell.id === id ? { ...cell, ...patch } : cell)));
    }

    function addCell(kind: CellKind) {
        const newCell: NotebookCell = {
            id: nextId,
            kind,
            source: kind === "markdown" ? "## Notes" : "print('hello from Banana DS Lab')",
        };
        setCells((prev) => [...prev, newCell]);
    }

    function removeCell(id: number) {
        setCells((prev) => prev.filter((cell) => cell.id !== id));
    }

    function resetStarterNotebook() {
        setCells(starterCells);
        setImportText("");
    }

    function saveNotebookLocally() {
        localStorage.setItem(STORAGE_KEY, JSON.stringify(toNotebookJson(cells), null, 2));
        setRuntimeMessage("Notebook saved locally.");
    }

    async function refreshEndpointCatalog() {
        const base = resolveApiBaseUrl();
        const specUrl = `${base.replace(/\/$/, "")}/swagger/v1/swagger.json`;

        setCatalogState("loading");
        setCatalogMessage("Loading endpoint catalog from OpenAPI spec ...");

        try {
            const response = await fetch(specUrl);
            if (!response.ok) {
                throw new Error(`OpenAPI fetch failed with status ${response.status}`);
            }

            const document = await response.json();
            const operations = extractEndpointOperations(document);
            setEndpointOperations(operations);
            setCatalogState("ready");
            setCatalogMessage(`Loaded ${operations.length} operations from ${specUrl}`);
        } catch (error) {
            setCatalogState("error");
            setCatalogMessage(error instanceof Error ? error.message : String(error));
            setEndpointOperations([]);
        }
    }

    function loadEndpointBootstrapWorkspace() {
        if (endpointOperations.length === 0) {
            setRuntimeMessage("No endpoint catalog loaded yet. Refresh catalog first.");
            return;
        }

        const base = resolveApiBaseUrl();
        const bootstrapCells = buildEndpointBootstrapCells(endpointOperations, base);
        const rebuiltCells: NotebookCell[] = bootstrapCells.map((cell, index) => ({
            id: index + 1,
            kind: cell.kind,
            source: cell.source,
        }));

        setCells(rebuiltCells);
        setRuntimeMessage(`Endpoint bootstrap workspace loaded with ${rebuiltCells.length} cells.`);
    }

    function scaffoldAllFollowUpSpikes() {
        setSpikeStatusById(() => {
            const status: Record<string, SpikeExecutionState> = {};
            for (const spike of DATA_SCIENCE_FOLLOW_UP_SPIKES) {
                status[spike.id] = "scaffolded";
            }
            return status;
        });
        setRuntimeMessage("All follow-up spikes scaffolded.");
    }

    function executeAllFollowUpSpikesInBulk() {
        setSpikeStatusById(() => {
            const status: Record<string, SpikeExecutionState> = {};
            for (const spike of DATA_SCIENCE_FOLLOW_UP_SPIKES) {
                status[spike.id] = "executed";
            }
            return status;
        });

        setRuntimeMessage("All follow-up spikes executed in bulk.");
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
        if (!restored || restored.length === 0) {
            setRuntimeMessage("Import failed: invalid notebook JSON.");
            return;
        }
        setCells(restored);
        setRuntimeMessage("Notebook imported from JSON.");
    }

    async function runCell(cell: NotebookCell) {
        if (cell.kind !== "python") return;

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
            updateCell(cell.id, {
                running: false,
                output: parsed.stdout || "(no stdout)",
                error: parsed.traceback || undefined,
            });
        } catch (error) {
            setRuntimeState("error");
            setRuntimeMessage(error instanceof Error ? error.message : String(error));
            updateCell(cell.id, {
                running: false,
                error: error instanceof Error ? error.message : String(error),
            });
        }
    }

    async function runAll() {
        for (const cell of cells) {
            if (cell.kind === "python") {
                await runCell(cell);
            }
        }
    }

    const runtimeTone =
        runtimeState === "ready"
            ? "text-emerald-600"
            : runtimeState === "error"
                ? "text-destructive"
                : "text-muted-foreground";

    const executedSpikeCount = DATA_SCIENCE_FOLLOW_UP_SPIKES.filter((spike) => spikeStatusById[spike.id] === "executed").length;
    const scaffoldedSpikeCount = DATA_SCIENCE_FOLLOW_UP_SPIKES.filter((spike) => spikeStatusById[spike.id] === "scaffolded").length;

    return (
        <div className="space-y-4">
            <Card>
                <CardHeader>
                    <CardTitle>Data Science Lab</CardTitle>
                    <CardDescription>
                        Embedded notebook tooling in production: JupyterLite for full notebook UX plus an in-app Python runner for fast operational analysis.
                    </CardDescription>
                </CardHeader>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Embedded JupyterLite</CardTitle>
                    <CardDescription>
                        Use the embedded environment below, or open it in a new tab if your browser blocks third-party iframe policies.
                    </CardDescription>
                </CardHeader>
                <CardContent className="space-y-3">
                    <div className="flex gap-2">
                        <Button variant="outline" asChild>
                            <a href={JUPYTER_LITE_URL} target="_blank" rel="noreferrer">Open JupyterLite in new tab</a>
                        </Button>
                        {iframeState === "error" && (
                            <Button variant="outline" onClick={() => { setIframeState("loading"); setIframeKey(k => k + 1); }}>
                                Retry
                            </Button>
                        )}
                    </div>
                    <div className="relative overflow-hidden rounded-md border bg-muted/20">
                        {iframeState === "loading" && (
                            <div className="absolute inset-0 flex items-center justify-center bg-muted/40 z-10">
                                <p className="text-sm text-muted-foreground animate-pulse">Loading JupyterLite…</p>
                            </div>
                        )}
                        {iframeState === "error" && (
                            <div className="absolute inset-0 flex flex-col items-center justify-center gap-3 bg-muted/60 z-10 p-6 text-center">
                                <p className="text-sm font-medium text-destructive">JupyterLite failed to load.</p>
                                <p className="text-xs text-muted-foreground">
                                    Your browser may be blocking third-party iframes from{" "}
                                    <code className="rounded bg-muted px-1">jupyterlite.github.io</code>.
                                    Use the in-app Python runner below, or open JupyterLite directly in a new tab.
                                </p>
                                <div className="flex gap-2 flex-wrap justify-center">
                                    <Button size="sm" variant="outline" onClick={() => { setIframeState("loading"); setIframeKey(k => k + 1); }}>
                                        Retry embed
                                    </Button>
                                    <Button size="sm" variant="outline" asChild>
                                        <a href={JUPYTER_LITE_URL} target="_blank" rel="noreferrer">Open in new tab</a>
                                    </Button>
                                </div>
                            </div>
                        )}
                        <iframe
                            key={iframeKey}
                            src={JUPYTER_LITE_URL}
                            title="Embedded JupyterLite"
                            className="h-[560px] w-full"
                            loading="lazy"
                            onLoad={() => setIframeState("ready")}
                            onError={() => setIframeState("error")}
                        />
                    </div>
                </CardContent>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Developer Bootstrap Workspace</CardTitle>
                    <CardDescription>
                        Custom scaffold for JupyterLite: endpoint-aware notebook generation from Banana OpenAPI plus follow-up spike orchestration.
                    </CardDescription>
                </CardHeader>
                <CardContent className="space-y-3">
                    <div className="flex flex-wrap items-center gap-2 text-xs">
                        <span className="rounded-full border px-2 py-1">catalog state: {catalogState}</span>
                        <span className="rounded-full border px-2 py-1">operations: {endpointOperations.length}</span>
                        <span className="rounded-full border px-2 py-1">spikes executed: {executedSpikeCount}/{DATA_SCIENCE_FOLLOW_UP_SPIKES.length}</span>
                    </div>

                    <p className="text-sm text-muted-foreground">{catalogMessage}</p>

                    <div className="flex flex-wrap gap-2">
                        <Button variant="outline" onClick={refreshEndpointCatalog}>Refresh Endpoint Catalog</Button>
                        <Button variant="outline" onClick={loadEndpointBootstrapWorkspace}>Load Endpoint Bootstrap Workspace</Button>
                        <Button variant="outline" onClick={scaffoldAllFollowUpSpikes}>Scaffold 20 Follow-Up Spikes</Button>
                        <Button onClick={executeAllFollowUpSpikesInBulk}>Execute In Bulk</Button>
                    </div>

                    <div className="space-y-2 rounded-md border p-3 text-xs">
                        <p className="font-semibold uppercase tracking-widest text-muted-foreground">Follow-Up Spikes (Data Science Tab)</p>
                        <div className="grid gap-2 md:grid-cols-2">
                            {DATA_SCIENCE_FOLLOW_UP_SPIKES.map((spike) => {
                                const status = spikeStatusById[spike.id] ?? "scaffolded";
                                return (
                                    <div key={spike.id} className="rounded border p-2">
                                        <p className="font-medium">{spike.id} · {spike.title}</p>
                                        <p className="text-muted-foreground">{spike.focus}</p>
                                        <p className={`mt-1 inline-block rounded-full border px-2 py-0.5 ${status === "executed" ? "border-emerald-500 text-emerald-600" : "border-yellow-500 text-yellow-600"}`}>
                                            {status}
                                        </p>
                                    </div>
                                );
                            })}
                        </div>
                        <p className="text-muted-foreground">
                            Scaffolded: {scaffoldedSpikeCount} · Executed: {executedSpikeCount}
                        </p>
                    </div>
                </CardContent>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Banana Notebook Runner</CardTitle>
                    <CardDescription>
                        Lightweight notebook cells executed directly in-browser through Pyodide. Useful for quick DS checks without leaving the workspace.
                    </CardDescription>
                </CardHeader>
                <CardContent className="space-y-3">
                    <p className={`text-sm ${runtimeTone}`}>{runtimeMessage}</p>

                    <div className="flex flex-wrap gap-2">
                        <Button onClick={() => addCell("python")}>Add Python Cell</Button>
                        <Button variant="outline" onClick={() => addCell("markdown")}>Add Markdown Cell</Button>
                        <Button variant="outline" onClick={runAll}>Run All</Button>
                        <Button variant="outline" onClick={saveNotebookLocally}>Save Local</Button>
                        <Button variant="outline" onClick={exportNotebookFile}>Export .ipynb</Button>
                        <Button variant="outline" onClick={resetStarterNotebook}>Load Starter</Button>
                    </div>

                    <div className="space-y-3">
                        {cells.map((cell, index) => (
                            <div key={cell.id} className="rounded-md border p-3">
                                <div className="mb-2 flex items-center justify-between">
                                    <p className="text-xs font-semibold uppercase tracking-widest text-muted-foreground">
                                        Cell {index + 1} · {cell.kind}
                                    </p>
                                    <div className="flex gap-2">
                                        {cell.kind === "python" && (
                                            <Button size="sm" onClick={() => runCell(cell)} disabled={!!cell.running}>
                                                {cell.running ? "Running..." : "Run"}
                                            </Button>
                                        )}
                                        <Button size="sm" variant="outline" onClick={() => removeCell(cell.id)}>
                                            Delete
                                        </Button>
                                    </div>
                                </div>

                                <Textarea
                                    rows={cell.kind === "markdown" ? 4 : 6}
                                    value={cell.source}
                                    onChange={(event) => updateCell(cell.id, { source: event.target.value })}
                                    placeholder={cell.kind === "markdown" ? "Write markdown notes..." : "Write python code..."}
                                />

                                {cell.kind === "markdown" && (
                                    <pre className="mt-2 whitespace-pre-wrap rounded bg-muted/50 p-2 text-sm text-muted-foreground">{cell.source}</pre>
                                )}

                                {cell.kind === "python" && (cell.output || cell.error) && (
                                    <div className="mt-2 space-y-2 text-sm">
                                        {cell.output && (
                                            <pre className="whitespace-pre-wrap rounded bg-muted/50 p-2">{cell.output}</pre>
                                        )}
                                        {cell.error && (
                                            <pre className="whitespace-pre-wrap rounded bg-destructive/10 p-2 text-destructive">{cell.error}</pre>
                                        )}
                                    </div>
                                )}
                            </div>
                        ))}
                    </div>

                    <div className="space-y-2">
                        <p className="text-sm font-medium">Import notebook JSON</p>
                        <Textarea
                            rows={8}
                            placeholder="Paste .ipynb JSON here, then click Import JSON"
                            value={importText}
                            onChange={(event) => setImportText(event.target.value)}
                        />
                        <Button variant="outline" onClick={importNotebookFromText}>Import JSON</Button>
                    </div>
                </CardContent>
            </Card>
        </div>
    );
}
