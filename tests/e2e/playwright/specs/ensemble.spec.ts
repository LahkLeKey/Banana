import path from "path";
import { expect, test } from "@playwright/test";
import { routeAllBackgroundCalls, routeEnsembleError, routeEnsembleSuccess } from "../helpers/mock-api";

const SCREENSHOTS_DIR = path.resolve(__dirname, "../../../../docs/screenshots");

test.describe("Ensemble panel screenshots", () => {
  test("ensemble-form-idle", async ({ page }) => {
    await routeAllBackgroundCalls(page);
    await page.goto("/classify");
    await page.waitForSelector("[data-testid='ensemble-form']");

    const form = page.locator("[data-testid='ensemble-form']");
    await expect(form).toBeVisible();

    await form.screenshot({
      path: path.join(SCREENSHOTS_DIR, "ensemble-form-idle.png"),
      animations: "disabled",
    });
  });

  test("ensemble-verdict-success", async ({ page }) => {
    await routeEnsembleSuccess(page);
    await routeAllBackgroundCalls(page);
    await page.goto("/classify");
    await page.waitForSelector("[data-testid='ensemble-form']");

    // Type a sample and submit
    await page.locator("[data-testid='ensemble-form'] textarea").fill("this is a banana");
    await page.locator("[data-testid='ensemble-form'] button[type='button']").first().click();

    // Wait for verdict surface to update
    await page.waitForSelector("[data-testid='ensemble-verdict-copy'], [data-testid='ensemble-verdict-empty']");

    const surface = page.locator("[data-testid='ensemble-verdict-surface']");
    await surface.screenshot({
      path: path.join(SCREENSHOTS_DIR, "ensemble-verdict-success.png"),
      animations: "disabled",
    });
  });

  test("ensemble-verdict-error", async ({ page }) => {
    await routeEnsembleError(page);
    await routeAllBackgroundCalls(page);
    await page.goto("/classify");
    await page.waitForSelector("[data-testid='ensemble-form']");

    await page.locator("[data-testid='ensemble-form'] textarea").fill("this is a banana");
    await page.locator("[data-testid='ensemble-form'] button[type='button']").first().click();

    // Wait for error state
    await page.waitForSelector("[data-testid='ensemble-error'], [data-testid='ensemble-verdict-surface']");

    const surface = page.locator("[data-testid='ensemble-verdict-surface']");
    await surface.screenshot({
      path: path.join(SCREENSHOTS_DIR, "ensemble-verdict-error.png"),
      animations: "disabled",
    });
  });

  test("app-shell-idle", async ({ page }) => {
    await routeAllBackgroundCalls(page);
    await page.goto("/classify");
    await page.waitForSelector("[data-testid='ensemble-form']");

    await page.screenshot({
      path: path.join(SCREENSHOTS_DIR, "app-shell-idle.png"),
      animations: "disabled",
    });
  });

  test("full-page-classify", async ({ page }) => {
    await routeAllBackgroundCalls(page);
    await page.goto("/classify");
    await page.waitForSelector("[data-testid='ensemble-form']");

    await page.screenshot({
      path: path.join(SCREENSHOTS_DIR, "full-page-classify.png"),
      fullPage: true,
      animations: "disabled",
    });
  });
});
