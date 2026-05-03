/**
 * Playwright config for the DS Pyodide in-browser execution tests.
 *
 * Co-located with src/typescript/react so that @playwright/test resolves
 * from node_modules here.  testDir points back to the spec files at
 * tests/e2e/playwright/specs/.
 *
 * Run via (from src/typescript/react/):
 *   bun run test:ds-e2e
 *
 * These tests hit the live production URL (or PLAYWRIGHT_BASE_URL override).
 * Pyodide loads ~10 MB from CDN, so timeouts are intentionally long.
 */
import {defineConfig, devices} from "@playwright/test";

const baseURL = process.env.PLAYWRIGHT_BASE_URL ?? "https://banana.engineer";

export default defineConfig({
    // Resolved relative to this config file: up 3 levels to repo root, then down
    testDir : "../../../tests/e2e/playwright/specs",
    testMatch : "**/ds-pyodide-execution.spec.ts",
    outputDir : "./test-results/ds-pyodide",
    fullyParallel : false,
    forbidOnly : !!process.env.CI,
    retries : process.env.CI ? 1 : 0,
    workers : 1,
    reporter : [
        [ "list" ], [ "html", {open : "never", outputFolder : "./playwright-report/ds-pyodide"} ]
    ],
    use : {
        baseURL,
        trace : "retain-on-failure",
        screenshot : "only-on-failure",
        video : "retain-on-failure",
        // Pyodide CDN can be slow — allow generous navigation timeouts
        navigationTimeout : 30_000,
        actionTimeout : 15_000,
    },
    projects : [
        {
            name : "ds-pyodide-chromium",
            use : {
                ...devices["Desktop Chrome"],
                viewport : {width : 1440, height : 900},
                launchOptions : {
                    args : [
                        "--no-sandbox",
                        "--disable-setuid-sandbox",
                        "--disable-dev-shm-usage",
                        // Allow SharedArrayBuffer (required by some Pyodide builds)
                        "--enable-features=SharedArrayBuffer",
                    ],
                },
            },
        },
    ],
});
