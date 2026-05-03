import {NOTEBOOK_LIBRARY, RUNTIME_CONTRACTS} from "../../../lib/dsConstants";
import type {
    NotebookCell, NotebookSurfaceOutput, PlotlyFigure, RuntimeContractDefinition} from
    "../../../lib/dsTypes";

type SuiteFixture = {
    id: string; label : string; description : string; cells : NotebookCell[];
};

function markdownCell(id: number, source: string): NotebookCell
{
    return {id, kind : "markdown", source, previewMode : true};
}

function pythonCell(id: number, source: string, options: Partial<NotebookCell>): NotebookCell
{
    return {
        id,
        kind : "python",
        source,
        execCount : 1,
        ...options,
    };
}

function plotlyOutput(figure: PlotlyFigure): NotebookCell["richOutput"]
{
    return [ {kind : "plotly", figure} ];
}

export const surfaceFixtures: Record<string, NotebookSurfaceOutput> = {
    vega : {
        kind : "surface",
        family : "vega",
        title : "Vega-Lite interactive surface",
        summary :
            "Interactive declarative bar surface hosted directly inside the Banana notebook output.",
        payload : {
            spec : {
                mark : "bar",
                values : [
                    {x : "banana", y : 42},
                    {x : "ripeness", y : 31},
                    {x : "chat", y : 18},
                    {x : "trucks", y : 25},
                ],
            },
        },
    },
    altair : {
        kind : "surface",
        family : "altair",
        title : "Altair declarative surface",
        summary : "Declarative line surface with inline point inspection.",
        payload : {
            spec : {
                mark : "line",
                values : [
                    {x : "run-1", y : 0.79},
                    {x : "run-2", y : 0.83},
                    {x : "run-3", y : 0.87},
                    {x : "run-4", y : 0.9},
                    {x : "run-5", y : 0.92},
                ],
            },
        },
    },
    bokeh : {
        kind : "surface",
        family : "bokeh",
        title : "Bokeh time-window surface",
        summary :
            "Windowed line navigation simulates Bokeh-style timeseries interaction inside the notebook host.",
        payload : {
            series : [
                {x : "09:00", y : 11},
                {x : "09:05", y : 16},
                {x : "09:10", y : 18},
                {x : "09:15", y : 14},
                {x : "09:20", y : 20},
                {x : "09:25", y : 24},
                {x : "09:30", y : 22},
                {x : "09:35", y : 27},
            ],
            windowSize : 5,
        },
    },
    widgets : {
        kind : "surface",
        family : "widgets",
        title : "Widget control surface",
        summary :
            "Dropdown and slider controls drive the selected notebook dataset live in the browser host.",
        payload : {
            datasets : {
                banana : [
                    {x : "train", y : 44},
                    {x : "validate", y : 33},
                    {x : "serve", y : 21},
                ],
                ripeness : [
                    {x : "green", y : 18},
                    {x : "yellow", y : 34},
                    {x : "brown", y : 12},
                ],
                chat : [
                    {x : "ingest", y : 9},
                    {x : "route", y : 15},
                    {x : "respond", y : 19},
                ],
            },
        },
    },
    ipympl : {
        kind : "surface",
        family : "ipympl",
        title : "ipympl cursor surface",
        summary :
            "Interactive timeseries windowing stands in for notebook-native backend cursor inspection.",
        payload : {
            series : [
                {x : "0", y : 2},
                {x : "1", y : 5},
                {x : "2", y : 4},
                {x : "3", y : 7},
                {x : "4", y : 6},
                {x : "5", y : 8},
                {x : "6", y : 9},
            ],
            windowSize : 4,
        },
    },
    bqplot : {
        kind : "surface",
        family : "bqplot",
        title : "bqplot scatter surface",
        summary : "Clickable scatter-style points mirror widget-driven notebook exploration.",
        payload : {
            points : [
                {x : "p1", y : 4},
                {x : "p2", y : 9},
                {x : "p3", y : 6},
                {x : "p4", y : 12},
            ],
        },
    },
    geospatial : {
        kind : "surface",
        family : "geospatial",
        title : "Geospatial dispatch surface",
        summary : "Zoom and click markers to inspect orchard and depot coverage inline.",
        payload : {
            points : [
                {label : "Orchard A", x : 18, y : 22, value : 124},
                {label : "Depot B", x : 46, y : 34, value : 87},
                {label : "Port C", x : 74, y : 18, value : 53},
                {label : "Hub D", x : 61, y : 52, value : 96},
            ],
        },
    },
    network : {
        kind : "surface",
        family : "network",
        title : "Network lineage surface",
        summary : "Click nodes to inspect linked model, dataset, and deployment relationships.",
        payload : {
            nodes : [
                {id : "dataset", label : "dataset"},
                {id : "trainer", label : "trainer"},
                {id : "model", label : "model"},
                {id : "deploy", label : "deploy"},
            ],
            edges : [
                {source : "dataset", target : "trainer"},
                {source : "trainer", target : "model"},
                {source : "model", target : "deploy"},
                {source : "dataset", target : "model"},
            ],
        },
    },
    "three-d" : {
        kind : "surface",
        family : "three-d",
        title : "3D feature cube surface",
        summary : "Rotate a lightweight 3D cube to inspect notebook-side spatial interaction.",
        payload : {
            rotationSpeed : 0.02,
        },
    },
    canvas : {
        kind : "surface",
        family : "canvas",
        title : "Canvas waveform surface",
        summary : "Animate a low-level canvas signal and tune it live from inline controls.",
        payload : {
            amplitude : 0.7,
            frequency : 2.4,
        },
    },
};

export const dsContracts: RuntimeContractDefinition[] = RUNTIME_CONTRACTS;
export const dsLibrary = NOTEBOOK_LIBRARY;

export const suiteFixtures: SuiteFixture[] = [
    {
        id : "platform-setup",
        label : "01 - Platform Setup",
        description : "Shared helpers, endpoint catalog loader, and health smoke.",
        cells : [
            markdownCell(1,
                         "# 01 · Platform Setup\nShared helpers used by every Banana DS notebook."),
            pythonCell(
                2,
                "# Core imports & config\nprint('config_ok | base: https://api.banana.engineer')", {
                    output :
                        "config_ok | base: https://api.banana.engineer\nhelpers_ready\ncatalog_loaded | endpoints: 26\n  Audit: 1\n  Banana: 2\n  Chat: 2\n  Harvest: 2\n  Ripeness: 2\n",
                }),
            pythonCell(3, "# Platform health smoke\npp({'health_status': 200})", {
                richOutput : [ {
                    kind : "table",
                    table : {
                        columns : [ "group", "operation_count" ],
                        rows : [
                            [ "Audit", 1 ], [ "Banana", 2 ], [ "Chat", 2 ], [ "Harvest", 2 ],
                            [ "Ripeness", 2 ]
                        ],
                    },
                } ],
            }),
        ],
    },
    {
        id : "banana-classifier",
        label : "02 - Banana Classifier Workbench",
        description : "Classifier probes, sweeps, and summary checks.",
        cells : [
            markdownCell(
                1,
                "# 02 · Banana Classifier Workbench\nClassifier probes, sweeps, and summary checks."),
            pythonCell(2, "# Banana summary\npp(summary)", {
                richOutput : plotlyOutput({
                    data : [ {
                        type : "bar",
                        x : [ "banana", "not-banana", "borderline" ],
                        y : [ 58, 29, 13 ],
                        marker : {color : [ "#facc15", "#94a3b8", "#fb7185" ]}
                    } ],
                    layout : {title : {text : "Classifier verdict mix"}},
                }),
                output : "status: 200\nsummary_ready\n",
            }),
            pythonCell(3, "# Confidence spread analysis\nprint(rows)", {
                richOutput : [ {
                    kind : "table",
                    table : {
                        columns : [ "multiplier", "banana_score", "verdict" ],
                        rows : [
                            [ 1, 0.71, "banana" ], [ 2, 0.78, "banana" ], [ 3, 0.84, "banana" ],
                            [ 4, 0.67, "borderline" ]
                        ],
                    },
                } ],
            }),
        ],
    },
    {
        id : "ripeness-ml-ops",
        label : "03 - Ripeness and ML Model Ops",
        description : "Ripeness sweeps, training history, and model candidate checks.",
        cells : [
            markdownCell(
                1,
                "# 03 · Ripeness & ML Model Ops\nRipeness sweeps, training history, and candidate checks."),
            pythonCell(2, "# Training history\nprint(history)", {
                richOutput : plotlyOutput({
                    data : [ {
                        type : "scatter",
                        mode : "lines+markers",
                        x : [ "run-1", "run-2", "run-3", "run-4", "run-5" ],
                        y : [ 0.78, 0.82, 0.85, 0.87, 0.9 ],
                        line : {color : "#10b981"}
                    } ],
                    layout : {title : {text : "Model accuracy trend"}, yaxis : {range : [ 0, 1 ]}},
                }),
            }),
            pythonCell(3, "# Candidate model\npp(candidate)", {
                richOutput : [ {
                    kind : "table",
                    table : {
                        columns : [ "candidate", "metric", "value" ],
                        rows : [
                            [ "banana-left-brain-2026-05-03", "accuracy", 0.9 ],
                            [ "banana-left-brain-2026-05-03", "drift", 0.03 ],
                            [ "banana-left-brain-2026-05-03", "smoke_status", "pass" ]
                        ],
                    },
                } ],
                output : "smoke_status: pass\n",
            }),
        ],
    },
    {
        id : "logistics",
        label : "04 - Logistics Trucks and Harvest",
        description : "Fleet route checks and harvest status probes.",
        cells : [
            markdownCell(
                1,
                "# 04 · Logistics — Trucks & Harvest\nFleet route checks and harvest status probes."),
            pythonCell(2, "# List trucks\npp(trucks)", {
                richOutput : [ {
                    kind : "table",
                    table : {
                        columns : [ "truck_id", "route", "status" ],
                        rows : [
                            [ "truck-01", "orchard-a", "ready" ],
                            [ "truck-02", "depot-b", "loading" ],
                            [ "truck-03", "port-c", "in-transit" ]
                        ],
                    },
                } ],
            }),
            pythonCell(3, "# Harvest coverage\nprint(matrix)", {
                richOutput : plotlyOutput({
                    data : [ {
                        type : "bar",
                        x : [ "Orchard A", "Orchard B", "Depot B", "Port C" ],
                        y : [ 124, 98, 87, 53 ],
                        marker : {color : "#3b82f6"}
                    } ],
                    layout : {title : {text : "Dispatch coverage"}},
                }),
            }),
        ],
    },
    {
        id : "chat-audit",
        label : "05 - Chat Sessions and Operator Audit",
        description : "Chat lifecycle, audit, and telemetry snapshots.",
        cells : [
            markdownCell(
                1,
                "# 05 · Chat Sessions & Operator Audit\nChat lifecycle, audit, and telemetry snapshots."),
            pythonCell(2, "# Session lifecycle\nprint(session_count)", {
                richOutput : [ {
                    kind : "table",
                    table : {
                        columns : [ "session_id", "state", "messages" ],
                        rows : [
                            [ "chat-101", "active", 12 ], [ "chat-102", "active", 6 ],
                            [ "chat-099", "closed", 19 ]
                        ],
                    },
                } ],
                output : "streaming_status: healthy\naudit_rows: 18\n",
            }),
        ],
    },
    {
        id : "api-smoke",
        label : "06 - Full API Contract Smoke Suite",
        description : "One-shot API surface smoke checks.",
        cells : [
            markdownCell(
                1,
                "# 06 · Full API Contract Smoke Suite\nOne-shot health check across every endpoint group."),
            pythonCell(2, "# Smoke harness\nprint(results)", {
                output :
                    "[PASS] GET    /health\n[PASS] GET    /banana/summary\n[PASS] POST   /banana\n[PASS] GET    /ripeness/summary\n[PASS] GET    /chat\n[PASS] GET    /swagger/v1/swagger.json\n\n══ Smoke Results: 12/12 PASS ══\n",
                richOutput : [ {
                    kind : "table",
                    table : {
                        columns : [ "group", "path", "status", "verdict" ],
                        rows : [
                            [ "Health", "/health", 200, "PASS" ],
                            [ "Banana", "/banana/summary", 200, "PASS" ],
                            [ "Ripeness", "/ripeness/summary", 200, "PASS" ],
                            [ "Chat", "/chat", 200, "PASS" ]
                        ],
                    },
                } ],
            }),
        ],
    },
    {
        id : "visualization-workbench",
        label : "07 - Visualization Workbench",
        description : "Embedded chart and table scaffolds for notebook-first DS analysis.",
        cells : [
            markdownCell(
                1,
                "# 07 · Visualization Workbench\nNotebook-first interactive surface families for production DS workflows."),
            pythonCell(2, "# Plotly + table outputs\nprint('viz_ready')", {
                output : "viz_workbench_ready\n",
                richOutput : [
                    {
                        kind : "plotly",
                        figure : {
                            data : [ {
                                type : "bar",
                                x : [ "banana", "ripeness", "chat", "trucks" ],
                                y : [ 42, 31, 18, 25 ],
                                marker : {color : [ "#3b82f6", "#10b981", "#f59e0b", "#ef4444" ]}
                            } ],
                            layout : {title : {text : "Requests by endpoint group"}},
                        },
                    },
                    {
                        kind : "table",
                        table : {
                            columns : [ "group", "operation_count" ],
                            rows : [
                                [ "Banana", 2 ], [ "Ripeness", 2 ], [ "Chat", 2 ], [ "Harvest", 2 ]
                            ],
                        },
                    },
                ],
            }),
            pythonCell(3, "# Surface family outputs\nprint('surfaces_ready')", {
                richOutput : [
                    surfaceFixtures.vega, surfaceFixtures.altair, surfaceFixtures.bokeh,
                    surfaceFixtures.widgets, surfaceFixtures.ipympl, surfaceFixtures.geospatial,
                    surfaceFixtures.network, surfaceFixtures["three-d"], surfaceFixtures.canvas
                ],
            }),
        ],
    },
];

export function getSuiteFixture(label: string): SuiteFixture
{
    const suite = suiteFixtures.find((entry) => entry.label === label);
    if (!suite)
    {
        throw new Error(`Unknown DS suite fixture: ${label}`);
    }
    return suite;
}