import Fastify, {type FastifyInstance} from 'fastify';
import assert from 'node:assert/strict';
import type {AddressInfo} from 'node:net';
import test from 'node:test';

import {createServer} from '../src/server/createServer.js';
import type {AppRuntimeConfig} from '../src/shared/config.js';

type ProxyHarnessState = {
  lastBananaQuery?: Record<string, string|undefined>;
  lastBatchCreateBody?: Record<string, unknown>;
  lastBatchStatusId?: string;
  lastRipenessBody?: Record<string, unknown>;
};

type ProxyHarness = {
  gateway: FastifyInstance; close: () => Promise<void>;
  state: ProxyHarnessState;
};

async function createProxyHarness(): Promise<ProxyHarness> {
  const state: ProxyHarnessState = {};
  const upstream = Fastify({logger: false});

  upstream.get('/banana', async (request, reply) => {
    const query = request.query as Record<string, string|undefined>;
    state.lastBananaQuery = {...query};

    reply.header('X-Banana-Db-Contract', 'BananaProfileProjection');
    reply.header('X-Banana-Db-Source', 'legacy-api');
    reply.header('X-Banana-Db-RowCount', '1');

    return {
      purchases: Number(query.purchases ?? 0),
      multiplier: Number(query.multiplier ?? 0),
      banana: Number(query.purchases ?? 0) * Number(query.multiplier ?? 0),
      message: 'legacy banana response'
    };
  });

  upstream.post('/batches/create', async (request) => {
    const body = request.body as Record<string, unknown>;
    state.lastBatchCreateBody = body;

    return {
      batchId: body.batchId,
      originFarm: body.originFarm,
      exportStatus: 'Created',
      storageTempC: body.storageTempC,
      ethyleneExposure: body.ethyleneExposure,
      estimatedShelfLifeDays: body.estimatedShelfLifeDays,
    };
  });

  upstream.get('/batches/:id/status', async (request) => {
    const id = (request.params as {id: string}).id;
    state.lastBatchStatusId = id;

    return {
      batchId: id,
      originFarm: 'Farm A',
      exportStatus: 'Queued',
      storageTempC: 12,
      ethyleneExposure: 0.2,
      estimatedShelfLifeDays: 7,
    };
  });

  upstream.post('/ripeness/predict', async (request) => {
    const body = request.body as Record<string, unknown>;
    state.lastRipenessBody = body;

    return {
      batchId: body.batchId,
      predictedStage: 'Ready',
      shelfLifeHours: 42,
      ripeningIndex: 0.78,
      spoilageProbability: 0.13,
      coldChainRisk: 0.08,
    };
  });

  await upstream.listen({host: '127.0.0.1', port: 0});
  const upstreamAddress = upstream.server.address();

  if (!upstreamAddress || typeof upstreamAddress === 'string') {
    throw new Error('failed to bind upstream test server');
  }

  const config: AppRuntimeConfig = {
    host: '127.0.0.1',
    gatewayPort: 8180,
    bananaPort: 8181,
    batchPort: 8182,
    ripenessPort: 8183,
    mlPort: 8184,
    legacyApiBaseUrl:
        `http://127.0.0.1:${(upstreamAddress as AddressInfo).port}`,
    legacyMlApiBaseUrl:
        `http://127.0.0.1:${(upstreamAddress as AddressInfo).port}`,
    bananaDomainMode: 'proxy',
  };

  const gateway = createServer(config, {
    banana: true,
    batch: true,
    ripeness: true,
    ml: false,
  });

  await gateway.ready();

  return {
    gateway,
    state,
    close: async () => {
      await Promise.all([gateway.close(), upstream.close()]);
    }
  };
}

test('GET /health returns parity payload', async () => {
  const harness = await createProxyHarness();

  try {
    const response =
        await harness.gateway.inject({method: 'GET', url: '/health'});
    assert.equal(response.statusCode, 200);
    assert.deepEqual(response.json(), {status: 'ok'});
  } finally {
    await harness.close();
  }
});

test('GET /banana forwards query and metadata headers', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject(
        {method: 'GET', url: '/banana?purchases=11&multiplier=4'});

    assert.equal(response.statusCode, 200);
    assert.deepEqual(response.json(), {
      purchases: 11,
      multiplier: 4,
      banana: 44,
      message: 'legacy banana response',
    });

    assert.equal(
        response.headers['x-banana-db-contract'], 'BananaProfileProjection');
    assert.equal(response.headers['x-banana-db-source'], 'legacy-api');
    assert.equal(response.headers['x-banana-db-rowcount'], '1');

    assert.deepEqual(harness.state.lastBananaQuery, {
      purchases: '11',
      multiplier: '4',
    });
  } finally {
    await harness.close();
  }
});

test('POST /batches/create preserves payload contract', async () => {
  const harness = await createProxyHarness();

  const requestBody = {
    batchId: 'B-100',
    originFarm: 'North Farm',
    storageTempC: 13,
    ethyleneExposure: 0.21,
    estimatedShelfLifeDays: 8,
  };

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/batches/create',
      payload: requestBody,
    });

    assert.equal(response.statusCode, 200);
    assert.deepEqual(response.json(), {
      batchId: 'B-100',
      originFarm: 'North Farm',
      exportStatus: 'Created',
      storageTempC: 13,
      ethyleneExposure: 0.21,
      estimatedShelfLifeDays: 8,
    });

    assert.deepEqual(harness.state.lastBatchCreateBody, requestBody);
  } finally {
    await harness.close();
  }
});

test('GET /batches/:id/status preserves path contract', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'GET',
      url: '/batches/B-100/status',
    });

    assert.equal(response.statusCode, 200);
    assert.deepEqual(response.json(), {
      batchId: 'B-100',
      originFarm: 'Farm A',
      exportStatus: 'Queued',
      storageTempC: 12,
      ethyleneExposure: 0.2,
      estimatedShelfLifeDays: 7,
    });

    assert.equal(harness.state.lastBatchStatusId, 'B-100');
  } finally {
    await harness.close();
  }
});

test('POST /ripeness/predict preserves prediction contract', async () => {
  const harness = await createProxyHarness();

  const requestBody = {
    batchId: 'R-200',
    temperatureHistory: [11, 12, 12.5],
    daysSinceHarvest: 4,
    ethyleneExposure: 0.35,
    mechanicalDamage: 0.12,
    storageTempC: 12,
  };

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/ripeness/predict',
      payload: requestBody,
    });

    assert.equal(response.statusCode, 200);
    assert.deepEqual(response.json(), {
      batchId: 'R-200',
      predictedStage: 'Ready',
      shelfLifeHours: 42,
      ripeningIndex: 0.78,
      spoilageProbability: 0.13,
      coldChainRisk: 0.08,
    });

    assert.deepEqual(harness.state.lastRipenessBody, requestBody);
  } finally {
    await harness.close();
  }
});
