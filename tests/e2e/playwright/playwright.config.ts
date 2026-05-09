import {defineConfig, devices} from "@playwright/test";

const playwrightPort = process.env.PLAYWRIGHT_PORT ?? "5173";
const baseURL = process.env.PLAYWRIGHT_BASE_URL ?? `http://127.0.0.1:${playwrightPort}`;
const reuseExistingServer = process.env.PLAYWRIGHT_REUSE_SERVER === "1" ? true : !process.env.CI;

export default defineConfig({
    testDir : "./specs",
    outputDir : "./test-results",
    fullyParallel : false,
    forbidOnly : !!process.env.CI,
    retries : process.env.CI ? 1 : 0,
    workers : 1,
    reporter : [ [ "list" ], [ "html", {open : "never", outputFolder : "./playwright-report"} ] ],
    webServer : {
        command : `cd ../../../src/typescript/react && bun run dev -- --host 127.0.0.1 --port ${
            playwrightPort}`,
        url : baseURL,
        reuseExistingServer,
        timeout : 120_000,
    },
    use : {
        baseURL,
        trace : "retain-on-failure",
        screenshot : "only-on-failure",
        video : "off",
    },
    projects : [
        {
            name : "screenshots",
            testMatch : "**/*.spec.ts",
            use : {
                ...devices["Desktop Chrome"],
                viewport : {width : 1280, height : 800},
                launchOptions : {
                    args :
                        [ "--no-sandbox", "--disable-setuid-sandbox", "--disable-dev-shm-usage" ],
                },
            },
        },
    ],
});
