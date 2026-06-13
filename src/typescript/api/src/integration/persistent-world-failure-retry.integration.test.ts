import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import {registerWorldRoutes} from '../routes/world.ts';
import {createWorldServiceForTests} from '../routes/world.test-service.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerWorldRoutes(app, {worldService: createWorldServiceForTests()});
  await app.ready();
  return app;
}

describe('US3 deterministic failure retry integration', () => {
  test(
      'classifies retry deterministically and emits operator context',
      async () => {
        const app = await createApp();

        const first = await app.inject({
          method: 'POST',
          url: '/api/world/retry/orchestrate',
          payload: {
            generationInputFingerprint: 'seed-fingerprint-1',
            retryAttempt: 1,
            lastFailureCode: -11,
          },
        });

        const second = await app.inject({
          method: 'POST',
          url: '/api/world/retry/orchestrate',
          payload: {
            generationInputFingerprint: 'seed-fingerprint-1',
            retryAttempt: 1,
            lastFailureCode: -11,
          },
        });

        expect(first.statusCode).toBe(200);
        expect(second.statusCode).toBe(200);

        const a = first.json() as {
          classification: string;
          shouldRetry: boolean;
          retryFingerprint: string;
          operatorContext: {operatorMessage: string; remediationHint: string};
        };
        const b = second.json() as {
          retryFingerprint: string;
        };

        expect(a.classification).toBe('recoverable');
        expect(a.shouldRetry).toBe(true);
        expect(a.retryFingerprint).toBe(b.retryFingerprint);
        expect(a.operatorContext.operatorMessage)
            .toBe('deterministic retry available');
        expect(a.operatorContext.remediationHint)
            .toBe('retry_with_same_generation_inputs');

        const nonRecoverable = await app.inject({
          method: 'POST',
          url: '/api/world/retry/orchestrate',
          payload: {
            generationInputFingerprint: 'seed-fingerprint-2',
            retryAttempt: 1,
            lastFailureCode: -21,
          },
        });

        expect(nonRecoverable.statusCode).toBe(200);
        const nr = nonRecoverable.json() as {
          classification: string;
          shouldRetry: boolean;
          operatorContext: {remediationHint: string};
        };
        expect(nr.classification).toBe('non_recoverable');
        expect(nr.shouldRetry).toBe(false);
        expect(nr.operatorContext.remediationHint)
            .toBe('verify_area_identity_inputs');

        await app.close();
      });
});
