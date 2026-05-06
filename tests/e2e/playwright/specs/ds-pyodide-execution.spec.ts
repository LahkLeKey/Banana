/**
 * E2E tests for the Data Science page Pyodide in-browser execution.
 *
 * These tests hit the live production URL (banana.engineer) by default, or
 * the PLAYWRIGHT_BASE_URL override if set.  Pyodide loads ~10 MB from CDN,
 * so timeouts are intentionally generous.
 *
 * Architecture:
 *   - The page renders a single CellEditor for the ACTIVE cell only.
 *   - Clicking "+ Python" (inside the Notebook Catalog panel) adds a new cell
 *     and immediately makes it active.
 *   - Stable selectors use data-testid attributes:
 *       [data-testid="notebook-catalog-toggle"]  — opens/closes the catalog
 *       [data-testid="add-python-cell"]          — adds a new Python cell
 *       [data-testid="cell-code-editor"]         — the active cell's textarea
 *       [data-testid="cell-run-btn"]             — runs the active cell
 *
 * Coverage:
 *   1. Runtime badge warms idle -> ready
 *   2. Simple print() produces stdout
 *   3. json.dumps() produces correct output text
 *   4. BANANA_PLOTLY:: marker produces a chart surface, not raw text
 *   5. BANANA_TABLE:: marker produces a table element, not raw text
 *   6. Syntax error surfaces an error pre, not stdout
 *   7. Multi-statement cell accumulates all output lines
 *   8. Variables defined in one run are accessible in the next run
 */

import {expect, test} from "@playwright/test";

// ---------------------------------------------------------------------------
// Config
// ---------------------------------------------------------------------------

const PYODIDE_READY_TIMEOUT = 90_000; // CDN load can be slow
const CELL_RUN_TIMEOUT = 30_000;
const BASE_URL = process.env.PLAYWRIGHT_BASE_URL ?? "https://banana.engineer";

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/** Navigate to /data-science and wait for the page shell to be interactive. */
async function openDsPage(page: import("@playwright/test").Page): Promise<void>
{
    await page.goto(`${BASE_URL}/data-science`, {waitUntil : "domcontentloaded"});
    await page.waitForSelector("button:has-text('\u25b6\u25b6 Run All')", {timeout : 20_000});
}

/**
 * Click the runtime badge to warm Pyodide.
 * Actual badge labels: "o Idle" | "Loading" | "Runtime ready"
 */
async function warmRuntime(page: import("@playwright/test").Page): Promise<void>
{
    const badge = page.locator("button", {hasText : /Idle|Loading|Runtime ready/}).first();
    await badge.waitFor({timeout : 10_000});

    if ((await badge.textContent())?.includes("Idle"))
    {
        await badge.click();
    }

    await expect(badge).toHaveText(/Runtime ready/, {timeout : PYODIDE_READY_TIMEOUT});
}

/**
 * Open the Notebook Catalog panel if it is not already open.
 */
async function ensureCatalogOpen(page: import("@playwright/test").Page): Promise<void>
{
    const addBtn = page.locator("[data-testid='add-python-cell']");
    if (!await addBtn.isVisible())
    {
        await page.locator("[data-testid='notebook-catalog-toggle']").click();
        await expect(addBtn).toBeVisible({timeout : 5_000});
    }
}

/**
 * Add a new Python cell via the catalog, fill it, run it, and wait for
 * execution to complete.  Returns the stdout output pre locator.
 */
async function addAndRunPythonCell(
    page: import("@playwright/test").Page,
    source: string,
    ): Promise<import("@playwright/test").Locator>
{
    await ensureCatalogOpen(page);
    await page.locator("[data-testid='add-python-cell']").click();

    const editor = page.locator("[data-testid='cell-code-editor']").first();
    await expect(editor).toBeVisible({timeout : 5_000});
    await editor.fill(source);

    await page.locator("[data-testid='cell-run-btn']").first().click();

    await page.waitForFunction(
        () => !document.body.innerText.includes("Running\u2026"),
        {timeout : CELL_RUN_TIMEOUT},
    );

    return page.locator("pre.whitespace-pre-wrap").first();
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

test.describe("DS page — Pyodide in-browser execution", () => {
    test.setTimeout(PYODIDE_READY_TIMEOUT + 60_000);

    test("runtime badge warms from idle to ready", async ({page}) => {
        await openDsPage(page);
        await warmRuntime(page);
        await expect(page.locator("button", {hasText : "Runtime ready"})).toBeVisible();
    });

    test("simple print() cell produces stdout output", async ({page}) => {
        await openDsPage(page);
        await warmRuntime(page);
        const outputPre = await addAndRunPythonCell(page, "print('pyodide_exec_ok')");
        await expect(outputPre).toContainText("pyodide_exec_ok", {timeout : CELL_RUN_TIMEOUT});
    });

    test("json.dumps() cell produces valid output text", async ({page}) => {
        await openDsPage(page);
        await warmRuntime(page);

        const src = [
            "import json",
            "d = {'hello': 'banana', 'score': 42}",
            "print(json.dumps(d))",
        ].join("\n");

        const outputPre = await addAndRunPythonCell(page, src);
        await expect(outputPre).toContainText('"hello"', {timeout : CELL_RUN_TIMEOUT});
        await expect(outputPre).toContainText('"banana"', {timeout : CELL_RUN_TIMEOUT});
        await expect(outputPre).toContainText("42", {timeout : CELL_RUN_TIMEOUT});
    });

    test("BANANA_PLOTLY:: cell renders a chart surface, not raw marker text", async ({page}) => {
        await openDsPage(page);
        await warmRuntime(page);

        const src = [
            "import json",
            "figure = {",
            "    'data': [{'type': 'bar', 'x': ['a', 'b', 'c'], 'y': [1, 2, 3]}],",
            "    'layout': {'title': {'text': 'E2E Test Chart'}}",
            "}",
            "print('BANANA_PLOTLY::' + json.dumps(figure))",
        ].join("\n");

        await addAndRunPythonCell(page, src);
        await expect(page.locator(".js-plotly-plot").first()).toBeVisible({timeout : 20_000});
        await expect(page.locator("pre")).not.toContainText("BANANA_PLOTLY::");
    });

    test("BANANA_TABLE:: cell renders a table surface, not raw marker text", async ({page}) => {
        await openDsPage(page);
        await warmRuntime(page);

        const src = [
            "import json",
            "table = {",
            "    'columns': ['lane', 'accuracy'],",
            "    'rows': [['banana', 0.94], ['not-banana', 0.91]]",
            "}",
            "print('BANANA_TABLE::' + json.dumps(table))",
        ].join("\n");

        await addAndRunPythonCell(page, src);
        const tbl = page.locator("table");
        await expect(tbl).toBeVisible({timeout : 15_000});
        await expect(tbl).toContainText("lane");
        await expect(tbl).toContainText("accuracy");
        await expect(page.locator("pre")).not.toContainText("BANANA_TABLE::");
    });

    test("syntax error cell surfaces an error region, not stdout", async ({page}) => {
        await openDsPage(page);
        await warmRuntime(page);
        await addAndRunPythonCell(page, "def broken(:");

        const errorPre = page.locator("pre[class*='red']").first();
        await expect(errorPre).toBeVisible({timeout : CELL_RUN_TIMEOUT});
        await expect(errorPre).toContainText(/SyntaxError|Error/);
    });

    test("multi-statement cell accumulates output lines", async ({page}) => {
        await openDsPage(page);
        await warmRuntime(page);

        const src = [
            "print('line_one')",
            "print('line_two')",
            "print('line_three')",
        ].join("\n");

        const outputPre = await addAndRunPythonCell(page, src);
        await expect(outputPre).toContainText("line_one", {timeout : CELL_RUN_TIMEOUT});
        await expect(outputPre).toContainText("line_two", {timeout : CELL_RUN_TIMEOUT});
        await expect(outputPre).toContainText("line_three", {timeout : CELL_RUN_TIMEOUT});
    });

    test("variables persist across consecutive cell runs", async ({page}) => {
        await openDsPage(page);
        await warmRuntime(page);

        // First cell: define a variable in Pyodide globals()
        await addAndRunPythonCell(page, "shared_value = 'cross_cell_works'");

        // Second cell: read it — globals() is shared across runCell invocations
        await ensureCatalogOpen(page);
        await page.locator("[data-testid='add-python-cell']").click();
        const editor = page.locator("[data-testid='cell-code-editor']").first();
        await expect(editor).toBeVisible({timeout : 5_000});
        await editor.fill("print(shared_value)");
        await page.locator("[data-testid='cell-run-btn']").first().click();
        await page.waitForFunction(
            () => !document.body.innerText.includes("Running\u2026"),
            {timeout : CELL_RUN_TIMEOUT},
        );

        await expect(page.locator("pre.whitespace-pre-wrap").first())
            .toContainText(
                "cross_cell_works",
                {timeout : CELL_RUN_TIMEOUT},
            );
    });
});
