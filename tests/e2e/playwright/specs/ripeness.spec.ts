import path from "path";
import { test } from "@playwright/test";
import { routeAllBackgroundCalls, routeRipenessSuccess } from "../helpers/mock-api";

const SCREENSHOTS_DIR = path.resolve(__dirname, "../../../../docs/screenshots");

test.describe("Ripeness panel screenshots", () => {
  test("ripeness-idle", async ({ page }) => {
    await routeAllBackgroundCalls(page);
    await page.goto("/classify");
    await page.waitForSelector("[data-testid='ensemble-form']");

    // Scroll down to reach the ripeness panel if it exists below fold
    await page.evaluate(() => window.scrollTo(0, document.body.scrollHeight));
    await page.waitForTimeout(300);

    await page.screenshot({
      path: path.join(SCREENSHOTS_DIR, "ripeness-idle.png"),
      animations: "disabled",
    });
  });

  test("ripeness-result", async ({ page }) => {
    await routeRipenessSuccess(page);
    await routeAllBackgroundCalls(page);
    await page.goto("/classify");
    await page.waitForSelector("[data-testid='ensemble-form']");

    // Scroll down and interact with ripeness input
    await page.evaluate(() => window.scrollTo(0, document.body.scrollHeight));
    await page.waitForTimeout(300);

    // Find the ripeness textarea — it follows the ensemble form in the DOM
    const ripenessTextarea = page.locator("textarea").nth(1);
    const count = await ripenessTextarea.count();
    if (count > 0) {
      await ripenessTextarea.fill("yellow peel, brown spots");
      // Click the predict button nearest to this textarea
      const predictBtn = page.locator("button").filter({ hasText: /ripe/i }).first();
      const btnCount = await predictBtn.count();
      if (btnCount > 0) {
        await predictBtn.click();
        await page.waitForTimeout(800);
      }
    }

    await page.screenshot({
      path: path.join(SCREENSHOTS_DIR, "ripeness-result.png"),
      animations: "disabled",
    });
  });
});
