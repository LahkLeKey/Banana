import {expect, test} from "@playwright/test";

test.describe("Telemetry dashboard route", () => {
    test("replaces review spikes and exposes wasm/native drilldowns", async ({page}) => {
        await page.goto("/review-spikes");
        await page.waitForURL("**/telemetry");

        await expect(page.locator("[data-testid='telemetry-dashboard-title']")).toBeVisible();
        await expect(page.locator("[data-testid='wasm-worker-drilldown']")).toBeVisible();
        await expect(page.locator("[data-testid='native-drilldown']")).toBeVisible();
        await expect(page.locator("[data-testid='telemetry-readiness-badges']")).toBeVisible();
    });
});
