// Slice 023 -- canonical copy + error wording helpers.
//
// The strings here are the runtime mirror of
// .specify/specs/020-frontend-classifier-experience-spike/analysis/copy-and-cue-baseline.md
// Drift = bug. Adding a new state requires a new row in the baseline
// FIRST.

import type {EnsembleVerdict} from '@banana/ui';

export const VERDICT_EMPTY_COPY = 'Send a sample to get a verdict.';

/**
 * Maps a verdict to the baseline display copy. Mirrors the four
 * (label, did_escalate) rows in copy-and-cue-baseline.md.
 */
export function verdictCopy(verdict: EnsembleVerdict): string {
  if (verdict.label === 'banana') {
    return verdict.did_escalate ? 'Banana \u2014 needs a closer look.' :
                                  'Banana.';
  }
  if (verdict.label === 'not_banana') {
    return verdict.did_escalate ? 'Not a banana \u2014 needs a closer look.' :
                                  'Not a banana.';
  }
  // Unknown is not in the baseline; fall back to the empty-state prompt
  // so we don't introduce off-baseline copy. Adding a row to the baseline
  // is the contract path.
  return VERDICT_EMPTY_COPY;
}

/**
 * Maps a thrown Error (or unknown) to the baseline error wording. The
 * matching is intentionally narrow: only signals we can confidently
 * detect from a `fetch` rejection or our own `requestJson` shape are
 * mapped to specific strings; everything else falls back to "Something
 * went wrong. Try again."
 */
export function errorWording(err: unknown): string {
  if (err instanceof Error) {
    const message = err.message.toLowerCase();
    if (message.includes('failed to fetch') || message.includes('network')) {
      return 'Couldn\u2019t reach the banana service. Try again.';
    }
    if (message.includes('timeout') || message.includes('timed out')) {
      return 'That took too long. Try again.';
    }
    // Our requestJson surfaces "request failed (NNN)" for non-OK
    // responses; classify by status family.
    const match = err.message.match(/request failed \((\d+)\)/i);
    if (match) {
      const status = Number.parseInt(match[1], 10);
      if (status >= 500) {
        return 'Banana service is having a moment. Try again.';
      }
      if (status >= 400) {
        return 'That sample isn\u2019t quite right. Adjust and try again.';
      }
    }
    // Backend-shaped error message (e.g. "banana backend down") --
    // treat as 5xx-class.
    if (err.message.length > 0) {
      return 'Banana service is having a moment. Try again.';
    }
  }
  return 'Something went wrong. Try again.';
}

export const RETRY_BUTTON_COPY = 'Try again';
