export type CellKind = "markdown"|"python";

export type NotebookCell = {
    id: number; kind : CellKind; source : string;
    output?: string;
    error?: string;
    running?: boolean;
    richOutput?: NotebookRichOutput[];
    execCount?: number;
    previewMode?: boolean;
};

export type PlotlyFigure = {
    data: unknown[];
    layout?: Record<string, unknown>;
};

export type RuntimeContractStatus = "active"|"gated"|"deferred";

export type RuntimeContractFamily =|"plotly"|"table"|"vega"|"altair"|"bokeh"|"widgets"|"ipympl"|
    "bqplot"|"geospatial"|"network"|"three-d"|"canvas";

export type RuntimeContractCard = {
    kind: "contract"; family : RuntimeContractFamily; status : RuntimeContractStatus;
    title : string;
    summary : string;
    detail?: string; marker : string;
    payloadText?: string;
};

export type NotebookSurfaceOutput = {
    kind: "surface"; family : Exclude<RuntimeContractFamily, "plotly"|"table">; title : string;
    summary?: string; payload : Record<string, unknown>;
};

export type NotebookRichOutput =|
{
    kind: "plotly";
    figure: PlotlyFigure
}
|
{
    kind: "table";
    table: {columns: string[]; rows : Array<Array<string|number|null>>}
}
|
{
    kind: "image";
    /** base64-encoded PNG or SVG string (data URI or raw base64) */
    data: string;
    mimeType: "image/png"|"image/svg+xml";
    alt?: string;
}
|
{
    kind: "error";
    ename: string;
    evalue: string;
    traceback: string;
}
|
{
    kind: "notice";
    level: "warning"|"info";
    message: string
}
|NotebookSurfaceOutput|RuntimeContractCard;

export type RuntimeContractDefinition = {
    family: RuntimeContractFamily; status : RuntimeContractStatus; title : string; summary : string;
    detail : string;
    marker : string;
};

export type WorkspaceStat = {
    label: string; value : string; detail : string;
};

export type RuntimeState = "idle"|"loading"|"ready"|"error";
export type EndpointCatalogState = "idle"|"loading"|"ready"|"error";

export type PyodideInstance = {
    globals: {set: (name: string, value: unknown) => void;};
    runPythonAsync : (code: string) => Promise<unknown>;
};

export type NotebookDocument = {
    cells: Array<{
        cell_type : "markdown" | "code"; metadata : {
            language : "markdown" | "python";
            banana?: {
                output?: string;
                error?: string;
                execCount?: number;
                previewMode?: boolean;
                richOutput?: NotebookRichOutput[];
            };
        };
        source : string[];
        outputs?: Array<{
            output_type?: string;
            name?: string;
            text?: string[] | string;
            ename?: string;
            evalue?: string;
            traceback?: string[];
        }>;
        execution_count?: number | null;
    }>;
    metadata : {
        kernelspec : {display_name : string; language : string; name : string;};
        language_info : {name : string;};
    };
    nbformat : number;
    nbformat_minor : number;
};

export type NotebookLibraryEntry = {
    path: string; label : string; description : string;
};

declare global
{
    interface Window
    {
        loadPyodide?: (opts: {indexURL: string}) => Promise<PyodideInstance>;
        Plotly?: {
            newPlot: (el: HTMLElement, data: unknown[], layout?: Record<string, unknown>,
                      config?: Record<string, unknown>) => Promise<void>;
            react : (el: HTMLElement, data: unknown[], layout?: Record<string, unknown>,
                     config?: Record<string, unknown>) => Promise<void>;
            purge : (el: HTMLElement) => void;
            downloadImage :
                (el: HTMLElement,
                 opts: {format: string; filename : string; width : number; height : number}) =>
                    Promise<void>;
        };
    }
}
