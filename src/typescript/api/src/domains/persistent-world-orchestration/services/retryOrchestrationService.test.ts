import {describe, expect, test} from 'bun:test';

import {createRetryOrchestrationService} from './retryOrchestrationService.ts';

describe('retry orchestration service', () => {
  test('classifies recoverable failures with deterministic fingerprint', () => {
    const service = createRetryOrchestrationService();
    const a = service.decide({
      generationInputFingerprint: 'abc123',
      retryAttempt: 1,
      lastFailureCode: -11,
    });
    const b = service.decide({
      generationInputFingerprint: 'abc123',
      retryAttempt: 1,
      lastFailureCode: -11,
    });

    expect(a.classification).toBe('recoverable');
    expect(a.shouldRetry).toBe(true);
    expect(a.retryFingerprint).toBe(b.retryFingerprint);
  });

  test('classifies non-recoverable failures', () => {
    const service = createRetryOrchestrationService();
    const decision = service.decide({
      generationInputFingerprint: 'abc123',
      retryAttempt: 1,
      lastFailureCode: -21,
    });

    expect(decision.classification).toBe('non_recoverable');
    expect(decision.shouldRetry).toBe(false);
  });
});
