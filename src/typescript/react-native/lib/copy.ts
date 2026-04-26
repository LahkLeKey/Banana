// React Native canonical copy helpers (slice 025).
// Mirrors src/typescript/react/src/lib/copy.ts. Both files are runtime
// projections of
// .specify/specs/020-frontend-classifier-experience-spike/analysis/copy-and-cue-baseline.md
// Drift = bug.

import type {EnsembleVerdict} from '@banana/ui/native';

export const VERDICT_EMPTY_COPY = 'Send a sample to get a verdict.';

export function verdictCopy(verdict: EnsembleVerdict): string {
  if (verdict.label === 'banana') {
    return verdict.did_escalate ? 'Banana \u2014 needs a closer look.' :
                                  'Banana.';
  }
  if (verdict.label === 'not_banana') {
    return verdict.did_escalate ? 'Not a banana \u2014 needs a closer look.' :
                                  'Not a banana.';
  }
  return VERDICT_EMPTY_COPY;
}

export function errorWording(err: unknown): string {
  if (err instanceof Error) {
    const message = err.message.toLowerCase();
    if (message.includes('failed to fetch') || message.includes('network')) {
      return 'Couldn\u2019t reach the banana service. Try again.';
    }
    if (message.includes('timeout') || message.includes('timed out')) {
      return 'That took too long. Try again.';
    }
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
    if (err.message.length > 0) {
      return 'Banana service is having a moment. Try again.';
    }
  }
  return 'Something went wrong. Try again.';
}

export const RETRY_BUTTON_COPY = 'Try again';
