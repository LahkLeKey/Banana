import type {
    NotebookCell, NotebookLibraryEntry, RuntimeContractDefinition, RuntimeContractStatus} from
    "./dsTypes";

export const STORAGE_KEY = "banana.ds.notebook.v1";
export const PYODIDE_SCRIPT = "https://cdn.jsdelivr.net/pyodide/v0.27.5/full/pyodide.js";
export const PYODIDE_INDEX = "https://cdn.jsdelivr.net/pyodide/v0.27.5/full/";
export const PLOTLY_SCRIPT = "https://cdn.plot.ly/plotly-2.35.2.min.js";
export const PLOTLY_MAX_TRACE_POINTS = 1200;

export const BROWSER_HTTP_HELPER = [
    "import json",
    "from js import XMLHttpRequest",
    "",
    "API_BASE = 'https://api.banana.engineer'",
    "",
    "def pp(obj):",
    "    if isinstance(obj, str):",
    "        try:",
    "            obj = json.loads(obj)",
    "        except Exception:",
    "            print(obj)",
    "            return",
    "    print(json.dumps(obj, indent=2))",
    "",
    "def call_endpoint(method, path, payload=None, headers=None):",
    "    url = path if str(path).startswith('http') else f\"{API_BASE}{path}\"",
    "    try:",
    "        xhr = XMLHttpRequest.new()",
    "        xhr.open(method.upper(), url, False)",
    "        xhr.setRequestHeader('Accept', 'application/json')",
    "        if headers:",
    "            for k, v in headers.items():",
    "                xhr.setRequestHeader(str(k), str(v))",
    "        body = None",
    "        if payload is not None:",
    "            xhr.setRequestHeader('Content-Type', 'application/json')",
    "            body = json.dumps(payload)",
    "        xhr.send(body)",
    "        return {'ok': 200 <= int(xhr.status) < 300, 'status': int(xhr.status), 'body': str(xhr.responseText)}",
    "    except Exception as ex:",
    "        return {'ok': False, 'status': None, 'body': str(ex)}",
    "",
    "def get(path, **kw):",
    "    return call_endpoint('GET', path, **kw)",
    "",
    "def post(path, **kw):",
    "    return call_endpoint('POST', path, **kw)",
].join("\n");

export const NOTEBOOK_LIBRARY: NotebookLibraryEntry[] = [
    {
        path : "/notebooks/01-platform-setup.ipynb",
        label : "01 - Platform Setup",
        description : "Shared helpers, endpoint catalog loader, and health smoke.",
    },
    {
        path : "/notebooks/02-banana-classifier-workbench.ipynb",
        label : "02 - Banana Classifier Workbench",
        description : "Classifier probes, sweeps, and summary checks.",
    },
    {
        path : "/notebooks/03-ripeness-ml-model-ops.ipynb",
        label : "03 - Ripeness and ML Model Ops",
        description : "Ripeness sweeps, training history, and model candidate checks.",
    },
    {
        path : "/notebooks/04-logistics-trucks-harvest.ipynb",
        label : "04 - Logistics Trucks and Harvest",
        description : "Fleet route checks and harvest status probes.",
    },
    {
        path : "/notebooks/05-chat-sessions-operator-audit.ipynb",
        label : "05 - Chat Sessions and Operator Audit",
        description : "Chat lifecycle, audit, and telemetry snapshots.",
    },
    {
        path : "/notebooks/06-full-api-smoke-suite.ipynb",
        label : "06 - Full API Contract Smoke Suite",
        description : "One-shot API surface smoke checks.",
    },
    {
        path : "/notebooks/07-visualization-workbench.ipynb",
        label : "07 - Visualization Workbench",
        description : "Embedded chart and table scaffolds for notebook-first DS analysis.",
    },
];

export const starterCells: NotebookCell[] = [
    {
        id : 1,
        kind : "markdown",
        source :
            "# Banana DS Notebook\nJupyter-first workflow. Load a notebook or write cells below.",
    },
    {
        id : 2,
        kind : "python",
        source : `${BROWSER_HTTP_HELPER}\n\nprint('helpers_ready | base:', API_BASE)`,
    },
    {
        id : 3,
        kind : "python",
        source :
            "# Safe health check (works even if helper cell was not run)\nimport json\nif 'get' not in globals():\n    " +
                BROWSER_HTTP_HELPER.replace(/\n/g, "\n    ") +
                "\n\nr = get('/health')\nprint('health:', r['status'])\nr2 = get('/swagger/v1/swagger.json')\nif r2['ok']:\n    spec = json.loads(r2['body'])\n    count = sum(len(v) for v in (spec.get('paths') or {}).values())\n    print('endpoint_count:', count)\nelse:\n    print('catalog_error:', r2['status'])",
    },
];

export const VIZ_BAR_SCAFFOLD = [
    "import json",
    "figure = {",
    "    'data': [{'type': 'bar', 'x': ['banana', 'ripeness', 'chat', 'trucks'], 'y': [42, 31, 18, 25], 'marker': {'color': ['#3b82f6','#10b981','#f59e0b','#ef4444']}}],",
    "    'layout': {'title': {'text': 'Requests by Endpoint Group'}, 'xaxis': {'title': 'group'}, 'yaxis': {'title': 'count'}, 'paper_bgcolor': 'rgba(0,0,0,0)', 'plot_bgcolor': 'rgba(0,0,0,0)'}",
    "}",
    "print('BANANA_PLOTLY::' + json.dumps(figure))",
].join("\n");

export const VIZ_LINE_SCAFFOLD = [
    "import json",
    "figure = {",
    "    'data': [{'type': 'scatter', 'mode': 'lines+markers', 'x': ['run-1','run-2','run-3','run-4','run-5'], 'y': [0.79, 0.84, 0.86, 0.88, 0.90], 'name': 'accuracy', 'line': {'color': '#3b82f6'}}],",
    "    'layout': {'title': {'text': 'Accuracy Trend'}, 'xaxis': {'title': 'run'}, 'yaxis': {'title': 'accuracy', 'range': [0, 1]}, 'paper_bgcolor': 'rgba(0,0,0,0)', 'plot_bgcolor': 'rgba(0,0,0,0)'}",
    "}",
    "print('BANANA_PLOTLY::' + json.dumps(figure))",
].join("\n");

export const TABLE_SCAFFOLD = [
    "import json",
    "table = {",
    "    'columns': ['lane', 'samples', 'accuracy'],",
    "    'rows': [",
    "        ['banana', 512, 0.94],",
    "        ['not-banana', 498, 0.91],",
    "        ['ripeness', 321, 0.88]",
    "    ]",
    "}",
    "print('BANANA_TABLE::' + json.dumps(table))",
].join("\n");

export const VEGA_GATE_SCAFFOLD = [
    "import json",
    "payload = {",
    "    'title': 'Vega-Lite interactive surface',",
    "    'summary': 'Interactive declarative bar surface hosted directly inside the Banana notebook output.',",
    "    'spec': {",
    "        'mark': 'bar',",
    "        'values': [",
    "            {'x': 'banana', 'y': 42},",
    "            {'x': 'ripeness', 'y': 31},",
    "            {'x': 'chat', 'y': 18},",
    "            {'x': 'trucks', 'y': 25}",
    "        ]",
    "    }",
    "}",
    "print('BANANA_VEGA::' + json.dumps(payload))",
].join("\n");

export const BOKEH_GATE_SCAFFOLD = [
    "import json",
    "payload = {",
    "    'title': 'Bokeh time-window surface',",
    "    'summary': 'Windowed line navigation simulates Bokeh-style timeseries interaction inside the notebook host.',",
    "    'series': [",
    "        {'x': '09:00', 'y': 11},",
    "        {'x': '09:05', 'y': 16},",
    "        {'x': '09:10', 'y': 18},",
    "        {'x': '09:15', 'y': 14},",
    "        {'x': '09:20', 'y': 20},",
    "        {'x': '09:25', 'y': 24},",
    "        {'x': '09:30', 'y': 22},",
    "        {'x': '09:35', 'y': 27}",
    "    ],",
    "    'windowSize': 5",
    "}",
    "print('BANANA_BOKEH::' + json.dumps(payload))",
].join("\n");

export const ALTAIR_GATE_SCAFFOLD = [
    "import json",
    "payload = {",
    "    'title': 'Altair declarative surface',",
    "    'summary': 'Declarative line surface with inline point inspection.',",
    "    'spec': {",
    "        'mark': 'line',",
    "        'values': [",
    "            {'x': 'run-1', 'y': 0.79},",
    "            {'x': 'run-2', 'y': 0.83},",
    "            {'x': 'run-3', 'y': 0.87},",
    "            {'x': 'run-4', 'y': 0.9},",
    "            {'x': 'run-5', 'y': 0.92}",
    "        ]",
    "    }",
    "}",
    "print('BANANA_ALTAIR::' + json.dumps(payload))",
].join("\n");

export const WIDGET_GATE_SCAFFOLD = [
    "import json",
    "payload = {",
    "    'title': 'Widget control surface',",
    "    'summary': 'Dropdown and slider controls drive the selected notebook dataset live in the browser host.',",
    "    'datasets': {",
    "        'banana': [",
    "            {'x': 'train', 'y': 44},",
    "            {'x': 'validate', 'y': 33},",
    "            {'x': 'serve', 'y': 21}",
    "        ],",
    "        'ripeness': [",
    "            {'x': 'green', 'y': 18},",
    "            {'x': 'yellow', 'y': 34},",
    "            {'x': 'brown', 'y': 12}",
    "        ],",
    "        'chat': [",
    "            {'x': 'ingest', 'y': 9},",
    "            {'x': 'route', 'y': 15},",
    "            {'x': 'respond', 'y': 19}",
    "        ]",
    "    }",
    "}",
    "print('BANANA_WIDGET::' + json.dumps(payload))",
    "print('BANANA_BQPLOT::' + json.dumps({'title': 'bqplot scatter surface', 'summary': 'Clickable scatter-style points mirror widget-driven notebook exploration.', 'points': [{'x': 'p1', 'y': 4}, {'x': 'p2', 'y': 9}, {'x': 'p3', 'y': 6}, {'x': 'p4', 'y': 12}]}))",
].join("\n");

export const IPYMPL_GATE_SCAFFOLD = [
    "import json",
    "payload = {",
    "    'title': 'ipympl cursor surface',",
    "    'summary': 'Interactive timeseries windowing stands in for notebook-native backend cursor inspection.',",
    "    'series': [",
    "        {'x': '0', 'y': 2},",
    "        {'x': '1', 'y': 5},",
    "        {'x': '2', 'y': 4},",
    "        {'x': '3', 'y': 7},",
    "        {'x': '4', 'y': 6},",
    "        {'x': '5', 'y': 8},",
    "        {'x': '6', 'y': 9}",
    "    ],",
    "    'windowSize': 4",
    "}",
    "print('BANANA_IPYMPL::' + json.dumps(payload))",
].join("\n");

export const GEOSPATIAL_SURFACE_SCAFFOLD = [
    "import json",
    "payload = {",
    "    'title': 'Geospatial dispatch surface',",
    "    'summary': 'Zoom and click markers to inspect orchard and depot coverage inline.',",
    "    'points': [",
    "        {'label': 'Orchard A', 'x': 18, 'y': 22, 'value': 124},",
    "        {'label': 'Depot B', 'x': 46, 'y': 34, 'value': 87},",
    "        {'label': 'Port C', 'x': 74, 'y': 18, 'value': 53},",
    "        {'label': 'Hub D', 'x': 61, 'y': 52, 'value': 96}",
    "    ]",
    "}",
    "print('BANANA_GEO::' + json.dumps(payload))",
].join("\n");

export const NETWORK_SURFACE_SCAFFOLD = [
    "import json",
    "payload = {",
    "    'title': 'Network lineage surface',",
    "    'summary': 'Click nodes to inspect linked model, dataset, and deployment relationships.',",
    "    'nodes': [",
    "        {'id': 'dataset', 'label': 'dataset'},",
    "        {'id': 'trainer', 'label': 'trainer'},",
    "        {'id': 'model', 'label': 'model'},",
    "        {'id': 'deploy', 'label': 'deploy'}",
    "    ],",
    "    'edges': [",
    "        {'source': 'dataset', 'target': 'trainer'},",
    "        {'source': 'trainer', 'target': 'model'},",
    "        {'source': 'model', 'target': 'deploy'},",
    "        {'source': 'dataset', 'target': 'model'}",
    "    ]",
    "}",
    "print('BANANA_NETWORK::' + json.dumps(payload))",
].join("\n");

export const THREE_D_SURFACE_SCAFFOLD = [
    "import json",
    "payload = {",
    "    'title': '3D feature cube surface',",
    "    'summary': 'Rotate a lightweight 3D cube to inspect notebook-side spatial interaction.',",
    "    'rotationSpeed': 0.02",
    "}",
    "print('BANANA_3D::' + json.dumps(payload))",
].join("\n");

export const CANVAS_SURFACE_SCAFFOLD = [
    "import json",
    "payload = {",
    "    'title': 'Canvas waveform surface',",
    "    'summary': 'Animate a low-level canvas signal and tune it live from inline controls.',",
    "    'amplitude': 0.7,",
    "    'frequency': 2.4",
    "}",
    "print('BANANA_CANVAS::' + json.dumps(payload))",
].join("\n");

export const SCAFFOLDS: {label: string; source : string}[] = [
    {label : "Bar Chart", source : VIZ_BAR_SCAFFOLD},
    {label : "Line Chart", source : VIZ_LINE_SCAFFOLD},
    {label : "Table", source : TABLE_SCAFFOLD},
    {label : "Vega Surface", source : VEGA_GATE_SCAFFOLD},
    {label : "Altair Surface", source : ALTAIR_GATE_SCAFFOLD},
    {label : "Bokeh Surface", source : BOKEH_GATE_SCAFFOLD},
    {label : "Widget Surface", source : WIDGET_GATE_SCAFFOLD},
    {label : "ipympl Surface", source : IPYMPL_GATE_SCAFFOLD},
    {label : "Geo Surface", source : GEOSPATIAL_SURFACE_SCAFFOLD},
    {label : "Network Surface", source : NETWORK_SURFACE_SCAFFOLD},
    {label : "3D Surface", source : THREE_D_SURFACE_SCAFFOLD},
    {label : "Canvas Surface", source : CANVAS_SURFACE_SCAFFOLD},
];

export const RUNTIME_CONTRACTS: RuntimeContractDefinition[] = [
    {
        family : "plotly",
        status : "active",
        title : "Plotly charts",
        summary : "Primary interactive chart path for the replacement Data Science page.",
        detail :
            "Full figure JSON is rendered inline and persisted with notebook metadata plus standard output fallbacks.",
        marker : "BANANA_PLOTLY::",
    },
    {
        family : "table",
        status : "active",
        title : "Tabular output",
        summary : "Structured table payloads stay available alongside chart artifacts.",
        detail : "Use for operator-readable summaries, audit tables, and chart companion data.",
        marker : "BANANA_TABLE::",
    },
    {
        family : "vega",
        status : "active",
        title : "Vega / Vega-Lite",
        summary : "Declarative specs now render as interactive inline notebook surfaces.",
        detail :
            "Banana hosts lightweight declarative chart rendering directly in notebook output without leaving the DS page.",
        marker : "BANANA_VEGA::",
    },
    {
        family : "altair",
        status : "active",
        title : "Altair",
        summary : "Altair-style declarative specs render as interactive inline notebook charts.",
        detail :
            "Altair payloads are hosted through the same shared browser surface path as Vega-family outputs.",
        marker : "BANANA_ALTAIR::",
    },
    {
        family : "bokeh",
        status : "active",
        title : "Bokeh",
        summary :
            "Bokeh-style timeseries interaction is available through an inline windowed host surface.",
        detail :
            "Notebook outputs can expose live navigation behavior without leaving the default Data Science page runtime.",
        marker : "BANANA_BOKEH::",
    },
    {
        family : "widgets",
        status : "active",
        title : "ipywidgets",
        summary : "Inline controls now drive notebook-side dataset selection and parameter tuning.",
        detail : "The DS page hosts shared widget-style interactions directly in output artifacts.",
        marker : "BANANA_WIDGET::",
    },
    {
        family : "ipympl",
        status : "active",
        title : "ipympl",
        summary :
            "Interactive timeseries cursor/windowing is available as a live notebook surface.",
        detail :
            "The host exposes lightweight backend-like exploration without requiring a Python-side widget channel.",
        marker : "BANANA_IPYMPL::",
    },
    {
        family : "bqplot",
        status : "active",
        title : "bqplot",
        summary :
            "Interactive scatter-style notebook views render inline alongside widget-driven flows.",
        detail :
            "bqplot-style interactions reuse the same shared notebook surface host as the widget family.",
        marker : "BANANA_BQPLOT::",
    },
    {
        family : "geospatial",
        status : "active",
        title : "Geospatial surfaces",
        summary :
            "Zoomable map-like notebook marker surfaces are available directly in the DS page.",
        detail :
            "Specialized geospatial payloads now render through a lightweight inline host with marker inspection and zoom.",
        marker : "BANANA_GEO::",
    },
    {
        family : "network",
        status : "active",
        title : "Network graphs",
        summary : "Relationship and graph notebook views render as interactive node-link surfaces.",
        detail :
            "Users can inspect linked entities inline without leaving the default notebook experience.",
        marker : "BANANA_NETWORK::",
    },
    {
        family : "three-d",
        status : "active",
        title : "3D / WebGL",
        summary :
            "Lightweight 3D notebook interaction is available as an inline rotating canvas surface.",
        detail :
            "A shared host exposes controllable 3D previews while keeping the browser runtime bounded and lightweight.",
        marker : "BANANA_3D::",
    },
    {
        family : "canvas",
        status : "active",
        title : "Canvas custom surfaces",
        summary :
            "Low-level custom canvas surfaces now render as live animated notebook artifacts.",
        detail :
            "The notebook host can interpret bounded custom canvas payloads without falling back to opaque placeholders.",
        marker : "BANANA_CANVAS::",
    },
];

// Contract color maps — DARK variant for use inside dark output terminals
export const CONTRACT_TONE_DARK: Record<RuntimeContractStatus, string> = {
    active : "border-emerald-500/20 bg-emerald-500/10 text-emerald-100",
    gated : "border-amber-400/20 bg-amber-400/10 text-amber-100",
    deferred : "border-slate-500/20 bg-slate-500/10 text-slate-200",
};

// Contract color maps — LIGHT variant for use on white/slate backgrounds
export const CONTRACT_TONE_LIGHT: Record<RuntimeContractStatus, string> = {
    active : "border-emerald-300 bg-emerald-50 text-emerald-900",
    gated : "border-amber-300 bg-amber-50 text-amber-900",
    deferred : "border-slate-200 bg-slate-50 text-slate-700",
};

export const CONTRACT_PILL_TONE: Record<RuntimeContractStatus, string> = {
    active : "border-emerald-500/40 bg-emerald-100 text-emerald-700",
    gated : "border-amber-500/40 bg-amber-100 text-amber-700",
    deferred : "border-slate-300 bg-slate-100 text-slate-600",
};
