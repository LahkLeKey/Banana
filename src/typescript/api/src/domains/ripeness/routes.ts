import type {FastifyInstance} from 'fastify';

import type {BananaRipenessRequest} from '../../contracts/http.js';
import {ProxyHttpError} from '../../shared/httpClient.js';
import {logDomainCall} from '../../shared/prisma.js';

import {proxyRipenessPrediction} from './proxyClient.js';

function validateRipenessRequest(payload: unknown):
    payload is BananaRipenessRequest {
  if (!payload || typeof payload !== 'object') {
    return false;
  }

  const record = payload as Record<string, unknown>;
  if (typeof record.batchId !== 'string') {
    return false;
  }

  if (!Array.isArray(record.temperatureHistory) ||
      !record.temperatureHistory.every((value) => typeof value === 'number')) {
    return false;
  }

  return typeof record.daysSinceHarvest === 'number' &&
      typeof record.ethyleneExposure === 'number' &&
      typeof record.mechanicalDamage === 'number' &&
      (record.storageTempC === undefined ||
       typeof record.storageTempC === 'number');
}

export function registerRipenessRoutes(
    app: FastifyInstance, legacyApiBaseUrl: string): void {
  app.post('/ripeness/predict', async (request, reply) => {
    if (!validateRipenessRequest(request.body)) {
      const payload = {message: 'invalid ripeness prediction payload.'};
      await logDomainCall(
          'ripeness', '/ripeness/predict', 400, request.body, payload);
      return reply.code(400).send(payload);
    }

    const ripenessRequest = request.body;

    try {
      const upstream =
          await proxyRipenessPrediction(legacyApiBaseUrl, ripenessRequest);
      await logDomainCall(
          'ripeness', '/ripeness/predict', upstream.statusCode, ripenessRequest,
          upstream.payload);
      return reply.code(upstream.statusCode).send(upstream.payload);
    } catch (error) {
      if (error instanceof ProxyHttpError) {
        await logDomainCall(
            'ripeness', '/ripeness/predict', error.statusCode, ripenessRequest,
            error.payload);
        return reply.code(error.statusCode).send(error.payload);
      }

      request.log.error({err: error}, 'ripeness proxy failed');
      const payload = {message: 'ripeness domain proxy failed.'};
      await logDomainCall(
          'ripeness', '/ripeness/predict', 500, ripenessRequest, payload);
      return reply.code(500).send(payload);
    }
  });
}
