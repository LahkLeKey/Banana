// Tests for dsNotebook.ts library functions.
// Covers: parseRichOutput, toNotebookJson, fromNotebookJson, renderMarkdown,
//         summarizeSource, normalizeRichOutput, normalizePlotlyFigure.

import {describe, expect, test} from "bun:test";

import {
    fromNotebookJson,
    normalizePlotlyFigure,
    normalizeRichOutput,
    parseRichOutput,
    renderMarkdown,
    summarizeSource,
    toNotebookJson,
} from "./dsNotebook";
import type {NotebookCell} from "./dsTypes";

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

function pyCell(source: string, overrides: Partial<NotebookCell> = {}): NotebookCell
{
    return {id : 1, kind : "python", source, ...overrides};
}

function mdCell(source: string, overrides: Partial<NotebookCell> = {}): NotebookCell
{
    return {id : 2, kind : "markdown", source, previewMode : true, ...overrides};
}

// ---------------------------------------------------------------------------
// parseRichOutput
// ---------------------------------------------------------------------------

describe("parseRichOutput", () => {
    test("empty string returns empty cleanStdout and no richOutput", () => {
        const {cleanStdout, richOutput} = parseRichOutput("");
        expect(cleanStdout).toBe("");
        expect(richOutput).toBeUndefined();
    });

    test("plain stdout is preserved unchanged", () => {
        const {cleanStdout, richOutput} = parseRichOutput("hello\nworld");
        expect(cleanStdout).toBe("hello\nworld");
        expect(richOutput).toBeUndefined();
    });

    test("BANANA_PLOTLY:: line produces plotly richOutput", () => {
        const figure = {data : [ {type : "bar", x : [ "a" ], y : [ 1 ]} ], layout : {}};
        const {richOutput} = parseRichOutput(`BANANA_PLOTLY::${JSON.stringify(figure)}`);
        expect(richOutput).toBeDefined();
        expect(richOutput![0].kind).toBe("plotly");
    });

    test("BANANA_PLOTLY:: line is removed from cleanStdout", () => {
        const figure = {data : [ {type : "bar", x : [ "a" ], y : [ 1 ]} ]};
        const {cleanStdout} =
            parseRichOutput(`before\nBANANA_PLOTLY::${JSON.stringify(figure)}\nafter`);
        expect(cleanStdout).not.toContain("BANANA_PLOTLY");
        expect(cleanStdout).toContain("before");
        expect(cleanStdout).toContain("after");
    });

    test("malformed BANANA_PLOTLY:: produces a warning notice", () => {
        const {richOutput} = parseRichOutput("BANANA_PLOTLY::not-valid-json{{");
        expect(richOutput).toBeDefined();
        expect(richOutput![0].kind).toBe("notice");
        const notice = richOutput![0] as
        {
            kind: "notice";
            level: string
        };
        expect(notice.level).toBe("warning");
    });

    test("BANANA_TABLE:: line produces table richOutput", () => {
        const table = {columns : [ "a", "b" ], rows : [ [ "x", 1 ] ]};
        const {richOutput} = parseRichOutput(`BANANA_TABLE::${JSON.stringify(table)}`);
        expect(richOutput).toBeDefined();
        expect(richOutput![0].kind).toBe("table");
    });

    test("malformed BANANA_TABLE:: produces a warning notice", () => {
        const {richOutput} = parseRichOutput("BANANA_TABLE::{{bad");
        expect(richOutput).toBeDefined();
        expect(richOutput![0].kind).toBe("notice");
    });

    test("BANANA_VIZ:: bar line is converted to plotly richOutput", () => {
        const viz = {type : "bar", title : "Test", x : [ "a", "b" ], y : [ 1, 2 ]};
        const {richOutput} = parseRichOutput(`BANANA_VIZ::${JSON.stringify(viz)}`);
        expect(richOutput).toBeDefined();
        expect(richOutput![0].kind).toBe("plotly");
    });

    test("BANANA_VIZ:: line line uses scatter type", () => {
        const viz = {type : "line", x : [ "r1", "r2" ], y : [ 0.8, 0.9 ]};
        const {richOutput} = parseRichOutput(`BANANA_VIZ::${JSON.stringify(viz)}`);
        expect(richOutput).toBeDefined();
        const plotly = richOutput![0] as
        {
            kind: "plotly";
            figure: {data: Array<{type?: string}>}
        };
        expect(plotly.figure.data[0].type).toBe("scatter");
    });

    test("BANANA_VEGA:: produces a contract card richOutput", () => {
        const {richOutput} =
            parseRichOutput(`BANANA_VEGA::${JSON.stringify({title : "t", summary : "s"})}`);
        expect(richOutput).toBeDefined();
        const card = richOutput![0] as
        {
            kind: string;
            family?: string
        };
        expect(card.kind).toBe("contract");
        expect(card.family).toBe("vega");
    });

    test("BANANA_GEO:: produces a contract card with deferred status", () => {
        const {richOutput} =
            parseRichOutput(`BANANA_GEO::${JSON.stringify({title : "t", summary : "s"})}`);
        expect(richOutput).toBeDefined();
        const card = richOutput![0] as
        {
            kind: string;
            status?: string;
            family?: string
        };
        expect(card.kind).toBe("contract");
        expect(card.family).toBe("geospatial");
        expect(card.status).toBe("deferred");
    });

    test("mixed stdout and marker separates clean lines from rich output", () => {
        const table = {columns : [ "x" ], rows : [ [ 1 ] ]};
        const stdout = `line1\nBANANA_TABLE::${JSON.stringify(table)}\nline2`;
        const {cleanStdout, richOutput} = parseRichOutput(stdout);
        expect(cleanStdout).toContain("line1");
        expect(cleanStdout).toContain("line2");
        expect(richOutput).toHaveLength(1);
        expect(richOutput![0].kind).toBe("table");
    });

    test("multiple markers produce multiple richOutput entries", () => {
        const figure = {data : [ {type : "bar", x : [ "a" ], y : [ 1 ]} ]};
        const table = {columns : [ "c" ], rows : [ [ "v" ] ]};
        const stdout = [
            `BANANA_PLOTLY::${JSON.stringify(figure)}`,
            `BANANA_TABLE::${JSON.stringify(table)}`,
        ].join("\n");
        const {richOutput} = parseRichOutput(stdout);
        expect(richOutput).toHaveLength(2);
        expect(richOutput![0].kind).toBe("plotly");
        expect(richOutput![1].kind).toBe("table");
    });
});

// ---------------------------------------------------------------------------
// toNotebookJson / fromNotebookJson roundtrip
// ---------------------------------------------------------------------------

describe("toNotebookJson / fromNotebookJson", () => {
    test("roundtrip preserves python cell source", () => {
        const cells = [ pyCell("x = 1\nprint(x)") ];
        const json = toNotebookJson(cells);
        const restored = fromNotebookJson(JSON.stringify(json));
        expect(restored).not.toBeNull();
        expect(restored![0].source).toBe("x = 1\nprint(x)");
    });

    test("roundtrip preserves markdown cell kind", () => {
        const cells = [ mdCell("# Hello") ];
        const json = toNotebookJson(cells);
        const restored = fromNotebookJson(JSON.stringify(json));
        expect(restored![0].kind).toBe("markdown");
    });

    test("roundtrip preserves python cell output", () => {
        const cells = [ pyCell("print('hi')", {output : "hi"}) ];
        const json = toNotebookJson(cells);
        const restored = fromNotebookJson(JSON.stringify(json));
        expect(restored![0].output).toBe("hi");
    });

    test("roundtrip preserves python cell error", () => {
        const cells = [ pyCell("raise", {error : "NameError: name"}) ];
        const json = toNotebookJson(cells);
        const restored = fromNotebookJson(JSON.stringify(json));
        expect(restored![0].error).toBe("NameError: name");
    });

    test("roundtrip preserves execCount", () => {
        const cells = [ pyCell("1+1", {execCount : 3}) ];
        const json = toNotebookJson(cells);
        const restored = fromNotebookJson(JSON.stringify(json));
        expect(restored![0].execCount).toBe(3);
    });

    test("roundtrip sets nbformat to 4", () => {
        const json = toNotebookJson([ pyCell("pass") ]);
        expect(json.nbformat).toBe(4);
    });

    test("roundtrip sets kernelspec language to python", () => {
        const json = toNotebookJson([ pyCell("pass") ]);
        expect(json.metadata.kernelspec.language).toBe("python");
    });

    test("fromNotebookJson returns null for invalid JSON",
         () => { expect(fromNotebookJson("not json")).toBeNull(); });

    test("fromNotebookJson returns null when cells is missing",
         () => { expect(fromNotebookJson('{"nbformat": 4}')).toBeNull(); });

    test("fromNotebookJson restores fallback output from standard outputs array", () => {
        const raw = JSON.stringify({
            nbformat : 4,
            nbformat_minor : 5,
            metadata : {kernelspec : {language : "python"}},
            cells : [ {
                cell_type : "code",
                source : [ "print('x')" ],
                outputs : [ {output_type : "stream", name : "stdout", text : "x\n"} ],
            } ],
        });
        const restored = fromNotebookJson(raw);
        expect(restored).not.toBeNull();
        expect(restored![0].output).toContain("x");
    });
});

// ---------------------------------------------------------------------------
// renderMarkdown
// ---------------------------------------------------------------------------

describe("renderMarkdown", () => {
    test("# heading renders as h1", () => {
        expect(renderMarkdown("# Hello")).toContain("<h1");
        expect(renderMarkdown("# Hello")).toContain("Hello");
    });

    test("## heading renders as h2",
         () => { expect(renderMarkdown("## Section")).toContain("<h2"); });

    test("### heading renders as h3",
         () => { expect(renderMarkdown("### Sub")).toContain("<h3"); });

    test("- item renders as list-disc li", () => {
        expect(renderMarkdown("- item")).toContain("<li");
        expect(renderMarkdown("- item")).toContain("list-disc");
    });

    test("* item also renders as list-disc li",
         () => { expect(renderMarkdown("* item")).toContain("<li"); });

    test("1. item renders as list-decimal li",
         () => { expect(renderMarkdown("1. thing")).toContain("list-decimal"); });

    test("empty line renders as br", () => { expect(renderMarkdown("")).toContain("<br/>"); });

    test(
        "**bold** renders as strong",
        () => { expect(renderMarkdown("**bold text**")).toContain("<strong>bold text</strong>"); });

    test("*italic* renders as em",
         () => { expect(renderMarkdown("*italic*")).toContain("<em>italic</em>"); });

    test("`code` renders as code element", () => {
        expect(renderMarkdown("`snippet`")).toContain("<code");
        expect(renderMarkdown("`snippet`")).toContain("snippet");
    });

    test("< is HTML-escaped",
         () => { expect(renderMarkdown("<script>")).toContain("&lt;script&gt;"); });

    test("& is HTML-escaped", () => { expect(renderMarkdown("a & b")).toContain("a &amp; b"); });
});

// ---------------------------------------------------------------------------
// summarizeSource
// ---------------------------------------------------------------------------

describe("summarizeSource", () => {
    test("short source is returned unchanged (collapsed)", () => {
        const result = summarizeSource("x = 1");
        expect(result).toBe("x = 1");
    });

    test("multi-line source is collapsed into one line", () => {
        const result = summarizeSource("a = 1\nb = 2\nc = 3");
        expect(result).not.toContain("\n");
        expect(result).toContain("a = 1");
    });

    test("blank lines are filtered out", () => {
        const result = summarizeSource("a = 1\n\nb = 2");
        expect(result).toBe("a = 1 b = 2");
    });

    test("source longer than 160 chars is truncated with ...", () => {
        const long = "x = " +
                     "a".repeat(200);
        const result = summarizeSource(long);
        expect(result.endsWith("...")).toBe(true);
        expect(result.length).toBeLessThanOrEqual(160);
    });

    test("source exactly 160 chars is not truncated", () => {
        const exactly = "a".repeat(160);
        const result = summarizeSource(exactly);
        expect(result).toBe(exactly);
        expect(result.endsWith("...")).toBe(false);
    });
});

// ---------------------------------------------------------------------------
// normalizeRichOutput
// ---------------------------------------------------------------------------

describe("normalizeRichOutput", () => {
    test("non-array input returns undefined", () => {
        expect(normalizeRichOutput(null)).toBeUndefined();
        expect(normalizeRichOutput({})).toBeUndefined();
        expect(normalizeRichOutput("string")).toBeUndefined();
    });

    test("empty array returns undefined",
         () => { expect(normalizeRichOutput([])).toBeUndefined(); });

    test("plotly entry without figure.data is skipped", () => {
        const result = normalizeRichOutput([ {kind : "plotly", figure : {layout : {}}} ]);
        expect(result).toBeUndefined();
    });

    test("valid plotly entry is preserved", () => {
        const entry = {kind : "plotly", figure : {data : [ {type : "bar"} ], layout : {}}};
        const result = normalizeRichOutput([ entry ]);
        expect(result).toBeDefined();
        expect(result![0].kind).toBe("plotly");
    });

    test("valid table entry is preserved", () => {
        const entry = {kind : "table", table : {columns : [ "a" ], rows : [ [ "1" ] ]}};
        const result = normalizeRichOutput([ entry ]);
        expect(result).toBeDefined();
        expect(result![0].kind).toBe("table");
    });

    test("table columns are coerced to strings", () => {
        const entry = {kind : "table", table : {columns : [ 1, 2 ], rows : [ [ 3, 4 ] ]}};
        const result = normalizeRichOutput([ entry ]);
        const table = result![0] as
        {
            kind: "table";
            table: {columns: string[]}
        };
        expect(table.table.columns).toEqual([ "1", "2" ]);
    });

    test("notice entry is preserved", () => {
        const entry = {kind : "notice", level : "info", message : "hello"};
        const result = normalizeRichOutput([ entry ]);
        expect(result).toBeDefined();
        expect(result![0].kind).toBe("notice");
    });

    test("notice with unknown level defaults to info", () => {
        const entry = {kind : "notice", level : "unknown", message : "msg"};
        const result = normalizeRichOutput([ entry ]);
        const notice = result![0] as
        {
            kind: "notice";
            level: string
        };
        expect(notice.level).toBe("info");
    });

    test("contract entry with all fields is preserved", () => {
        const entry = {
            kind : "contract",
            family : "plotly",
            status : "active",
            title : "T",
            summary : "S",
            marker : "BANANA_PLOTLY::",
        };
        const result = normalizeRichOutput([ entry ]);
        expect(result).toBeDefined();
        expect(result![0].kind).toBe("contract");
    });

    test("contract entry missing required fields is skipped", () => {
        const entry = {kind : "contract", family : "plotly"};
        const result = normalizeRichOutput([ entry ]);
        expect(result).toBeUndefined();
    });

    test("null items in array are skipped", () => {
        const entry = {kind : "notice", level : "info", message : "ok"};
        const result = normalizeRichOutput([ null, entry, undefined ]);
        expect(result).toBeDefined();
        expect(result!.length).toBe(1);
    });
});

// ---------------------------------------------------------------------------
// normalizePlotlyFigure
// ---------------------------------------------------------------------------

describe("normalizePlotlyFigure", () => {
    test("null input returns notices only", () => {
        const {figure, notices} = normalizePlotlyFigure(null);
        expect(figure).toBeUndefined();
        expect(notices.length).toBeGreaterThan(0);
    });

    test("missing data array returns notice", () => {
        const {figure, notices} = normalizePlotlyFigure({layout : {}});
        expect(figure).toBeUndefined();
        expect(notices.length).toBeGreaterThan(0);
    });

    test("empty data array returns notice", () => {
        const {figure, notices} = normalizePlotlyFigure({data : []});
        expect(figure).toBeUndefined();
        expect(notices.length).toBeGreaterThan(0);
    });

    test("valid small figure is returned with no notices", () => {
        const raw = {
            data : [ {type : "bar", x : [ "a", "b" ], y : [ 1, 2 ]} ],
            layout : {title : "T"}
        };
        const {figure, notices} = normalizePlotlyFigure(raw);
        expect(figure).toBeDefined();
        expect(notices).toHaveLength(0);
    });

    test("figure layout is preserved", () => {
        const raw = {
            data : [ {type : "bar", x : [ "a" ], y : [ 1 ]} ],
            layout : {title : {text : "My Chart"}}
        };
        const {figure} = normalizePlotlyFigure(raw);
        expect((figure!.layout as {title : {text : string}}).title.text).toBe("My Chart");
    });

    test("large trace is downsampled and notice is added", () => {
        const n = 2000;
        const x = Array.from({length : n}, (_, i) => i);
        const y = Array.from({length : n}, (_, i) => i * 0.5);
        const raw = {data : [ {type : "scatter", x, y} ]};
        const {figure, notices} = normalizePlotlyFigure(raw);
        expect(figure).toBeDefined();
        expect(notices.length).toBeGreaterThan(0);
        expect(notices[0]).toContain("downsampled");
        expect((figure!.data[0] as {x : unknown[]}).x.length).toBeLessThan(n);
    });

    test("non-object items in data array are skipped", () => {
        const raw = {data : [ null, {type : "bar", x : [ "a" ], y : [ 1 ]} ]};
        const {figure} = normalizePlotlyFigure(raw);
        expect(figure).toBeDefined();
        expect(figure!.data.length).toBe(1);
    });
});
