import type {FastifyInstance} from 'fastify';

import type {BananaRequest} from '../../contracts/http.js';
import type {AppRuntimeConfig} from '../../shared/config.js';
import {ProxyHttpError} from '../../shared/httpClient.js';
import {logDomainCall, saveBananaCalculation} from '../../shared/prisma.js';

import {tryCalculateBananaViaNative} from './nativeClient.js';
import {proxyBananaCalculation} from './proxyClient.js';

function parseIntQuery(value: unknown): number|null {
  const parsed = Number(value);
  if (!Number.isInteger(parsed)) {
    return null;
  }

  return parsed;
}

export function registerBananaRoutes(
    app: FastifyInstance, config: AppRuntimeConfig): void {
  app.get('/banana', async (request, reply) => {
    const purchases =
        parseIntQuery((request.query as Record<string, unknown>).purchases);
    const multiplier =
        parseIntQuery((request.query as Record<string, unknown>).multiplier);

    if (purchases === null || multiplier === null) {
      const payload = {
        message: 'purchases and multiplier must be integer query params.'
      };
      await logDomainCall('banana', '/banana', 400, request.query, payload);
      return reply.code(400).send(payload);
    }

    const bananaRequest: BananaRequest = {purchases, multiplier};

    const shouldTryNative = config.bananaDomainMode !== 'proxy';
    if (shouldTryNative) {
      try {
        const nativeResult = await tryCalculateBananaViaNative(
            bananaRequest, {nativePath: process.env.BANANA_NATIVE_PATH});

        if (nativeResult) {
          if (nativeResult.metadata.dbContract) {
            reply.header(
                'X-Banana-Db-Contract', nativeResult.metadata.dbContract);
          }
          if (nativeResult.metadata.dbSource) {
            reply.header('X-Banana-Db-Source', nativeResult.metadata.dbSource);
          }
          if (nativeResult.metadata.dbRowCount) {
            reply.header(
                'X-Banana-Db-RowCount', nativeResult.metadata.dbRowCount);
          }

          await saveBananaCalculation({
            purchases: nativeResult.response.purchases,
            multiplier: nativeResult.response.multiplier,
            banana: nativeResult.response.banana,
            message: nativeResult.response.message,
            source: 'native-c',
          });
          await logDomainCall(
              'banana', '/banana', 200, bananaRequest, nativeResult.response);
          return reply.code(200).send(nativeResult.response);
        }

        if (config.bananaDomainMode === 'native') {
          const payload = {message: 'native banana domain is unavailable.'};
          await logDomainCall('banana', '/banana', 500, bananaRequest, payload);
          return reply.code(500).send(payload);
        }
      } catch (error) {
        if (config.bananaDomainMode === 'native') {
          request.log.error(
              {err: error}, 'banana native domain request failed');
          const payload = {message: 'native banana domain failed.'};
          await logDomainCall('banana', '/banana', 500, bananaRequest, payload);
          return reply.code(500).send(payload);
        }

        request.log.warn(
            {err: error}, 'banana native domain failed; falling back to proxy');
      }
    }

    try {
      const upstream =
          await proxyBananaCalculation(config.legacyApiBaseUrl, bananaRequest);

      const dbContract = upstream.headers.get('x-banana-db-contract');
      const dbSource = upstream.headers.get('x-banana-db-source');
      const dbRowCount = upstream.headers.get('x-banana-db-rowcount');
      if (dbContract) {
        reply.header('X-Banana-Db-Contract', dbContract);
      }
      if (dbSource) {
        reply.header('X-Banana-Db-Source', dbSource);
      }
      if (dbRowCount) {
        reply.header('X-Banana-Db-RowCount', dbRowCount);
      }

      await saveBananaCalculation({
        purchases: upstream.payload.purchases,
        multiplier: upstream.payload.multiplier,
        banana: upstream.payload.banana,
        message: upstream.payload.message,
        source: 'proxy',
      });
      await logDomainCall(
          'banana', '/banana', upstream.statusCode, bananaRequest,
          upstream.payload);
      return reply.code(upstream.statusCode).send(upstream.payload);
    } catch (error) {
      if (error instanceof ProxyHttpError) {
        await logDomainCall(
            'banana', '/banana', error.statusCode, bananaRequest,
            error.payload);
        return reply.code(error.statusCode).send(error.payload);
      }

      request.log.error({err: error}, 'banana domain request failed');
      const payload = {message: 'banana domain proxy failed.'};
      await logDomainCall('banana', '/banana', 500, bananaRequest, payload);
      return reply.code(500).send(payload);
    }
  });
}
