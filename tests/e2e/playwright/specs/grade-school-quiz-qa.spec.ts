import {expect, test} from "@playwright/test";

test.describe("Quiz QA benchmark and grading", () => {
    test.beforeEach(
        async ({page}) => { await page.goto("/quiz-qa", {waitUntil : "domcontentloaded"}); });

    test("shows benchmark panel and current score fields", async ({page}) => {
        await expect(page.getByTestId("quiz-qa-page")).toBeVisible({timeout : 10_000});
        await expect(page.getByTestId("quiz-benchmark-card")).toBeVisible();
        await expect(page.getByTestId("quiz-benchmark-status")).toBeVisible();
        await expect(page.getByTestId("quiz-current-score")).toBeVisible();
        await expect(page.getByTestId("quiz-previous-score")).toBeVisible();
    });

    test("supports mock attempt creation and human grading", async ({page}) => {
        const addMockButton = page.getByTestId("quiz-add-mock-btn");
        await expect(addMockButton).toBeVisible();

        await addMockButton.click();
        await expect(page.getByTestId("quiz-history-item").first()).toBeVisible();

        const firstGradeSelect = page.getByTestId("quiz-human-grade-select").first();
        await firstGradeSelect.selectOption("pass");
        await expect(firstGradeSelect).toHaveValue("pass");

        const firstNotes = page.getByTestId("quiz-grader-notes").first();
        await firstNotes.fill("Reviewed by QA teacher");
        await expect(firstNotes).toHaveValue("Reviewed by QA teacher");
    });
});
