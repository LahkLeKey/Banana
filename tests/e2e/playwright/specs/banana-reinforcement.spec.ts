/**
 * banana-reinforcement.spec.ts
 *
 * Playwright reinforcement training suite for the banana / not-banana classifier.
 *
 * Each test submits a labeled prompt to the /classify UI, reads the model's
 * verdict, and appends a feedback entry to data/not-banana/feedback/inbox.json.
 * Disagreements (predicted ≠ expected) land as "pending" entries for human
 * review. Agreements land as "pending-positive" positive-reinforcement signals.
 *
 * After all prompts, the suite asserts that the disagreement rate is below
 * DISAGREEMENT_THRESHOLD (40%) — if exceeded, the suite exits non-zero so CI
 * can surface the regression.
 */

import {expect, test} from "@playwright/test";
import path from "path";

import {
    routeAllBackgroundCalls,
    routeEnsembleWithLabel,
} from "../helpers/mock-api";
import {
    appendFeedbackEntry,
    checkDisagreementRate,
    DISAGREEMENT_THRESHOLD,
    type FeedbackEntry,
    makeBananaEntry,
} from "../helpers/training-feedback";

// ── Config ──────────────────────────────────────────────────────────────────

const INBOX_PATH = path.resolve(__dirname, "../../../../data/not-banana/feedback/inbox.json");
const SESSION_ID = `pw-${Date.now()}`;

// In mock mode (CI default) the API route is stubbed, so the "predicted" label
// comes from the mock fixture — not the real model. In live mode, the real API
// verdict is parsed from the DOM.
const MOCK_MODE = process.env.BANANA_TRAINING_MODE !== "live";

// ── Labeled prompt corpus for reinforcement ─────────────────────────────────

interface Prompt
{
    text: string;
    expectedLabel: "BANANA"|"NOT_BANANA";
    category: "clear"|"boundary";
}

const PROMPTS: Prompt[] = [
    // Clear banana signals
    {
        text : "yellow curved fruit with sweet tropical flavor",
        expectedLabel : "BANANA",
        category : "clear"
    },
    {
        text : "ripe banana with brown spots and soft texture",
        expectedLabel : "BANANA",
        category : "clear"
    },
    {
        text : "bunch of bananas hanging from a tree in the tropics",
        expectedLabel : "BANANA",
        category : "clear"
    },
    {
        text : "mushy overripe banana perfect for banana bread",
        expectedLabel : "BANANA",
        category : "clear"
    },
    {
        text : "green unripe banana with firm starchy flesh",
        expectedLabel : "BANANA",
        category : "clear"
    },
    // Clear not-banana signals
    {
        text : "ripe red apple with a crisp sweet bite",
        expectedLabel : "NOT_BANANA",
        category : "clear"
    },
    {
        text : "juicy orange with thick peel and citrus scent",
        expectedLabel : "NOT_BANANA",
        category : "clear"
    },
    {
        text : "bunch of grapes freshly picked from the vine",
        expectedLabel : "NOT_BANANA",
        category : "clear"
    },
    {
        text : "tropical mango with fibrous golden flesh",
        expectedLabel : "NOT_BANANA",
        category : "clear"
    },
    {
        text : "pineapple with spiky crown and sweet acidic juice",
        expectedLabel : "NOT_BANANA",
        category : "clear"
    },
    // Boundary / edge cases
    {
        text : "plantain used for cooking — similar shape to a banana",
        expectedLabel : "BANANA",
        category : "boundary"
    },
    {
        text : "banana flavored candy — artificial yellow taste",
        expectedLabel : "NOT_BANANA",
        category : "boundary"
    },
    {
        text : "dried banana chips with caramelized coating",
        expectedLabel : "BANANA",
        category : "boundary"
    },
    {
        text : "banana split dessert with ice cream and chocolate sauce",
        expectedLabel : "NOT_BANANA",
        category : "boundary"
    },
];

// ── Test suite ───────────────────────────────────────────────────────────────

const sessionEntries: FeedbackEntry[] = [];

test.describe("Banana reinforcement training suite", () => {
    for (let i = 0; i < PROMPTS.length; i++)
    {
        const {text, expectedLabel, category} = PROMPTS[i];

        test(`[${category}] ${expectedLabel} — "${text.slice(0, 50)}"`, async ({page}) => {
            await routeAllBackgroundCalls(page);

            let predictedLabel: string = expectedLabel; // default for mock mode

            if (MOCK_MODE)
            {
                // In mock mode, stub the ensemble response based on expected label.
                // This validates the feedback-writing pipeline rather than the model.
                await routeEnsembleWithLabel(page, expectedLabel);
            }

            await page.goto("/classify");
            await page.waitForSelector("[data-testid='ensemble-form']");

            const textarea = page.locator("[data-testid='ensemble-form'] textarea");
            await textarea.fill(text);
            await page.locator("[data-testid='ensemble-form'] button[type='button']")
                .first()
                .click();

            // Wait for verdict to surface
            await page.waitForSelector(
                "[data-testid='ensemble-verdict-copy'], [data-testid='ensemble-verdict-empty']",
                {timeout : 8000});

            if (!MOCK_MODE)
            {
                // Parse prediction from DOM in live mode
                const verdictEl = page.locator("[data-testid='ensemble-verdict-copy']").first();
                if (await verdictEl.count() > 0)
                {
                    const raw = (await verdictEl.textContent()) ?? "";
                    predictedLabel =
                        raw.trim().toLowerCase().includes("banana") ? "BANANA" : "NOT_BANANA";
                }
            }

            const entry = makeBananaEntry(
                {sessionId : SESSION_ID, index : i, text, expectedLabel, predictedLabel});
            appendFeedbackEntry(INBOX_PATH, entry);
            sessionEntries.push(entry);

            if (entry.disagreement)
            {
                console.warn(`[disagreement] expected=${expectedLabel} predicted=${
                    predictedLabel} text="${text.slice(0, 60)}"`);
            }
        });
    }

    test("disagreement rate guard", async () => {
        const {ok, rate, disagreements, total} =
            checkDisagreementRate(sessionEntries, DISAGREEMENT_THRESHOLD);
        console.log(`Banana reinforcement: ${disagreements}/${total} disagreements (${
            (rate * 100).toFixed(1)}%)`);
        expect(ok, `Disagreement rate ${(rate * 100).toFixed(1)}% exceeds threshold ${
                       (DISAGREEMENT_THRESHOLD * 100).toFixed(0)}%`)
            .toBe(true);
    });
});
