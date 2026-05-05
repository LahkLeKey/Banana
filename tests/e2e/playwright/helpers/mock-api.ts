import {type Page, type Route} from "@playwright/test";

// ────────────────────────────────────────────────────────────────────────────
// Typed mock shapes
// ────────────────────────────────────────────────────────────────────────────

interface EnsembleVerdict
{
    prediction: string;
    confidence: number;
    label: string;
    is_banana: boolean;
    model_version?: string;
}

interface ChatMessage
{
    role: "user"|"assistant";
    content: string;
}

interface RipenessResult
{
    ripeness: string;
    confidence: number;
    label: string;
}

// ────────────────────────────────────────────────────────────────────────────
// Ensemble
// ────────────────────────────────────────────────────────────────────────────

export const MOCK_ENSEMBLE_SUCCESS: EnsembleVerdict = {
    prediction : "banana",
    confidence : 0.97,
    label : "Banana",
    is_banana : true,
    model_version : "mock-v1",
};

export const MOCK_ENSEMBLE_ERROR_STATUS = 503;

export async function routeEnsembleSuccess(page: Page): Promise<void>
{
    await page.route("**/api/ensemble**", (route: Route) => route.fulfill({
        status : 200,
        contentType : "application/json",
        body : JSON.stringify({
            ensemble : MOCK_ENSEMBLE_SUCCESS,
            embedding : Array.from({length : 4}, (_, i) => i * 0.1),
        }),
    }));
}

export async function routeEnsembleError(page: Page): Promise<void>
{
    await page.route("**/api/ensemble**", (route: Route) => route.fulfill({
        status : MOCK_ENSEMBLE_ERROR_STATUS,
        contentType : "application/json",
        body : JSON.stringify({error : "inference unavailable"}),
    }));
}

// ────────────────────────────────────────────────────────────────────────────
// Chat
// ────────────────────────────────────────────────────────────────────────────

export const MOCK_CHAT_SESSION = {
    session_id : "mock-session-001",
    ready : true
};

export const MOCK_CHAT_MESSAGE: ChatMessage = {
    role : "assistant",
    content : "Yes, that looks like a ripe banana!",
};

export async function routeChatReady(page: Page): Promise<void>
{
    await page.route("**/api/chat/session**", (route: Route) => route.fulfill({
        status : 200,
        contentType : "application/json",
        body : JSON.stringify(MOCK_CHAT_SESSION),
    }));
    await page.route("**/chat/session**", (route: Route) => route.fulfill({
        status : 200,
        contentType : "application/json",
        body : JSON.stringify(MOCK_CHAT_SESSION),
    }));
}

export async function routeChatMessageReply(page: Page): Promise<void>
{
    await page.route("**/api/chat/message**", (route: Route) => route.fulfill({
        status : 200,
        contentType : "application/json",
        body : JSON.stringify(MOCK_CHAT_MESSAGE),
    }));
    await page.route("**/chat/message**", (route: Route) => route.fulfill({
        status : 200,
        contentType : "application/json",
        body : JSON.stringify(MOCK_CHAT_MESSAGE),
    }));
}

// ────────────────────────────────────────────────────────────────────────────
// Ripeness
// ────────────────────────────────────────────────────────────────────────────

export const MOCK_RIPENESS_RESULT: RipenessResult = {
    ripeness : "ripe",
    confidence : 0.88,
    label : "Ripe",
};

export async function routeRipenessSuccess(page: Page): Promise<void>
{
    await page.route("**/api/ripeness**", (route: Route) => route.fulfill({
        status : 200,
        contentType : "application/json",
        body : JSON.stringify(MOCK_RIPENESS_RESULT),
    }));
}

/**
 * Routes the ensemble API to return a verdict matching the given expected label.
 * Used by reinforcement suites in mock mode to validate feedback-writing logic
 * without requiring a real model.
 */
export async function routeEnsembleWithLabel(page: Page,
                                             expectedLabel: "BANANA"|"NOT_BANANA"): Promise<void>
{
    const isBanana = expectedLabel === "BANANA";
    await page.route("**/api/ensemble**", (route: Route) => route.fulfill({
        status : 200,
        contentType : "application/json",
        body : JSON.stringify({
            ensemble : {
                prediction : isBanana ? "banana" : "not-banana",
                confidence : 0.92,
                label : isBanana ? "Banana" : "Not Banana",
                is_banana : isBanana,
                model_version : "mock-reinforcement-v1",
            },
            embedding : [],
        }),
    }));
}

/**
 * Routes the ripeness API to return a verdict matching the given ripeness label.
 * Used by ripeness reinforcement suites in mock mode.
 */
export async function routeRipenessWithLabel(page: Page, label: "unripe"|"ripe"|"overripe"|
                                             "spoiled"): Promise<void>
{
    const labelDisplay = label.charAt(0).toUpperCase() + label.slice(1);
    await page.route("**/api/ripeness**", (route: Route) => route.fulfill({
        status : 200,
        contentType : "application/json",
        body : JSON.stringify({
            ripeness : label,
            confidence : 0.89,
            label : labelDisplay,
        }),
    }));
}

// ────────────────────────────────────────────────────────────────────────────
// Summary / bootstrap (silences unhandled API calls)
// ────────────────────────────────────────────────────────────────────────────

export async function routeAllBackgroundCalls(page: Page): Promise<void>
{
    // Silence any summary / status calls so the page loads cleanly
    await page.route(
        "**/api/summary**",
        (route: Route) => route.fulfill(
            {status : 200, contentType : "application/json", body : JSON.stringify({})}));
    await page.route(
        "**/api/health**",
        (route: Route) => route.fulfill(
            {status : 200, contentType : "application/json", body : JSON.stringify({ok : true})}));
}
