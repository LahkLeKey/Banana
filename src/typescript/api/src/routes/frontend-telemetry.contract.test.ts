import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';

import {registerFrontendTelemetryRoutes} from './frontend-telemetry.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerFrontendTelemetryRoutes(app);
  await app.ready();
  return app;
}

describe('frontend telemetry contract', () => {
  test('accepts valid component error telemetry payload', async () => {
    const app = await createApp();

    const response = await app.inject({
      method: 'POST',
      url: '/telemetry/component-errors',
      payload: {
        componentName: 'SafeNotebookGameplaySurface',
        message: 'render crash in gameplay panel',
        stack: 'Error: render crash',
        componentStack: 'at SafeNotebookGameplaySurface',
        routePath: '/notebooks',
        userAgent: 'bun-test-agent',
        timestamp: Date.now(),
        metadata: {
          area: 'notebooks-shell',
          boundaryId: 'safe-gameplay-surface',
        },
      },
    });

    expect(response.statusCode).toBe(202);
    expect(response.json()).toMatchObject({accepted: true});

    await app.close();
  });

  test('rejects invalid telemetry payload with validation_error', async () => {
    const app = await createApp();

    const response = await app.inject({
      method: 'POST',
      url: '/telemetry/component-errors',
      payload: {
        componentName: '',
        message: '',
        timestamp: 0,
      },
    });

    expect(response.statusCode).toBe(400);
    expect(response.json()).toMatchObject({
      error: {
        code: 'validation_error',
        message: 'frontend_component_error_telemetry_invalid_payload',
      },
    });

    await app.close();
  });
});