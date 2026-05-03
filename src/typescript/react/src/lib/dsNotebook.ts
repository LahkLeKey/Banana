import {
    BROWSER_HTTP_HELPER,
    PLOTLY_MAX_TRACE_POINTS,
    PLOTLY_SCRIPT,
    RUNTIME_CONTRACTS
} from "./dsConstants";
import type {
    CellKind, NotebookCell, NotebookDocument, NotebookRichOutput, PlotlyFigure, RuntimeContractCard,
    RuntimeContractDefinition, RuntimeContractFamily, RuntimeContractStatus,} from "./dsTypes";

const LIVE_SURFACE_FAMILIES = new Set<RuntimeContractFamily>([
    "vega",
    "altair",
    "bokeh",
    "widgets",
    "ipympl",
    "bqplot",
    "geospatial",
    "network",
    "three-d",
    "canvas",
]);

// ---------------------------------------------------------------------------
// Plotly loader
// ---------------------------------------------------------------------------
let plotlyLoadPromise: Promise<void>|null = null;

export function ensurePlotly(): Promise<void>
{
    if (window.Plotly)
        return Promise.resolve();
    if (plotlyLoadPromise)
        return plotlyLoadPromise;
    plotlyLoadPromise = new Promise<void>((resolve, reject) => {
        const script = document.createElement("script");
        script.src = PLOTLY_SCRIPT;
        script.async = true;
        script.onload = () => resolve();
        script.onerror = () => reject(new Error("Failed to load Plotly"));
        document.head.appendChild(script);
    });
    return plotlyLoadPromise;
}

// ---------------------------------------------------------------------------
// Minimal inline markdown renderer (no deps)
// ---------------------------------------------------------------------------
export function renderMarkdown(src: string): string
{
    return src.replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;")
        .split("\n")
        .map((line) => {
            if (/^######\s/.test(line))
                return `<h6 class="text-xs font-semibold mt-2">${line.slice(7)}</h6>`;
            if (/^#####\s/.test(line))
                return `<h5 class="text-sm font-semibold mt-2">${line.slice(6)}</h5>`;
            if (/^####\s/.test(line))
                return `<h4 class="text-sm font-bold mt-3">${line.slice(5)}</h4>`;
            if (/^###\s/.test(line))
                return `<h3 class="text-base font-bold mt-3">${line.slice(4)}</h3>`;
            if (/^##\s/.test(line))
                return `<h2 class="text-lg font-bold mt-4">${line.slice(3)}</h2>`;
            if (/^#\s/.test(line))
                return `<h1 class="text-xl font-bold mt-4">${line.slice(2)}</h1>`;
            if (/^[-*]\s/.test(line))
                return `<li class="ml-4 list-disc">${line.slice(2)}</li>`;
            if (/^\d+\.\s/.test(line))
                return `<li class="ml-4 list-decimal">${line.replace(/^\d+\.\s/, "")}</li>`;
            if (line.trim() === "")
                return "<br/>";
            return `<p class="leading-relaxed">${
                line.replace(/\*\*(.+?)\*\*/g, "<strong>$1</strong>")
                    .replace(/\*(.+?)\*/g, "<em>$1</em>")
                    .replace(
                        /`(.+?)`/g,
                        '<code class="rounded bg-muted px-1 font-mono text-xs">$1</code>')}</p>`;
        })
        .join("\n");
}

// ---------------------------------------------------------------------------
// Notebook serialization / deserialization
// ---------------------------------------------------------------------------
export function toNotebookJson(cells: NotebookCell[]): NotebookDocument
{
    return {
        cells : cells.map(
            (cell) => ({
                cell_type : cell.kind === "python" ? "code" : "markdown",
                metadata : {
                    language : cell.kind === "python" ? "python" : "markdown",
                    banana : {
                        output : cell.output,
                        error : cell.error,
                        execCount : cell.execCount,
                        previewMode : cell.previewMode,
                        richOutput : cell.richOutput,
                    },
                },
                source : cell.source.split("\n"),
                ...(cell.kind === "python" ? {
                    execution_count : cell.execCount ?? null,
                    outputs : [
                        ...(cell.output
                                ? [ {output_type : "stream", name : "stdout", text : cell.output} ]
                                : []),
                        ...(cell.error ? [ {
                            output_type : "error",
                            ename : "ExecutionError",
                            evalue : cell.error,
                            traceback : cell.error.split("\n")
                        } ]
                                       : []),
                    ],
                }
                                           : {}),
            })),
        metadata : {
            kernelspec : {display_name : "Python 3", language : "python", name : "python3"},
            language_info : {name : "python"},
        },
        nbformat : 4,
        nbformat_minor : 5,
    };
}

export function normalizeRichOutput(raw: unknown): NotebookRichOutput[]|undefined
{
    if (!Array.isArray(raw))
        return undefined;
    const normalized: NotebookRichOutput[] = [];

    for (const item of raw)
    {
        if (!item || typeof item !== "object")
            continue;
        const entry = item as Record<string, unknown>;
        const kind = entry.kind;

        if (kind === "plotly")
        {
            const fig = entry.figure as Record<string, unknown>| undefined;
            if (fig && Array.isArray(fig.data))
            {
                normalized.push({
                    kind : "plotly",
                    figure : {
                        data : fig.data,
                        layout : typeof fig.layout === "object" && fig.layout
                                     ? (fig.layout as Record<string, unknown>)
                                     : undefined,
                    },
                });
            }
            continue;
        }

        if (kind === "table")
        {
            const table = entry.table as Record<string, unknown>| undefined;
            const columns = table?.columns;
            const rows = table?.rows;
            if (Array.isArray(columns) && Array.isArray(rows))
            {
                normalized.push({
                    kind : "table",
                    table : {
                        columns : columns.map((v) => String(v)),
                        rows : rows.map(
                            (row) => Array.isArray(row)
                                         ? row.map((v) => (v == null               ? null
                                                           : typeof v === "number" ? v
                                                                                   : String(v)))
                                         : [],
                            ),
                    },
                });
            }
            continue;
        }

        if (kind === "notice")
        {
            const message = typeof entry.message === "string"
                                ? entry.message
                                : "Unsupported visualization payload.";
            const level = entry.level === "warning" ? "warning" : "info";
            normalized.push({kind : "notice", level, message});
            continue;
        }

        if (kind === "surface")
        {
            const family = typeof entry.family === "string"
                               ? (entry.family as RuntimeContractFamily)
                               : undefined;
            const title = typeof entry.title === "string" ? entry.title : undefined;
            const payload = entry.payload;
            if (family && LIVE_SURFACE_FAMILIES.has(family) && title && payload &&
                typeof payload === "object" && !Array.isArray(payload))
            {
                normalized.push({
                    kind : "surface",
                    family : family as Exclude<RuntimeContractFamily, "plotly"|"table">,
                    title,
                    summary : typeof entry.summary === "string" ? entry.summary : undefined,
                    payload : payload as Record<string, unknown>,
                });
            }
            continue;
        }

        if (kind === "contract")
        {
            const family = typeof entry.family === "string"
                               ? (entry.family as RuntimeContractFamily)
                               : undefined;
            const status =
                entry.status === "active" || entry.status === "gated" || entry.status === "deferred"
                    ? (entry.status as RuntimeContractStatus)
                    : undefined;
            const title = typeof entry.title === "string" ? entry.title : undefined;
            const summary = typeof entry.summary === "string" ? entry.summary : undefined;
            const marker = typeof entry.marker === "string" ? entry.marker : undefined;
            if (family && status && title && summary && marker)
            {
                normalized.push({
                    kind : "contract",
                    family,
                    status,
                    title,
                    summary,
                    detail : typeof entry.detail === "string" ? entry.detail : undefined,
                    marker,
                    payloadText : typeof entry.payloadText === "string" ? entry.payloadText
                                                                        : undefined,
                });
            }
        }
    }

    return normalized.length > 0 ? normalized : undefined;
}

function extractFallbackOutput(cell: {
    outputs?: Array<
        {output_type?: string; text?: string[] | string; evalue?: string; traceback?: string[]}>;
}): {output?: string; error?: string}
{
    if (!Array.isArray(cell.outputs))
        return {};
    let output: string|undefined;
    let error: string|undefined;

    for (const out of cell.outputs)
    {
        if (out.output_type === "stream")
        {
            const txt = Array.isArray(out.text)        ? out.text.join("\n")
                        : typeof out.text === "string" ? out.text
                                                       : "";
            if (txt.trim())
                output = output ? `${output}\n${txt}` : txt;
        }
        if (out.output_type === "error")
        {
            const tb = Array.isArray(out.traceback) ? out.traceback.join("\n") : "";
            error = tb || (out.evalue ?? "Execution failed.");
        }
    }
    return {output, error};
}

export function fromNotebookJson(raw: string): NotebookCell[]|null
{
    try
    {
        const parsed = JSON.parse(raw) as
        {
            cells?: Array<{
                cell_type?: string;
                source?: string[] | string;
                metadata?: {
                    banana?: {
                        output?: string;
                        error?: string;
                        execCount?: number;
                        previewMode?: boolean;
                        richOutput?: unknown;
                    };
                };
                execution_count?: number | null;
                outputs?: Array<{
                    output_type?: string;
                    text?: string[] | string;
                    evalue?: string;
                    traceback?: string[]
                }>;
            }>;
        };
        if (!parsed.cells || !Array.isArray(parsed.cells))
            return null;

        let id = 1;
        return parsed.cells.map((cell) => {
            const kind: CellKind = cell.cell_type === "code" ? "python" : "markdown";
            const source = Array.isArray(cell.source)        ? cell.source.join("\n")
                           : typeof cell.source === "string" ? cell.source
                                                             : "";
            const bananaMeta = cell.metadata?.banana;
            const fallback = extractFallbackOutput(cell);
            return {
                id : id++,
                kind,
                source,
                output : bananaMeta?.output ?? fallback.output,
                error : bananaMeta?.error ?? fallback.error,
                execCount : bananaMeta?.execCount ??
                                (typeof cell.execution_count === "number" ? cell.execution_count
                                                                          : undefined),
                previewMode : kind === "markdown" ? (bananaMeta?.previewMode ?? true) : undefined,
                richOutput : normalizeRichOutput(bananaMeta?.richOutput),
            };
        });
    }
    catch
    {
        return null;
    }
}

export function normalizeNotebookForBrowser(cells: NotebookCell[]): NotebookCell[]
{
    return cells.map((cell) => {
        const baseCell: NotebookCell = {
            ...cell,
            previewMode : cell.kind === "markdown" ? (cell.previewMode ?? true) : undefined,
        };
        if (cell.kind !== "python")
            return baseCell;

        const src = cell.source;
        const looksLikeLegacyHelper = src.includes("urllib.request") &&
                                      src.includes("def call_endpoint(") &&
                                      src.includes("API_BASE");
        if (!looksLikeLegacyHelper)
            return baseCell;

        const suffix = src.includes("print('setup_ok')") ? "\n\nprint('setup_ok')"
                       : src.includes("print('helpers_ready")
                           ? "\n\nprint('helpers_ready | base:', API_BASE)"
                           : "";
        return {...baseCell, source : `${BROWSER_HTTP_HELPER}${suffix}`};
    });
}

export function withNotebookDefaults(cells: NotebookCell[]): NotebookCell[]
{
    return normalizeNotebookForBrowser(cells).map(
        (cell) => ({
            ...cell,
            previewMode : cell.kind === "markdown" ? (cell.previewMode ?? true) : undefined,
        }));
}

export function summarizeSource(source: string): string
{
    const compact = source.split("\n")
                        .map((line) => line.trim())
                        .filter(Boolean)
                        .join(" ")
                        .replace(/\s+/g, " ");
    return compact.length > 160 ? `${compact.slice(0, 157)}...` : compact;
}

// ---------------------------------------------------------------------------
// Plotly figure normalization
// ---------------------------------------------------------------------------
function downsampleTrace(trace: Record<string, unknown>):
    {trace: Record<string, unknown>; reduced : boolean}
{
    const x = trace.x;
    const y = trace.y;
    if (!Array.isArray(x) || !Array.isArray(y))
        return {trace, reduced : false};
    const n = Math.min(x.length, y.length);
    if (n <= PLOTLY_MAX_TRACE_POINTS)
        return {trace, reduced : false};

    const stride = Math.ceil(n / PLOTLY_MAX_TRACE_POINTS);
    const nextX: unknown[] = [];
    const nextY: unknown[] = [];
    for (let i = 0; i < n; i += stride)
    {
        nextX.push(x[i]);
        nextY.push(y[i]);
    }
    return {trace : {...trace, x : nextX, y : nextY}, reduced : true};
}

export function normalizePlotlyFigure(raw: unknown): {figure?: PlotlyFigure; notices : string[]}
{
    const notices: string[] = [];
    if (!raw || typeof raw !== "object")
        return {notices : [ "Plotly payload ignored: expected object." ]};
    const obj = raw as Record<string, unknown>;
    if (!Array.isArray(obj.data))
        return {notices : [ "Plotly payload ignored: missing data array." ]};

    const normalizedData: unknown[] = [];
    let reducedAny = false;

    for (const traceRaw of obj.data)
    {
        if (!traceRaw || typeof traceRaw !== "object")
            continue;
        const result = downsampleTrace(traceRaw as Record<string, unknown>);
        if (result.reduced)
            reducedAny = true;
        normalizedData.push(result.trace);
    }

    if (normalizedData.length === 0)
        return {notices : [ "Plotly payload ignored: no valid traces." ]};
    if (reducedAny)
        notices.push(`Plotly traces were downsampled to ${
            PLOTLY_MAX_TRACE_POINTS} points for notebook responsiveness.`);

    return {
        figure : {
            data : normalizedData,
            layout : typeof obj.layout === "object" && obj.layout
                         ? (obj.layout as Record<string, unknown>)
                         : undefined,
        },
        notices,
    };
}

// ---------------------------------------------------------------------------
// Contract card builder
// ---------------------------------------------------------------------------
function parseMarkerPayload(raw: string): Record<string, unknown>|undefined
{
    const trimmed = raw.trim();
    if (!trimmed)
        return undefined;
    try
    {
        const parsed = JSON.parse(trimmed) as unknown;
        return parsed && typeof parsed === "object" ? (parsed as Record<string, unknown>)
                                                    : {detail : String(parsed)};
    }
    catch
    {
        return {detail : trimmed};
    }
}

export function buildContractCard(definition: RuntimeContractDefinition,
                                  rawPayload: string): RuntimeContractCard
{
    const payload = parseMarkerPayload(rawPayload);
    const trimmedPayload = rawPayload.trim();
    const title = typeof payload?.title === "string" ? payload.title : definition.title;
    const summary = typeof payload?.summary === "string" ? payload.summary : definition.summary;
    const detail = typeof payload?.detail === "string" ? payload.detail : definition.detail;
    const status = payload?.status === "active" || payload?.status === "gated" ||
                           payload?.status === "deferred"
                       ? payload.status
                       : definition.status;
    return {
        kind : "contract",
        family : definition.family,
        status,
        title,
        summary,
        detail,
        marker : definition.marker,
        payloadText : trimmedPayload ? (() => {
            try
            {
                return JSON.stringify(JSON.parse(trimmedPayload), null, 2);
            }
            catch
            {
                return trimmedPayload;
            }
        })()
                                     : undefined,
    };
}

function buildSurfaceOutput(definition: RuntimeContractDefinition,
                            rawPayload: string): NotebookRichOutput
{
    const payload = parseMarkerPayload(rawPayload) ?? {};
    const title = typeof payload.title === "string" ? payload.title : definition.title;
    const summary = typeof payload.summary === "string" ? payload.summary : definition.summary;
    const {title : _title, summary : _summary, status : _status, detail : _detail, ...rest} =
        payload;

    return {
        kind : "surface",
        family : definition.family as Exclude<RuntimeContractFamily, "plotly"|"table">,
        title,
        summary,
        payload : rest,
    };
}

// ---------------------------------------------------------------------------
// Rich output parser (stdout → structured outputs)
// ---------------------------------------------------------------------------
export function parseRichOutput(stdoutRaw: string):
    {cleanStdout: string; richOutput?: NotebookRichOutput[]}
{
    const lines = stdoutRaw.split("\n");
    const keep: string[] = [];
    const richOutputs: NotebookRichOutput[] = [];

    for (const line of lines)
    {
        if (line.startsWith("BANANA_PLOTLY::"))
        {
            try
            {
                const normalized = normalizePlotlyFigure(
                    JSON.parse(line.slice("BANANA_PLOTLY::".length)) as unknown);
                if (normalized.figure)
                    richOutputs.push({kind : "plotly", figure : normalized.figure});
                for (const n of normalized.notices)
                    richOutputs.push({kind : "notice", level : "info", message : n});
            }
            catch
            {
                richOutputs.push({
                    kind : "notice",
                    level : "warning",
                    message : "Malformed BANANA_PLOTLY payload ignored."
                });
            }
            continue;
        }

        if (line.startsWith("BANANA_VIZ::"))
        {
            try
            {
                const spec = JSON.parse(line.slice("BANANA_VIZ::".length)) as
                {
                    type: "bar"|"line";
                    title?: string;
                    x: Array<string|number>;
                    y: number[];
                };
                const traceType = spec.type === "bar" ? "bar" : "scatter";
                const figure: PlotlyFigure = {
                    data : [ {
                        type : traceType,
                        x : spec.x,
                        y : spec.y,
                        ...(spec.type === "line" ? {mode : "lines+markers"} : {})
                    } ],
                    layout : spec.title ? {title : {text : spec.title}} : {},
                };
                const normalized = normalizePlotlyFigure(figure);
                if (normalized.figure)
                    richOutputs.push({kind : "plotly", figure : normalized.figure});
                for (const n of normalized.notices)
                    richOutputs.push({kind : "notice", level : "info", message : n});
            }
            catch
            {
                richOutputs.push({
                    kind : "notice",
                    level : "warning",
                    message : "Malformed BANANA_VIZ payload ignored."
                });
            }
            continue;
        }

        if (line.startsWith("BANANA_TABLE::"))
        {
            try
            {
                const table = JSON.parse(line.slice("BANANA_TABLE::".length)) as
                {
                    columns: string[];
                    rows: Array<Array<string|number|null>>;
                };
                richOutputs.push({kind : "table", table});
            }
            catch
            {
                richOutputs.push({
                    kind : "notice",
                    level : "warning",
                    message : "Malformed BANANA_TABLE payload ignored."
                });
            }
            continue;
        }

        if (line.startsWith("BANANA_IMAGE::"))
        {
            try
            {
                const payload = JSON.parse(line.slice("BANANA_IMAGE::".length)) as
                {
                    data: string;
                    mimeType?: string;
                    alt?: string;
                };
                const mime = payload.mimeType === "image/svg+xml" ? "image/svg+xml" : "image/png";
                if (typeof payload.data === "string" && payload.data.length > 0)
                {
                    richOutputs.push({
                        kind : "image",
                        data : payload.data,
                        mimeType : mime,
                        alt : typeof payload.alt === "string" ? payload.alt : undefined,
                    });
                }
            }
            catch
            {
                richOutputs.push({
                    kind : "notice",
                    level : "warning",
                    message : "Malformed BANANA_IMAGE payload ignored."
                });
            }
            continue;
        }

        let matched = false;
        for (const contract of RUNTIME_CONTRACTS)
        {
            if (contract.marker === "BANANA_PLOTLY::" || contract.marker === "BANANA_TABLE::")
                continue;
            if (line.startsWith(contract.marker))
            {
                richOutputs.push(
                    LIVE_SURFACE_FAMILIES.has(contract.family)
                        ? buildSurfaceOutput(contract, line.slice(contract.marker.length))
                        : buildContractCard(contract, line.slice(contract.marker.length)),
                );
                matched = true;
                break;
            }
        }
        if (!matched)
            keep.push(line);
    }

    return {
        cleanStdout : keep.join("\n").trim(),
        richOutput : richOutputs.length > 0 ? richOutputs : undefined,
    };
}
