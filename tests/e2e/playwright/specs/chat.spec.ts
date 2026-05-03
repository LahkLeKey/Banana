import path from "path";
import { expect, test } from "@playwright/test";
import { routeAllBackgroundCalls, routeChatMessageReply, routeChatReady } from "../helpers/mock-api";

const SCREENSHOTS_DIR = path.resolve(__dirname, "../../../../docs/screenshots");

test.describe("Chat panel screenshots", () => {
  test("chat-empty", async ({ page }) => {
    await routeChatReady(page);
    await routeAllBackgroundCalls(page);
    await page.goto("/classify");

    // Wait for the page to settle — chat panel loads after the ensemble form
    await page.waitForSelector("[data-testid='ensemble-form']");
    await page.waitForTimeout(500);

    await page.screenshot({
      path: path.join(SCREENSHOTS_DIR, "chat-empty.png"),
      animations: "disabled",
    });
  });

  test("chat-with-messages", async ({ page }) => {
    await routeChatReady(page);
    await routeChatMessageReply(page);
    await routeAllBackgroundCalls(page);
    await page.goto("/classify");
    await page.waitForSelector("[data-testid='ensemble-form']");
    await page.waitForTimeout(600);

    // Find and fill the chat input — try both input and textarea
    const chatInput = page
      .locator("input[placeholder], textarea[placeholder]")
      .filter({ hasText: "" })
      .last();

    const inputCount = await chatInput.count();
    if (inputCount > 0) {
      await chatInput.fill("Is this a banana?");
      await chatInput.press("Enter");
      // Allow the mock reply to appear
      await page.waitForTimeout(800);
    }

    await page.screenshot({
      path: path.join(SCREENSHOTS_DIR, "chat-with-messages.png"),
      animations: "disabled",
    });
  });
});
