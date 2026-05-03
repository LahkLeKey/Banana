export type BootstrapCell = {
    kind: "markdown"|"python"; source : string;
};

export type EndpointOperation = {
    method: string; path : string; summary : string; group : string;
};

export type DataScienceSpike = {
    id: string; title : string; focus : string;
};

type OpenApiDocument = {
    paths?: Record<string, Record<string,
                                  {
                                      summary?: string;
                                      tags?: string[];
                                      operationId?: string;
                                  }>>;
};

export const DATA_SCIENCE_FOLLOW_UP_SPIKES: DataScienceSpike[] = [
    {
        id : "DS-201",
        title : "Endpoint Catalog Notebook Bootstrap",
        focus : "Generate endpoint-aware notebook workspace from OpenAPI.",
    },
    {
        id : "DS-202",
        title : "Auth-Aware Request Cells",
        focus : "Add reusable auth header helper cells for secured routes.",
    },
    {
        id : "DS-203",
        title : "Request Replay Harness",
        focus : "Persist and replay endpoint calls from notebook metadata.",
    },
    {
        id : "DS-204",
        title : "Schema Sample Generator",
        focus : "Generate payload skeletons from OpenAPI schemas.",
    },
    {
        id : "DS-205",
        title : "Multi-Endpoint Workflow Cells",
        focus : "Prewire cells for create/read/update endpoint chains.",
    },
    {
        id : "DS-206",
        title : "Latency Benchmark Cell Pack",
        focus : "Measure per-endpoint timings and p95 in notebook output.",
    },
    {
        id : "DS-207",
        title : "Failure Matrix Runner",
        focus : "Probe expected error codes and fallback responses.",
    },
    {
        id : "DS-208",
        title : "Pagination Inspector",
        focus : "Auto-walk cursor/limit APIs and summarize page completeness.",
    },
    {
        id : "DS-209",
        title : "Chat Session Diagnostics",
        focus : "Bootstrapped cells for chat session lifecycle validation.",
    },
    {
        id : "DS-210",
        title : "Training Workbench Notebook Suite",
        focus : "Train/history/promote endpoint orchestration cells.",
    },
    {
        id : "DS-211",
        title : "Ripeness Evaluation Grid",
        focus : "Batch ripeness request experiments with confidence scoring.",
    },
    {
        id : "DS-212",
        title : "Ensemble Drift Snapshot",
        focus : "Track ensemble outputs and compare drift over runs.",
    },
    {
        id : "DS-213",
        title : "Harvest and Truck Route Probes",
        focus : "Domain endpoint smoke cells for logistics flows.",
    },
    {
        id : "DS-214",
        title : "Operator Audit Timeline Export",
        focus : "Notebook transforms for audit endpoint timeline analysis.",
    },
    {
        id : "DS-215",
        title : "Notebook-to-Issue Evidence Export",
        focus : "Emit markdown snippets for bug reports from cell outputs.",
    },
    {
        id : "DS-216",
        title : "Endpoint Contract Diff Cells",
        focus : "Compare local and production OpenAPI endpoint signatures.",
    },
    {
        id : "DS-217",
        title : "Response Shape Assertions",
        focus : "Inline assertions for known response contract fields.",
    },
    {
        id : "DS-218",
        title : "Safe Mutation Dry-Run Layer",
        focus : "Mark mutating calls and require explicit execute flags.",
    },
    {
        id : "DS-219",
        title : "Notebook Session Snapshots",
        focus : "Save and restore endpoint experiment states.",
    },
    {
        id : "DS-220",
        title : "Onboarding Guided Runbook",
        focus : "Step-by-step developer bootstrap flow in first notebook.",
    },
    {
        id : "DS-221",
        title : "Swagger to Notebook Endpoint Injection",
        focus :
            "One-click inject from live Swagger explorer into notebook runner with prewired request helpers.",
    },
    {
        id : "DS-222",
        title : "Split-Panel Virtualized Layout",
        focus :
            "Side-by-side Swagger and Notebook panels with view toggle (Split / Swagger / Notebook).",
    },
    {
        id : "DS-223",
        title : "Swagger Request Replay in Notebook",
        focus :
            "Capture Swagger Try-It-Out execution and replay the exact request in a notebook cell.",
    },
    {
        id : "DS-224",
        title : "OpenAPI Schema Skeleton Generator",
        focus :
            "Generate typed Python payload templates from OpenAPI request body schemas for any endpoint.",
    },
    {
        id : "DS-225",
        title : "Response Diff Viewer",
        focus : "Compare two notebook cell outputs side-by-side to detect response shape drift.",
    },
    {
        id : "DS-226",
        title : "Notebook Runner UX Hardening",
        focus :
            "Cell reorder, output folding, runtime status badge, and keyboard shortcut run-all for the virtualized runner.",
    },
];

export function buildExecutedSpikeMap(): Record<string, "scaffolded"|"executed">
{
    const status: Record<string, "scaffolded"|"executed"> = {};
    for (const spike of DATA_SCIENCE_FOLLOW_UP_SPIKES)
    {
        status[spike.id] = "executed";
    }
    return status;
}

export function extractEndpointOperations(document: unknown): EndpointOperation[]
{
    const openApi = document as OpenApiDocument;
    if (!openApi.paths)
    {
        return [];
    }

    const operations: EndpointOperation[] = [];
    for (const [path, methods] of Object.entries(openApi.paths))
    {
        for (const [method, details] of Object.entries(methods ?? {}))
        {
            const normalizedMethod = method.toUpperCase();
            if (!["GET", "POST", "PUT", "PATCH", "DELETE", "HEAD", "OPTIONS"].includes(
                    normalizedMethod))
            {
                continue;
            }

            operations.push({
                method : normalizedMethod,
                path,
                summary : details.summary ?? details.operationId ?? "No summary available",
                group : details.tags?.[0] ?? "Ungrouped",
            });
        }
    }

    operations.sort((a, b) => {
        if (a.group !== b.group)
        {
            return a.group.localeCompare(b.group);
        }
        if (a.path !== b.path)
        {
            return a.path.localeCompare(b.path);
        }
        return a.method.localeCompare(b.method);
    });

    return operations;
}

export function buildEndpointBootstrapCells(operations: EndpointOperation[],
                                            apiBaseUrl: string): BootstrapCell[]
{
    const safeBase = apiBaseUrl.replace(/\/$/, "");

    const endpointLines =
        operations.map((op) => `- ${op.method} ${op.path} (${op.group}) - ${op.summary}`);
    const endpointCatalogLiteral = JSON.stringify(
        operations.map((op) => ({
                           method : op.method,
                           path : op.path,
                           group : op.group,
                           summary : op.summary,
                       })),
        null,
        2,
    );

    const safeGetCandidates =
        operations
            .filter((op) => op.method === "GET" && !op.path.includes("{") && op.path.length < 64)
            .slice(0, 8)
            .map((op) => ({method : op.method, path : op.path, summary : op.summary}));

    return [
        {
            kind : "markdown",
            source : [
                "# Banana API Bootstrap Workspace",
                "This notebook is scaffolded from the live OpenAPI spec and is designed to bootstrap developers across all endpoints.",
                "",
                "## Endpoint Coverage",
                ...endpointLines,
                "",
                "## How to Use",
                "1. Run the setup cell to load endpoint catalog and helpers.",
                "2. Run endpoint group summary to discover available API areas.",
                "3. Use call_endpoint for any route in endpoint_catalog.",
            ].join("\n"),
        },
        {
            kind : "python",
            source : [
                "import json",
                "from js import XMLHttpRequest",
                "",
                `API_BASE = \"${safeBase}\"`,
                `endpoint_catalog = json.loads('''${endpointCatalogLiteral}''')`,
                "print('endpoint_count:', len(endpoint_catalog))",
            ].join("\n"),
        },
        {
            kind : "python",
            source : [
                "def call_endpoint(method, path, payload=None, headers=None):",
                "    method = method.upper()",
                "    url = f\"{API_BASE}{path}\"",
                "    try:",
                "        xhr = XMLHttpRequest.new()",
                "        xhr.open(method, url, False)",
                "        xhr.setRequestHeader('Accept', 'application/json')",
                "        if headers:",
                "            for k, v in headers.items():",
                "                xhr.setRequestHeader(str(k), str(v))",
                "        body = None",
                "        if payload is not None:",
                "            xhr.setRequestHeader('Content-Type', 'application/json')",
                "            body = json.dumps(payload)",
                "        xhr.send(body)",
                "        return {\"ok\": 200 <= int(xhr.status) < 300, \"status\": int(xhr.status), \"body\": str(xhr.responseText)}",
                "    except Exception as ex:",
                "        return {\"ok\": False, \"status\": None, \"body\": str(ex)}",
                "",
                "print('helper_ready')",
            ].join("\n"),
        },
        {
            kind : "python",
            source : [
                "group_counts = {}",
                "for op in endpoint_catalog:",
                "    group_counts[op['group']] = group_counts.get(op['group'], 0) + 1",
                "",
                "for group in sorted(group_counts):",
                "    print(f\"{group}: {group_counts[group]} operations\")",
            ].join("\n"),
        },
        {
            kind : "python",
            source : [
                "for op in endpoint_catalog:",
                "    print(f\"{op['method']:6} {op['path']} :: {op['summary']}\")",
            ].join("\n"),
        },
        {
            kind : "python",
            source : [
                "# Safe endpoint smoke calls (GET routes without path params)",
                `safe_gets = json.loads('''${JSON.stringify(safeGetCandidates, null, 2)}''')`,
                "for op in safe_gets:",
                "    result = call_endpoint(op['method'], op['path'])",
                "    print(op['method'], op['path'], '->', result['status'])",
            ].join("\n"),
        },
        {
            kind : "markdown",
            source : [
                "## Developer Bootstrapping Checklist",
                "- Verify /health is reachable.",
                "- Inspect endpoint groups and select a domain.",
                "- Execute safe GET probes.",
                "- Run a targeted POST using explicit payload and headers.",
                "- Export notebook and attach to onboarding evidence.",
            ].join("\n"),
        },
    ];
}
