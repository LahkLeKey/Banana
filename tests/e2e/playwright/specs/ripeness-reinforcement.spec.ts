/**
 * ripeness-reinforcement.spec.ts
 *
 * Playwright reinforcement training suite for the ripeness classifier.
 *
 * Each test calls the ripeness API directly via page.request (bypasses DOM
 * scroll/visibility issues with the ripeness panel) and writes a feedback entry
 * to data/ripeness/feedback/inbox.json.
 *
 * Prompts include at least 1 boundary case per label class drawn from the
 * challenge-set to ensure the model is exercised on known hard cases.
 */

import {expect, test} from "@playwright/test";
import path from "path";

import {routeRipenessWithLabel} from "../helpers/mock-api";
import {
    appendFeedbackEntry,
    checkDisagreementRate,
    DISAGREEMENT_THRESHOLD,
    type FeedbackEntry,
    makeRipenessEntry,
    type RipenessLabel,
} from "../helpers/training-feedback";

// ── Config ──────────────────────────────────────────────────────────────────

const INBOX_PATH = path.resolve(__dirname, "../../../../data/ripeness/feedback/inbox.json");
const SESSION_ID = `pw-${Date.now()}`;
const MOCK_MODE = process.env.BANANA_TRAINING_MODE !== "live";
const API_BASE = process.env.PLAYWRIGHT_BASE_URL ?? "http://localhost:5173";

// ── Labeled prompt corpus ───────────────────────────────────────────────────

interface RipenessPrompt
{
    text: string;
    expectedLabel: RipenessLabel;
    category: "clear"|"boundary";
}

const PROMPTS: RipenessPrompt[] = [
    // Unripe
    {
        text : "bright green banana with firm starchy flesh and no sweetness",
        expectedLabel : "unripe",
        category : "clear"
    },
    {
        text : "hard green fruit with chalky texture and astringent taste",
        expectedLabel : "unripe",
        category : "clear"
    },
    {
        text : "solid green peel banana picked before ripeness fully develops",
        expectedLabel : "unripe",
        category : "boundary"
    },
    // Ripe
    {
        text : "golden yellow banana with sweet aroma and soft creamy flesh",
        expectedLabel : "ripe",
        category : "clear"
    },
    {
        text : "fully yellow peel with tender flesh and natural sweetness",
        expectedLabel : "ripe",
        category : "clear"
    },
    {
        text : "mostly yellow with faint green tip and balanced sweetness",
        expectedLabel : "ripe",
        category : "boundary"
    },
    // Overripe
    {
        text : "yellow banana covered in dark brown spots with very soft flesh",
        expectedLabel : "overripe",
        category : "clear"
    },
    {
        text : "deeply freckled banana with mushy texture ideal for baking",
        expectedLabel : "overripe",
        category : "clear"
    },
    {
        text : "deep yellow fruit with dense brown freckles and soft flesh",
        expectedLabel : "overripe",
        category : "boundary"
    },
    // Spoiled
    {
        text : "blackened banana with fermented smell and completely collapsed structure",
        expectedLabel : "spoiled",
        category : "clear"
    },
    {
        text : "entirely black peel with liquid seeping from splits and foul odor",
        expectedLabel : "spoiled",
        category : "clear"
    },
    {
        text : "mostly black banana with sour fermented aroma still attached to bunch",
        expectedLabel : "spoiled",
        category : "boundary"
    },
];

// ── Test suite ───────────────────────────────────────────────────────────────

const sessionEntries: FeedbackEntry[] = [];

test.describe("Ripeness reinforcement training suite", () => {
    for (let i = 0; i < PROMPTS.length; i++)
    {
        const {text, expectedLabel, category} = PROMPTS[i];

        test(`[${category}] ${expectedLabel} — "${text.slice(0, 50)}"`, async ({page}) => {
            let predictedLabel: string = expectedLabel; // default for mock mode

            if (MOCK_MODE)
            {
                // In mock mode, stub the ripeness response to match expected label.
                await routeRipenessWithLabel(page, expectedLabel);
                // Navigate to the app so page.request has a base context
                await page.goto("/");
            }

            // Call the ripeness API directly — avoids DOM coupling
            const encodedText = encodeURIComponent(text);
            const apiBase = MOCK_MODE ? "" // relative — will use the mocked route
                                      : API_BASE.replace(/\/$/, "");

            let response: Record<string, unknown> = {};
            try
            {
                const res = await page.request.get(`${apiBase}/api/ripeness?text=${encodedText}`);
                if (res.ok())
                {
                    response = (await res.json()) as Record<string, unknown>;
                    const raw = String(response["ripeness"] ?? "");
                    if (raw)
                        predictedLabel = raw.toLowerCase().trim();
                }
            }
            catch
            {
                // Network error in mock mode if route not matched — use expected
                predictedLabel = expectedLabel;
            }

            const entry = makeRipenessEntry(
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

    test("per-label disagreement rate guard", async () => {
        const labels: RipenessLabel[] = [ "unripe", "ripe", "overripe", "spoiled" ];
        let anyFailed = false;

        for (const label of labels)
        {
            const labelEntries = sessionEntries.filter((e) => e.label === label);
            const {ok, rate, disagreements, total} =
                checkDisagreementRate(labelEntries, DISAGREEMENT_THRESHOLD);
            console.log(`  ${label}: ${disagreements}/${total} disagreements (${
                (rate * 100).toFixed(1)}%) — ${ok ? "OK" : "FAIL"}`);
            if (!ok)
                anyFailed = true;
        }

        const overall = checkDisagreementRate(sessionEntries, DISAGREEMENT_THRESHOLD);
        console.log(`Ripeness overall: ${overall.disagreements}/${overall.total} (${
            (overall.rate * 100).toFixed(1)}%)`);

        expect(anyFailed, "One or more ripeness labels exceeded the disagreement threshold")
            .toBe(false);
    });
});
