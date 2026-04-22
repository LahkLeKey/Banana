import Fastify, {type FastifyInstance} from 'fastify';

import {registerBananaRoutes} from '../domains/banana/routes.js';
import {registerBatchRoutes} from '../domains/batch/routes.js';
import {registerMlRoutes} from '../domains/ml/routes.js';
import {registerRipenessRoutes} from '../domains/ripeness/routes.js';
import type {AppRuntimeConfig} from '../shared/config.js';

export type DomainSelection = {
  banana: boolean; batch: boolean; ripeness: boolean; ml: boolean;
};

export function createServer(
    config: AppRuntimeConfig, selection: DomainSelection): FastifyInstance {
  const app = Fastify({logger: true});

  app.get('/health', async () => ({status: 'ok'}));

  if (selection.banana) {
    registerBananaRoutes(app, config);
  }

  if (selection.batch) {
    registerBatchRoutes(app, config);
  }

  if (selection.ripeness) {
    registerRipenessRoutes(app, config.legacyApiBaseUrl);
  }

  if (selection.ml) {
    registerMlRoutes(app, config.legacyMlApiBaseUrl);
  }

  return app;
}
