import type {FastifyInstance} from 'fastify';

import type {BananaMlBinaryRequest, BananaMlRegressionRequest, BananaMlTransformerRequest,} from '../../contracts/http.js';
import {ProxyHttpError} from '../../shared/httpClient.js';
import {logDomainCall} from '../../shared/prisma.js';

import {proxyMlBinary, proxyMlRegression, proxyMlTransformer} from './proxyClient.js';

function isNumberArray(value: unknown): value is number[] {
  return Array.isArray(value) &&
      value.every((entry) => typeof entry === 'number');
}

function validateRegression(payload: unknown):
    payload is BananaMlRegressionRequest {
  return !!payload && typeof payload === 'object' &&
      isNumberArray((payload as Record<string, unknown>).features);
}

function validateBinary(payload: unknown): payload is BananaMlBinaryRequest {
  return !!payload && typeof payload === 'object' &&
      isNumberArray((payload as Record<string, unknown>).features);
}

function validateTransformer(payload: unknown):
    payload is BananaMlTransformerRequest {
  return !!payload && typeof payload === 'object' &&
      isNumberArray((payload as Record<string, unknown>).tokenFeatures);
}

export function registerMlRoutes(
    app: FastifyInstance, legacyMlBaseUrl: string): void {
  app.post('/ml/regression', async (request, reply) => {
    if (!validateRegression(request.body)) {
      const payload = {message: 'invalid ML regression request payload.'};
      await logDomainCall('ml', '/ml/regression', 400, request.body, payload);
      return reply.code(400).send(payload);
    }

    const regressionRequest = request.body;

    try {
      const upstream =
          await proxyMlRegression(legacyMlBaseUrl, regressionRequest);
      await logDomainCall(
          'ml', '/ml/regression', upstream.statusCode, regressionRequest,
          upstream.payload);
      return reply.code(upstream.statusCode).send(upstream.payload);
    } catch (error) {
      if (error instanceof ProxyHttpError) {
        await logDomainCall(
            'ml', '/ml/regression', error.statusCode, regressionRequest,
            error.payload);
        return reply.code(error.statusCode).send(error.payload);
      }

      request.log.error({err: error}, 'ML regression proxy failed');
      const payload = {message: 'ML regression domain proxy failed.'};
      await logDomainCall(
          'ml', '/ml/regression', 500, regressionRequest, payload);
      return reply.code(500).send(payload);
    }
  });

  app.post('/ml/binary', async (request, reply) => {
    if (!validateBinary(request.body)) {
      const payload = {message: 'invalid ML binary request payload.'};
      await logDomainCall('ml', '/ml/binary', 400, request.body, payload);
      return reply.code(400).send(payload);
    }

    const binaryRequest = request.body;

    try {
      const upstream = await proxyMlBinary(legacyMlBaseUrl, binaryRequest);
      await logDomainCall(
          'ml', '/ml/binary', upstream.statusCode, binaryRequest,
          upstream.payload);
      return reply.code(upstream.statusCode).send(upstream.payload);
    } catch (error) {
      if (error instanceof ProxyHttpError) {
        await logDomainCall(
            'ml', '/ml/binary', error.statusCode, binaryRequest, error.payload);
        return reply.code(error.statusCode).send(error.payload);
      }

      request.log.error({err: error}, 'ML binary proxy failed');
      const payload = {message: 'ML binary domain proxy failed.'};
      await logDomainCall('ml', '/ml/binary', 500, binaryRequest, payload);
      return reply.code(500).send(payload);
    }
  });

  app.post('/ml/transformer', async (request, reply) => {
    if (!validateTransformer(request.body)) {
      const payload = {message: 'invalid ML transformer request payload.'};
      await logDomainCall('ml', '/ml/transformer', 400, request.body, payload);
      return reply.code(400).send(payload);
    }

    const transformerRequest = request.body;

    try {
      const upstream =
          await proxyMlTransformer(legacyMlBaseUrl, transformerRequest);
      await logDomainCall(
          'ml', '/ml/transformer', upstream.statusCode, transformerRequest,
          upstream.payload);
      return reply.code(upstream.statusCode).send(upstream.payload);
    } catch (error) {
      if (error instanceof ProxyHttpError) {
        await logDomainCall(
            'ml', '/ml/transformer', error.statusCode, transformerRequest,
            error.payload);
        return reply.code(error.statusCode).send(error.payload);
      }

      request.log.error({err: error}, 'ML transformer proxy failed');
      const payload = {message: 'ML transformer domain proxy failed.'};
      await logDomainCall(
          'ml', '/ml/transformer', 500, transformerRequest, payload);
      return reply.code(500).send(payload);
    }
  });
}
