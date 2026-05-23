import type {FastifyInstance} from 'fastify';

import {getDatabaseRuntimeStatus} from '../services/databaseRuntime.ts';

export async function registerHealthRoutes(app: FastifyInstance) {
  app.get('/health', async () => ({
                       status: 'ok',
                       database: getDatabaseRuntimeStatus(),
                     }));
  app.get('/ready', async () => ({
                      status: 'ready',
                      database: getDatabaseRuntimeStatus(),
                    }));
}
