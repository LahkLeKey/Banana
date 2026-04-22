import type {FastifyInstance} from 'fastify';

import type {BananaBatchCreateRequest} from '../../contracts/http.js';
import {ProxyHttpError} from '../../shared/httpClient.js';
import {logDomainCall} from '../../shared/prisma.js';

import {proxyBatchCreate, proxyBatchStatus} from './proxyClient.js';

function validateBatchCreateRequest(payload: unknown):
    payload is BananaBatchCreateRequest {
  if (!payload || typeof payload !== 'object') {
    return false;
  }

  const record = payload as Record<string, unknown>;
  return typeof record.batchId === 'string' &&
      typeof record.originFarm === 'string' &&
      typeof record.storageTempC === 'number' &&
      typeof record.ethyleneExposure === 'number' &&
      typeof record.estimatedShelfLifeDays === 'number';
}

export function registerBatchRoutes(
    app: FastifyInstance, legacyApiBaseUrl: string): void {
  app.post('/batches/create', async (request, reply) => {
    if (!validateBatchCreateRequest(request.body)) {
      const payload = {message: 'invalid batch create payload.'};
      await logDomainCall(
          'batch', '/batches/create', 400, request.body, payload);
      return reply.code(400).send(payload);
    }

    const batchRequest = request.body;

    try {
      const upstream = await proxyBatchCreate(legacyApiBaseUrl, batchRequest);
      await logDomainCall(
          'batch', '/batches/create', upstream.statusCode, batchRequest,
          upstream.payload);
      return reply.code(upstream.statusCode).send(upstream.payload);
    } catch (error) {
      if (error instanceof ProxyHttpError) {
        await logDomainCall(
            'batch', '/batches/create', error.statusCode, batchRequest,
            error.payload);
        return reply.code(error.statusCode).send(error.payload);
      }

      request.log.error({err: error}, 'batch create proxy failed');
      const payload = {message: 'batch domain proxy failed.'};
      await logDomainCall(
          'batch', '/batches/create', 500, batchRequest, payload);
      return reply.code(500).send(payload);
    }
  });

  app.get('/batches/:id/status', async (request, reply) => {
    const id = (request.params as Record<string, string>).id;
    if (!id || id.trim().length === 0) {
      const payload = {message: 'batch id is required.'};
      await logDomainCall(
          'batch', '/batches/:id/status', 400, request.params, payload);
      return reply.code(400).send(payload);
    }

    try {
      const upstream = await proxyBatchStatus(legacyApiBaseUrl, id);
      await logDomainCall(
          'batch', '/batches/:id/status', upstream.statusCode, {id},
          upstream.payload);
      return reply.code(upstream.statusCode).send(upstream.payload);
    } catch (error) {
      if (error instanceof ProxyHttpError) {
        await logDomainCall(
            'batch', '/batches/:id/status', error.statusCode, {id},
            error.payload);
        return reply.code(error.statusCode).send(error.payload);
      }

      request.log.error({err: error}, 'batch status proxy failed');
      const payload = {message: 'batch domain proxy failed.'};
      await logDomainCall('batch', '/batches/:id/status', 500, {id}, payload);
      return reply.code(500).send(payload);
    }
  });
}
