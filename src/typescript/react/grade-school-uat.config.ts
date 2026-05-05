import {defineConfig, devices} from "@playwright/test";

const baseURL = process.env.PLAYWRIGHT_BASE_URL ?? "https://banana.engineer";
const localBaseUrl = /^https?:\/\/(localhost|127\.0\.0\.1)(:\d+)?/i.test(baseURL);

export default defineConfig({
    testDir : "../../../tests/e2e/playwright/specs",
    testMatch : "**/grade-school-quiz-qa.spec.ts",
    outputDir : "./test-results/grade-school-uat",
    fullyParallel : false,
    forbidOnly : !!process.env.CI,
    retries : process.env.CI ? 1 : 0,
    workers : 1,
    reporter : [
        [ "list" ],
        [ "html", {open : "never", outputFolder : "./playwright-report/grade-school-uat"} ]
    ],
    webServer : localBaseUrl ? {
        command : "bun run dev -- --host 127.0.0.1 --port 5173",
        url : baseURL,
        reuseExistingServer : !process.env.CI,
        timeout : 120_000,
    }
                             : undefined,
    use : {
        baseURL,
        trace : "retain-on-failure",
        screenshot : "only-on-failure",
        video : "off",
        navigationTimeout : 20_000,
        actionTimeout : 10_000,
    },
    projects : [
        {
            name : "grade-school-uat-chromium",
            use : {
                ...devices["Desktop Chrome"],
                viewport : {width : 1366, height : 900},
                launchOptions : {
                    args : [
                        "--no-sandbox",
                        "--disable-setuid-sandbox",
                        "--disable-dev-shm-usage",
                    ],
                },
            },
        },
    ],
});
