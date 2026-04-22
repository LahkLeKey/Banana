import type {FastifyInstance} from 'fastify';

import type {BananaBatchCreateRequest} from '../../contracts/http.js';
import type {AppRuntimeConfig} from '../../shared/config.js';
import {ProxyHttpError} from '../../shared/httpClient.js';
import {logDomainCall} from '../../shared/prisma.js';

import {tryCreateBatchViaNative, tryGetBatchStatusViaNative} from './nativeClient.js';
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

function mapNativeStatusToHttp(status: number|undefined): number {
  switch (status) {
    case 1:
    case 2:
      return 400;
    case 6:
      return 404;
    default:
      return 500;
  }
}

function mapNativeStatusMessage(statusCode: number): string {
  switch (statusCode) {
    case 400:
      return 'batch native input validation failed.';
    case 404:
      return 'batch was not found in native state.';
    default:
      return 'batch native domain failed.';
  }
}

export function registerBatchRoutes(
    app: FastifyInstance, config: AppRuntimeConfig): void {
  app.post('/batches/create', async (request, reply) => {
    if (!validateBatchCreateRequest(request.body)) {
      const payload = {message: 'invalid batch create payload.'};
      await logDomainCall(
          'batch', '/batches/create', 400, request.body, payload);
      return reply.code(400).send(payload);
    }

    const batchRequest = request.body;

    const shouldTryNative = config.batchDomainMode !== 'proxy';
    if (shouldTryNative) {
      try {
        const nativeBatch = await tryCreateBatchViaNative(
            batchRequest, {nativePath: process.env.BANANA_NATIVE_PATH});

        if (nativeBatch) {
          reply.header('X-Batch-Domain-Source', 'native-fastify');
          await logDomainCall(
              'batch', '/batches/create', 200, batchRequest, nativeBatch);
          return reply.code(200).send(nativeBatch);
        }

        if (config.batchDomainMode === 'native') {
          const payload = {message: 'batch native domain is unavailable.'};
          await logDomainCall(
              'batch', '/batches/create', 500, batchRequest, payload);
          return reply.code(500).send(payload);
        }
      } catch (error) {
        const nativeStatus = (error as {status?: number}).status;
        const statusCode = mapNativeStatusToHttp(nativeStatus);
        const payload = {message: mapNativeStatusMessage(statusCode)};

        if (config.batchDomainMode === 'native') {
          request.log.error({err: error}, 'batch native create request failed');
          await logDomainCall(
              'batch', '/batches/create', statusCode, batchRequest, payload);
          return reply.code(statusCode).send(payload);
        }

        request.log.warn(
            {err: error}, 'batch native create failed; falling back to proxy');
      }
    }

    try {
      const upstream =
          await proxyBatchCreate(config.legacyApiBaseUrl, batchRequest);
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
      const shouldTryNative = config.batchDomainMode !== 'proxy';
      if (shouldTryNative) {
        try {
          const nativeBatch = await tryGetBatchStatusViaNative(
              id, {nativePath: process.env.BANANA_NATIVE_PATH});

          if (nativeBatch) {
            reply.header('X-Batch-Domain-Source', 'native-fastify');
            await logDomainCall(
                'batch', '/batches/:id/status', 200, {id}, nativeBatch);
            return reply.code(200).send(nativeBatch);
          }

          if (config.batchDomainMode === 'native') {
            const payload = {message: 'batch native domain is unavailable.'};
            await logDomainCall(
                'batch', '/batches/:id/status', 500, {id}, payload);
            return reply.code(500).send(payload);
          }
        } catch (error) {
          const nativeStatus = (error as {status?: number}).status;
          const statusCode = mapNativeStatusToHttp(nativeStatus);
          const payload = {message: mapNativeStatusMessage(statusCode)};

          if (config.batchDomainMode === 'native') {
            request.log.error(
                {err: error}, 'batch native status request failed');
            await logDomainCall(
                'batch', '/batches/:id/status', statusCode, {id}, payload);
            return reply.code(statusCode).send(payload);
          }

          request.log.warn(
              {err: error},
              'batch native status failed; falling back to proxy');
        }
      }

      const upstream = await proxyBatchStatus(config.legacyApiBaseUrl, id);
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
