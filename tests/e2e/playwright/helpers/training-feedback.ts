/**
 * training-feedback.ts
 *
 * Shared helper for Playwright reinforcement training suites.
 * Appends structured feedback entries to a model's feedback inbox.json
 * so that the existing apply-not-banana-feedback.py pipeline can ingest them
 * after human review.
 */

import fs from "fs";
import path from "path";

// ── Constants ──────────────────────────────────────────────────────────────

/** Fraction of prompts that may disagree before the suite fails. */
export const DISAGREEMENT_THRESHOLD = 0.4;

// ── Types ──────────────────────────────────────────────────────────────────

export type BananaLabel = "BANANA"|"NOT_BANANA";
export type RipenessLabel = "unripe"|"ripe"|"overripe"|"spoiled";

export interface FeedbackEntry {
    id: string;
    label: string;
    payload: {text: string};
    status: "pending"|"pending-positive";
    source: "playwright-reinforcement";
    session_id: string;
    predicted_label: string;
    disagreement: boolean;
    reviewed_by: string[];
    created_at_utc: string;
}

export interface BananaEntryOpts {
    sessionId: string;
    index: number;
    text: string;
    expectedLabel: BananaLabel;
    predictedLabel: string;
}

export interface RipenessEntryOpts {
    sessionId: string;
    index: number;
    text: string;
    expectedLabel: RipenessLabel;
    predictedLabel: string;
}

// ── Helpers ────────────────────────────────────────────────────────────────

function utcNow(): string
{
    return new Date().toISOString().replace(/\.\d{3}Z$/, "Z");
}

function normaliseLabel(raw: string): string
{
    return raw.trim().toLowerCase().replace(/[^a-z0-9_-]/g, "-");
}

export function makeBananaEntry(opts: BananaEntryOpts): FeedbackEntry
{
    const disagreement = normaliseLabel(opts.predictedLabel) !== normaliseLabel(opts.expectedLabel);
    return {
        id : `pw-banana-${opts.sessionId}-${String(opts.index).padStart(3, "0")}`,
        label : opts.expectedLabel,
        payload : {text : opts.text.trim()},
        status : disagreement ? "pending" : "pending-positive",
        source : "playwright-reinforcement",
        session_id : opts.sessionId,
        predicted_label : opts.predictedLabel,
        disagreement,
        reviewed_by : [],
        created_at_utc : utcNow(),
    };
}

export function makeRipenessEntry(opts: RipenessEntryOpts): FeedbackEntry
{
    const disagreement = normaliseLabel(opts.predictedLabel) !== normaliseLabel(opts.expectedLabel);
    return {
        id : `pw-ripeness-${opts.sessionId}-${String(opts.index).padStart(3, "0")}`,
        label : opts.expectedLabel,
        payload : {text : opts.text.trim()},
        status : disagreement ? "pending" : "pending-positive",
        source : "playwright-reinforcement",
        session_id : opts.sessionId,
        predicted_label : opts.predictedLabel,
        disagreement,
        reviewed_by : [],
        created_at_utc : utcNow(),
    };
}

/**
 * Appends a feedback entry to the given inbox JSON file.
 * Creates the file (and parent directories) if absent.
 * Deduplicates by canonical text + expected label to avoid double-writing.
 */
export function appendFeedbackEntry(inboxPath: string, entry: FeedbackEntry): void
{
    const absPath = path.resolve(inboxPath);
    const dir = path.dirname(absPath);
    if (!fs.existsSync(dir))
        fs.mkdirSync(dir, {recursive : true});

    let existing: FeedbackEntry[] = [];
    if (fs.existsSync(absPath))
    {
        try
        {
            existing = JSON.parse(fs.readFileSync(absPath, "utf-8")) as FeedbackEntry[];
            if (!Array.isArray(existing))
                existing = [];
        }
        catch
        {
            existing = [];
        }
    }

    const canonicalKey = `${entry.label}::${entry.payload.text}`;
    const isDuplicate = existing.some((e) => `${e.label}::${e.payload.text}` === canonicalKey);
    if (!isDuplicate)
    {
        existing.push(entry);
        fs.writeFileSync(absPath, JSON.stringify(existing, null, 4) + "\n", "utf-8");
    }
}

/**
 * Asserts disagreement rate is below threshold.
 * Returns true if within threshold, false if exceeded.
 */
export function checkDisagreementRate(entries: FeedbackEntry[], threshold = DISAGREEMENT_THRESHOLD):
    {ok: boolean; rate : number; disagreements : number; total : number}
{
    if (entries.length === 0)
        return {ok : true, rate : 0, disagreements : 0, total : 0};
    const disagreements = entries.filter((e) => e.disagreement).length;
    const rate = disagreements / entries.length;
    return {ok : rate <= threshold, rate, disagreements, total : entries.length};
}
