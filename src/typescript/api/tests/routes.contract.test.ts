import Fastify, {type FastifyInstance} from 'fastify';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import type {AddressInfo} from 'node:net';
import path from 'node:path';
import test from 'node:test';

import {createServer} from '../src/server/createServer.js';
import type {AppRuntimeConfig} from '../src/shared/config.js';

type ProxyHarnessState = {
  lastBananaQuery?: Record<string, string|undefined>;
  lastBatchCreateBody?: Record<string, unknown>;
  lastBatchStatusId?: string;
  lastRipenessBody?: Record<string, unknown>;
  lastMlRegressionBody?: Record<string, unknown>;
  lastMlBinaryBody?: Record<string, unknown>;
  lastMlTransformerBody?: Record<string, unknown>;
};

type ProxyHarness = {
  gateway: FastifyInstance; close: () => Promise<void>;
  state: ProxyHarnessState;
};

type ProxyHarnessOptions = {
  configOverrides?: Partial<AppRuntimeConfig>;
};

const LOCAL_NATIVE_DIR =
    path.resolve(process.cwd(), '../../../build/native/bin/Release');
const LOCAL_NATIVE_LIBRARY = process.platform === 'win32' ?
    path.join(LOCAL_NATIVE_DIR, 'banana_native.dll') :
    process.platform === 'darwin' ?
    path.join(LOCAL_NATIVE_DIR, 'libbanana_native.dylib') :
    path.join(LOCAL_NATIVE_DIR, 'libbanana_native.so');
const MISSING_NATIVE_DIR = path.join(LOCAL_NATIVE_DIR, 'missing-native-path');
const HAS_LOCAL_NATIVE_LIBRARY = fs.existsSync(LOCAL_NATIVE_LIBRARY);

async function createProxyHarness(options: ProxyHarnessOptions = {}):
    Promise<ProxyHarness> {
  const state: ProxyHarnessState = {};
  const upstream = Fastify({logger: false});

  upstream.get('/banana', async (request, reply) => {
    const query = request.query as Record<string, string|undefined>;
    state.lastBananaQuery = {...query};

    if (query.purchases === '999') {
      return reply.code(502).send({message: 'banana upstream failure'});
    }

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

  upstream.post('/batches/create', async (request, reply) => {
    const body = request.body as Record<string, unknown>;
    state.lastBatchCreateBody = body;

    if (body.batchId === 'ERR-503') {
      return reply.code(503).send(
          {statusCode: 503, message: 'batch upstream unavailable'});
    }

    return {
      batchId: body.batchId,
      originFarm: body.originFarm,
      exportStatus: 'Created',
      storageTempC: body.storageTempC,
      ethyleneExposure: body.ethyleneExposure,
      estimatedShelfLifeDays: body.estimatedShelfLifeDays,
    };
  });

  upstream.get('/batches/:id/status', async (request, reply) => {
    const id = (request.params as {id: string}).id;
    state.lastBatchStatusId = id;

    if (id === 'ERR-404') {
      return reply.code(404).send(
          {statusCode: 404, message: 'batch not found upstream'});
    }

    return {
      batchId: id,
      originFarm: 'Farm A',
      exportStatus: 'Queued',
      storageTempC: 12,
      ethyleneExposure: 0.2,
      estimatedShelfLifeDays: 7,
    };
  });

  upstream.post('/ripeness/predict', async (request, reply) => {
    const body = request.body as Record<string, unknown>;
    state.lastRipenessBody = body;

    if (body.batchId === 'ERR-500') {
      return reply.code(500).send(
          {statusCode: 500, message: 'ripeness upstream failure'});
    }

    return {
      batchId: body.batchId,
      predictedStage: 'Ready',
      shelfLifeHours: 42,
      ripeningIndex: 0.78,
      spoilageProbability: 0.13,
      coldChainRisk: 0.08,
    };
  });

  upstream.post('/ml/regression', async (request, reply) => {
    const body = request.body as Record<string, unknown>;
    state.lastMlRegressionBody = body;

    if ((body.features as unknown[] | undefined)?.[0] === -1) {
      return reply.code(422).send({message: 'upstream regression rejection'});
    }

    return {
      score: 0.81,
      modelVersion: 'legacy-regression-v1',
    };
  });

  upstream.post('/ml/binary', async (request, reply) => {
    const body = request.body as Record<string, unknown>;
    state.lastMlBinaryBody = body;

    if ((body.features as unknown[] | undefined)?.[0] === -9) {
      return reply.code(503).send(
          {statusCode: 503, message: 'upstream binary outage'});
    }

    return {
      predictedLabel: 'BANANA',
      bananaProbability: 0.88,
      notBananaProbability: 0.12,
      decisionMargin: 0.76,
      jaccardSimilarity: 0.81,
      confusionMatrix: {
        truePositive: 0.74,
        falsePositive: 0.11,
        falseNegative: 0.08,
        trueNegative: 0.07,
      },
      modelVersion: 'legacy-binary-v1',
    };
  });

  upstream.post('/ml/transformer', async (request, reply) => {
    const body = request.body as Record<string, unknown>;
    state.lastMlTransformerBody = body;

    if ((body.tokenFeatures as unknown[] | undefined)?.[0] === -9) {
      return reply.code(502).send(
          {statusCode: 502, message: 'upstream transformer failure'});
    }

    return {
      predictedLabel: 'NOT_BANANA',
      bananaProbability: 0.08,
      notBananaProbability: 0.92,
      attentionFocus: 0.67,
      modelVersion: 'legacy-transformer-v1',
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
    batchDomainMode: 'proxy',
    bananaDomainMode: 'proxy',
  };

  const mergedConfig: AppRuntimeConfig = {
    ...config,
    ...options.configOverrides,
  };

  const gateway = createServer(mergedConfig, {
    banana: true,
    batch: true,
    ripeness: true,
    ml: true,
    notBanana: true,
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

test('GET /banana rejects non-integer query params', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject(
        {method: 'GET', url: '/banana?purchases=11.5&multiplier=x'});

    assert.equal(response.statusCode, 400);
    assert.deepEqual(
        response.json(),
        {message: 'purchases and multiplier must be integer query params.'});
  } finally {
    await harness.close();
  }
});

test('GET /banana propagates upstream proxy status', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject(
        {method: 'GET', url: '/banana?purchases=999&multiplier=2'});

    assert.equal(response.statusCode, 502);
    assert.deepEqual(response.json(), {message: 'banana upstream failure'});
  } finally {
    await harness.close();
  }
});

test('GET /banana maps transport errors to proxy failure payload', async () => {
  const harness = await createProxyHarness({
    configOverrides: {legacyApiBaseUrl: 'http://127.0.0.1:1'},
  });

  try {
    const response = await harness.gateway.inject(
        {method: 'GET', url: '/banana?purchases=3&multiplier=2'});

    assert.equal(response.statusCode, 500);
    assert.deepEqual(response.json(), {
      message: 'banana domain proxy failed.',
    });
  } finally {
    await harness.close();
  }
});

test('GET /banana returns native unavailable in native mode', async () => {
  const originalNativePath = process.env.BANANA_NATIVE_PATH;
  process.env.BANANA_NATIVE_PATH = MISSING_NATIVE_DIR;
  const harness = await createProxyHarness({
    configOverrides: {bananaDomainMode: 'native'},
  });

  try {
    const response = await harness.gateway.inject(
        {method: 'GET', url: '/banana?purchases=4&multiplier=3'});

    assert.equal(response.statusCode, 500);
    assert.deepEqual(response.json(), {
      message: 'native banana domain is unavailable.',
    });
  } finally {
    await harness.close();
    if (originalNativePath === undefined) {
      delete process.env.BANANA_NATIVE_PATH;
    } else {
      process.env.BANANA_NATIVE_PATH = originalNativePath;
    }
  }
});

test(
    'GET /banana returns native response in native mode',
    {skip: !HAS_LOCAL_NATIVE_LIBRARY}, async () => {
      const originalNativePath = process.env.BANANA_NATIVE_PATH;
      process.env.BANANA_NATIVE_PATH = LOCAL_NATIVE_DIR;
      const harness = await createProxyHarness({
        configOverrides: {bananaDomainMode: 'native'},
      });

      try {
        const response = await harness.gateway.inject(
            {method: 'GET', url: '/banana?purchases=4&multiplier=3'});

        assert.equal(response.statusCode, 200);
        assert.equal(response.json().purchases, 4);
        assert.equal(response.json().multiplier, 3);
        assert.equal(typeof response.json().banana, 'number');
        assert.equal(response.json().banana > 0, true);
        assert.equal(typeof response.json().message, 'string');
      } finally {
        await harness.close();
        if (originalNativePath === undefined) {
          delete process.env.BANANA_NATIVE_PATH;
        } else {
          process.env.BANANA_NATIVE_PATH = originalNativePath;
        }
      }
    });

test(
    'GET /banana returns native failure in native mode when native throws',
    {skip: !HAS_LOCAL_NATIVE_LIBRARY}, async () => {
      const originalNativePath = process.env.BANANA_NATIVE_PATH;
      process.env.BANANA_NATIVE_PATH = LOCAL_NATIVE_DIR;
      const harness = await createProxyHarness({
        configOverrides: {bananaDomainMode: 'native'},
      });

      try {
        const response = await harness.gateway.inject(
            {method: 'GET', url: '/banana?purchases=-3&multiplier=2'});

        assert.equal(response.statusCode, 500);
        assert.equal(
            response.json().message === 'native banana domain failed.' ||
                response.json().message ===
                    'native banana domain is unavailable.',
            true);
      } finally {
        await harness.close();
        if (originalNativePath === undefined) {
          delete process.env.BANANA_NATIVE_PATH;
        } else {
          process.env.BANANA_NATIVE_PATH = originalNativePath;
        }
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

test('POST /batches/create rejects invalid payload', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/batches/create',
      payload: {batchId: 42},
    });

    assert.equal(response.statusCode, 400);
    assert.deepEqual(response.json(), {
      message: 'invalid batch create payload.',
    });
  } finally {
    await harness.close();
  }
});

test('POST /batches/create propagates upstream proxy status', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/batches/create',
      payload: {
        batchId: 'ERR-503',
        originFarm: 'North Farm',
        storageTempC: 13,
        ethyleneExposure: 0.21,
        estimatedShelfLifeDays: 8,
      },
    });

    assert.equal(response.statusCode, 503);
    assert.deepEqual(response.json(), {
      statusCode: 503,
      message: 'batch upstream unavailable',
    });
  } finally {
    await harness.close();
  }
});

test(
    'POST /batches/create maps transport errors to proxy failure payload',
    async () => {
      const harness = await createProxyHarness({
        configOverrides: {legacyApiBaseUrl: 'http://127.0.0.1:1'},
      });

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/batches/create',
          payload: {
            batchId: 'B-PROXY-DOWN',
            originFarm: 'North Farm',
            storageTempC: 13,
            ethyleneExposure: 0.21,
            estimatedShelfLifeDays: 8,
          },
        });

        assert.equal(response.statusCode, 500);
        assert.deepEqual(response.json(), {
          message: 'batch domain proxy failed.',
        });
      } finally {
        await harness.close();
      }
    });

test(
    'POST /batches/create returns native unavailable in native mode',
    async () => {
      const originalNativePath = process.env.BANANA_NATIVE_PATH;
      process.env.BANANA_NATIVE_PATH = MISSING_NATIVE_DIR;
      const harness = await createProxyHarness({
        configOverrides: {batchDomainMode: 'native'},
      });

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/batches/create',
          payload: {
            batchId: 'B-NATIVE-UNAVAILABLE',
            originFarm: 'Native Farm',
            storageTempC: 12,
            ethyleneExposure: 0.2,
            estimatedShelfLifeDays: 7,
          },
        });

        assert.equal(response.statusCode, 500);
        assert.deepEqual(response.json(), {
          message: 'batch native domain is unavailable.',
        });
      } finally {
        await harness.close();
        if (originalNativePath === undefined) {
          delete process.env.BANANA_NATIVE_PATH;
        } else {
          process.env.BANANA_NATIVE_PATH = originalNativePath;
        }
      }
    });

test(
    'POST /batches/create maps native input failure status in native mode',
    {skip: !HAS_LOCAL_NATIVE_LIBRARY}, async () => {
      const originalNativePath = process.env.BANANA_NATIVE_PATH;
      process.env.BANANA_NATIVE_PATH = LOCAL_NATIVE_DIR;
      const harness = await createProxyHarness({
        configOverrides: {batchDomainMode: 'native'},
      });

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/batches/create',
          payload: {
            batchId: '',
            originFarm: 'Native Farm',
            storageTempC: 12,
            ethyleneExposure: 0.2,
            estimatedShelfLifeDays: 7,
          },
        });

        assert.equal(response.statusCode, 400);
        assert.deepEqual(response.json(), {
          message: 'batch native input validation failed.',
        });
      } finally {
        await harness.close();
        if (originalNativePath === undefined) {
          delete process.env.BANANA_NATIVE_PATH;
        } else {
          process.env.BANANA_NATIVE_PATH = originalNativePath;
        }
      }
    });

test(
    'POST /batches/create maps non-status native errors to 500 in native mode',
    async () => {
      const originalNativePath = process.env.BANANA_NATIVE_PATH;
      process.env.BANANA_NATIVE_PATH = LOCAL_NATIVE_DIR;
      const harness = await createProxyHarness({
        configOverrides: {batchDomainMode: 'native'},
      });

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/batches/create',
          payload: {
            batchId: 'B-INVALID-LIFE',
            originFarm: 'Native Farm',
            storageTempC: 12,
            ethyleneExposure: 0.2,
            estimatedShelfLifeDays: 7.5,
          },
        });

        assert.equal(response.statusCode, 500);
        assert.deepEqual(response.json(), {
          message: 'batch native domain failed.',
        });
      } finally {
        await harness.close();
        if (originalNativePath === undefined) {
          delete process.env.BANANA_NATIVE_PATH;
        } else {
          process.env.BANANA_NATIVE_PATH = originalNativePath;
        }
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

test('GET /batches/:id/status rejects blank batch id', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'GET',
      url: '/batches/%20/status',
    });

    assert.equal(response.statusCode, 400);
    assert.deepEqual(response.json(), {
      message: 'batch id is required.',
    });
  } finally {
    await harness.close();
  }
});

test('GET /batches/:id/status propagates upstream proxy status', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'GET',
      url: '/batches/ERR-404/status',
    });

    assert.equal(response.statusCode, 404);
    assert.deepEqual(response.json(), {
      statusCode: 404,
      message: 'batch not found upstream',
    });
  } finally {
    await harness.close();
  }
});

test(
    'GET /batches/:id/status maps transport errors to proxy failure payload',
    async () => {
      const harness = await createProxyHarness({
        configOverrides: {legacyApiBaseUrl: 'http://127.0.0.1:1'},
      });

      try {
        const response = await harness.gateway.inject({
          method: 'GET',
          url: '/batches/B-PROXY-DOWN/status',
        });

        assert.equal(response.statusCode, 500);
        assert.deepEqual(response.json(), {
          message: 'batch domain proxy failed.',
        });
      } finally {
        await harness.close();
      }
    });

test(
    'GET /batches/:id/status returns native unavailable in native mode',
    async () => {
      const originalNativePath = process.env.BANANA_NATIVE_PATH;
      process.env.BANANA_NATIVE_PATH = MISSING_NATIVE_DIR;
      const harness = await createProxyHarness({
        configOverrides: {batchDomainMode: 'native'},
      });

      try {
        const response = await harness.gateway.inject({
          method: 'GET',
          url: '/batches/B-NATIVE-UNAVAILABLE/status',
        });

        assert.equal(response.statusCode, 500);
        assert.deepEqual(response.json(), {
          message: 'batch native domain is unavailable.',
        });
      } finally {
        await harness.close();
        if (originalNativePath === undefined) {
          delete process.env.BANANA_NATIVE_PATH;
        } else {
          process.env.BANANA_NATIVE_PATH = originalNativePath;
        }
      }
    });

test(
    'GET /batches/:id/status maps native not-found status in native mode',
    {skip: !HAS_LOCAL_NATIVE_LIBRARY}, async () => {
      const originalNativePath = process.env.BANANA_NATIVE_PATH;
      process.env.BANANA_NATIVE_PATH = LOCAL_NATIVE_DIR;
      const harness = await createProxyHarness({
        configOverrides: {batchDomainMode: 'native'},
      });

      try {
        const response = await harness.gateway.inject({
          method: 'GET',
          url: '/batches/DOES-NOT-EXIST/status',
        });

        assert.equal(response.statusCode, 404);
        assert.deepEqual(response.json(), {
          message: 'batch was not found in native state.',
        });
      } finally {
        await harness.close();
        if (originalNativePath === undefined) {
          delete process.env.BANANA_NATIVE_PATH;
        } else {
          process.env.BANANA_NATIVE_PATH = originalNativePath;
        }
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

test('POST /ripeness/predict rejects invalid payload', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/ripeness/predict',
      payload: {
        batchId: 'R-200',
        temperatureHistory: ['invalid'],
      },
    });

    assert.equal(response.statusCode, 400);
    assert.deepEqual(response.json(), {
      message: 'invalid ripeness prediction payload.',
    });
  } finally {
    await harness.close();
  }
});

test('POST /ripeness/predict propagates upstream proxy status', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/ripeness/predict',
      payload: {
        batchId: 'ERR-500',
        temperatureHistory: [11, 12, 12.5],
        daysSinceHarvest: 4,
        ethyleneExposure: 0.35,
        mechanicalDamage: 0.12,
      },
    });

    assert.equal(response.statusCode, 500);
    assert.deepEqual(response.json(), {
      statusCode: 500,
      message: 'ripeness upstream failure',
    });
  } finally {
    await harness.close();
  }
});

test(
    'POST /ripeness/predict maps transport errors to proxy failure payload',
    async () => {
      const harness = await createProxyHarness({
        configOverrides: {legacyApiBaseUrl: 'http://127.0.0.1:1'},
      });

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/ripeness/predict',
          payload: {
            batchId: 'R-201',
            temperatureHistory: [11, 12],
            daysSinceHarvest: 2,
            ethyleneExposure: 0.15,
            mechanicalDamage: 0.1,
          },
        });

        assert.equal(response.statusCode, 500);
        assert.deepEqual(response.json(), {
          message: 'ripeness domain proxy failed.',
        });
      } finally {
        await harness.close();
      }
    });

test('POST /ml/regression preserves proxy contract', async () => {
  const harness = await createProxyHarness();
  const payload = {features: [0.5, 0.25, 0.75]};

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/ml/regression',
      payload,
    });

    assert.equal(response.statusCode, 200);
    assert.deepEqual(response.json(), {
      score: 0.81,
      modelVersion: 'legacy-regression-v1',
    });
    assert.deepEqual(harness.state.lastMlRegressionBody, payload);
  } finally {
    await harness.close();
  }
});

test('POST /ml/binary preserves proxy contract', async () => {
  const harness = await createProxyHarness();
  const payload = {features: [0.3, 0.7, 0.2, 0.9]};

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/ml/binary',
      payload,
    });

    assert.equal(response.statusCode, 200);
    assert.equal(response.json().predictedLabel, 'BANANA');
    assert.deepEqual(harness.state.lastMlBinaryBody, payload);
  } finally {
    await harness.close();
  }
});

test('POST /ml/transformer preserves proxy contract', async () => {
  const harness = await createProxyHarness();
  const payload = {tokenFeatures: [0.1, 0.2, 0.3, 0.4, 0.5]};

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/ml/transformer',
      payload,
    });

    assert.equal(response.statusCode, 200);
    assert.equal(response.json().predictedLabel, 'NOT_BANANA');
    assert.deepEqual(harness.state.lastMlTransformerBody, payload);
  } finally {
    await harness.close();
  }
});

test('POST /ml/regression propagates upstream proxy error status', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/ml/regression',
      payload: {features: [-1, 0, 1]},
    });

    assert.equal(response.statusCode, 422);
    assert.deepEqual(
        response.json(), {message: 'upstream regression rejection'});
  } finally {
    await harness.close();
  }
});

test('POST /ml/regression rejects invalid payload', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/ml/regression',
      payload: {features: ['bad']},
    });

    assert.equal(response.statusCode, 400);
    assert.deepEqual(response.json(), {
      message: 'invalid ML regression request payload.',
    });
  } finally {
    await harness.close();
  }
});

test(
    'POST /ml/regression maps transport errors to proxy failure payload',
    async () => {
      const harness = await createProxyHarness({
        configOverrides: {legacyMlApiBaseUrl: 'http://127.0.0.1:1'},
      });

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/ml/regression',
          payload: {features: [0.4, 0.5, 0.6]},
        });

        assert.equal(response.statusCode, 500);
        assert.deepEqual(response.json(), {
          message: 'ML regression domain proxy failed.',
        });
      } finally {
        await harness.close();
      }
    });

test('POST /ml/binary rejects invalid payload', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/ml/binary',
      payload: {features: ['bad']},
    });

    assert.equal(response.statusCode, 400);
    assert.deepEqual(response.json(), {
      message: 'invalid ML binary request payload.',
    });
  } finally {
    await harness.close();
  }
});

test('POST /ml/binary propagates upstream proxy status', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/ml/binary',
      payload: {features: [-9, 0, 0]},
    });

    assert.equal(response.statusCode, 503);
    assert.deepEqual(response.json(), {
      statusCode: 503,
      message: 'upstream binary outage',
    });
  } finally {
    await harness.close();
  }
});

test(
    'POST /ml/binary maps transport errors to proxy failure payload',
    async () => {
      const harness = await createProxyHarness({
        configOverrides: {legacyMlApiBaseUrl: 'http://127.0.0.1:1'},
      });

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/ml/binary',
          payload: {features: [0.5, 0.3]},
        });

        assert.equal(response.statusCode, 500);
        assert.deepEqual(response.json(), {
          message: 'ML binary domain proxy failed.',
        });
      } finally {
        await harness.close();
      }
    });

test('POST /ml/transformer propagates upstream proxy status', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/ml/transformer',
      payload: {tokenFeatures: [-9, 0.2, 0.3]},
    });

    assert.equal(response.statusCode, 502);
    assert.deepEqual(response.json(), {
      statusCode: 502,
      message: 'upstream transformer failure',
    });
  } finally {
    await harness.close();
  }
});

test(
    'POST /ml/transformer maps transport errors to proxy failure payload',
    async () => {
      const harness = await createProxyHarness({
        configOverrides: {legacyMlApiBaseUrl: 'http://127.0.0.1:1'},
      });

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/ml/transformer',
          payload: {tokenFeatures: [0.1, 0.2, 0.3]},
        });

        assert.equal(response.statusCode, 500);
        assert.deepEqual(response.json(), {
          message: 'ML transformer domain proxy failed.',
        });
      } finally {
        await harness.close();
      }
    });

test('POST /ml/transformer rejects invalid payload', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/ml/transformer',
      payload: {tokenFeatures: ['bad']},
    });

    assert.equal(response.statusCode, 400);
    assert.deepEqual(response.json(), {
      message: 'invalid ML transformer request payload.',
    });
  } finally {
    await harness.close();
  }
});

test(
    'POST /not-banana/junk handles polymorphic actor and entity payloads',
    async () => {
      const harness = await createProxyHarness();

      const requestBody = {
        actors: [
          {
            actorType: 'warehouse-worker',
            actorId: 'A-1',
            payload: {shift: 'night', role: 'picker'},
          },
          {
            type: 'camera',
            id: 'A-2',
            payload: {lensState: 'dusty'},
          },
        ],
        entities: [
          {
            entityType: 'crate',
            entityId: 'E-1',
            payload: {weightKg: 8.2},
          },
          {
            kind: 'forklift',
            id: 'E-2',
            payload: {batteryPercent: 63},
          },
        ],
        junk: {
          arbitrary:
              {region: 'dock-west', discardedParts: ['bolt', 'scrap-metal']}
        },
      };

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/not-banana/junk',
          payload: requestBody,
        });

        assert.equal(response.statusCode, 200);
        const body = response.json() as {
          classification: string;
          actorCount: number;
          entityCount: number;
          notBananaProbability: number;
          bananaProbability: number;
          normalizedActors: Array<{actorType: string}>;
          normalizedEntities: Array<{entityType: string}>;
        };

        assert.equal(body.classification, 'NOT_BANANA');
        assert.equal(body.actorCount, 2);
        assert.equal(body.entityCount, 2);
        assert.equal(typeof body.notBananaProbability, 'number');
        assert.equal(typeof body.bananaProbability, 'number');
        assert.equal(body.normalizedActors[0]?.actorType, 'warehouse-worker');
        assert.equal(body.normalizedEntities[1]?.entityType, 'forklift');
      } finally {
        await harness.close();
      }
    });

test('POST /not-banana/junk rejects non-object payload', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/not-banana/junk',
      payload: null,
    });

    assert.equal(response.statusCode, 400);
    assert.deepEqual(response.json(), {
      message: 'not-banana payload must be a JSON object.',
    });
  } finally {
    await harness.close();
  }
});

test(
    'POST /not-banana/junk rejects actor entries without discriminator',
    async () => {
      const harness = await createProxyHarness();

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/not-banana/junk',
          payload: {
            actors: [{actorId: 'A-1'}],
          },
        });

        assert.equal(response.statusCode, 400);
        assert.deepEqual(response.json(), {
          message:
              'actor entries must include actorType/type/kind discriminator.',
        });
      } finally {
        await harness.close();
      }
    });

test(
    'POST /not-banana/junk rejects actor entries with non-object payload',
    async () => {
      const harness = await createProxyHarness();

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/not-banana/junk',
          payload: {
            actors: [42],
          },
        });

        assert.equal(response.statusCode, 400);
        assert.deepEqual(response.json(), {
          message: 'actor entries must be JSON objects.',
        });
      } finally {
        await harness.close();
      }
    });

test(
    'POST /not-banana/junk rejects entity entries without discriminator',
    async () => {
      const harness = await createProxyHarness();

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/not-banana/junk',
          payload: {
            entities: [{entityId: 'E-1'}],
          },
        });

        assert.equal(response.statusCode, 400);
        assert.deepEqual(response.json(), {
          message:
              'entity entries must include entityType/type/kind discriminator.',
        });
      } finally {
        await harness.close();
      }
    });

test(
    'POST /not-banana/junk rejects entity entries with non-object payload',
    async () => {
      const harness = await createProxyHarness();

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/not-banana/junk',
          payload: {
            entities: ['bad-entity'],
          },
        });

        assert.equal(response.statusCode, 400);
        assert.deepEqual(response.json(), {
          message: 'entity entries must be JSON objects.',
        });
      } finally {
        await harness.close();
      }
    });

test(
    'POST /not-banana/junk rejects empty payload without analyzable content',
    async () => {
      const harness = await createProxyHarness();

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/not-banana/junk',
          payload: {},
        });

        assert.equal(response.statusCode, 400);
        assert.deepEqual(response.json(), {
          message:
              'payload must include actors, entities, or arbitrary junk content.',
        });
      } finally {
        await harness.close();
      }
    });

test(
    'POST /not-banana/junk infers junk payload from unknown top-level fields',
    async () => {
      const harness = await createProxyHarness();

      try {
        const response = await harness.gateway.inject({
          method: 'POST',
          url: '/not-banana/junk',
          payload: {
            equipment: ['forklift', 'pallet-jack'],
            facilityZone: 'dock-7',
          },
        });

        assert.equal(response.statusCode, 200);
        const body = response.json() as {
          classification: string;
          actorCount: number;
          entityCount: number;
        };
        assert.equal(body.classification, 'NOT_BANANA');
        assert.equal(body.actorCount, 0);
        assert.equal(body.entityCount, 0);
      } finally {
        await harness.close();
      }
    });

test('POST /not-banana/junk flags banana-like signals for review', async () => {
  const harness = await createProxyHarness();

  try {
    const response = await harness.gateway.inject({
      method: 'POST',
      url: '/not-banana/junk',
      payload: {
        junk: {
          note: 'banana peel and yellow ripe fruit near bunch line',
          sensorTag: 'ethylene-high',
        },
      },
    });

    assert.equal(response.statusCode, 200);
    const body = response.json() as {
      classification: string;
      message: string
    };
    assert.equal(body.classification, 'MAYBE_BANANA');
    assert.equal(
        body.message,
        'object set contains banana-like signals; manual review is recommended.');
  } finally {
    await harness.close();
  }
});
