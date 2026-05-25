import {createHash} from 'node:crypto';

export type RetryClassification = 'recoverable'|'non_recoverable';

export type RetryOrchestrationRequest = {
  generationInputFingerprint: string|number; retryAttempt: number;
  lastFailureCode: number;
};

export type RetryOrchestrationDecision = {
  classification: RetryClassification; shouldRetry: boolean;
  retryFingerprint: string;
  maxRetryAttempts: number;
};

export interface RetryOrchestrationService {
  decide(request: RetryOrchestrationRequest): RetryOrchestrationDecision;
}

const MAX_RETRY_ATTEMPTS = 3;
const RECOVERABLE_CODES = new Set([-11, -12]);

function deterministicRetryFingerprint(request: RetryOrchestrationRequest):
    string {
  const normalized = JSON.stringify({
    generationInputFingerprint: String(request.generationInputFingerprint),
    retryAttempt: request.retryAttempt,
    lastFailureCode: request.lastFailureCode,
  });
  return createHash('sha256').update(normalized).digest('hex');
}

export function createRetryOrchestrationService(): RetryOrchestrationService {
  return {
    decide(request): RetryOrchestrationDecision {
      const classification: RetryClassification =
          RECOVERABLE_CODES.has(request.lastFailureCode) ? 'recoverable' :
                                                           'non_recoverable';
      const shouldRetry = classification === 'recoverable' &&
          request.retryAttempt < MAX_RETRY_ATTEMPTS;

      return {
        classification,
        shouldRetry,
        retryFingerprint: deterministicRetryFingerprint(request),
        maxRetryAttempts: MAX_RETRY_ATTEMPTS,
      };
    },
  };
}
