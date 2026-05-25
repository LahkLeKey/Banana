import {describe, expect, test} from 'bun:test';

import {createFailureContextService} from './failureContextService.ts';

describe('failure context service', () => {
  test('emits operator-facing context for non-recoverable failures', () => {
    const service = createFailureContextService();
    const context = service.build(-21, 'non_recoverable');

    expect(context.operatorMessage)
        .toBe('non-recoverable deterministic generation failure');
    expect(context.remediationHint).toBe('verify_area_identity_inputs');
  });
});
