import assert from 'node:assert/strict';
import test from 'node:test';

import {__resetPrismaClientForTests, __setPrismaClientForTests, type DomainName, getPrismaClient, logDomainCall, saveBananaCalculation, saveNotBananaClassification,} from '../src/shared/prisma.js';

type CreateSpy = {
  calls: unknown[]; create: (args: unknown) => Promise<unknown>;
};

function createSpy(options: {throws?: boolean} = {}): CreateSpy {
  const calls: unknown[] = [];

  return {
    calls,
    create: async (args: unknown) => {
      calls.push(args);
      if (options.throws) {
        throw new Error('forced prisma failure');
      }
      return null;
    },
  };
}

test('getPrismaClient returns injected cached client', async () => {
  const fakeClient = {
    bananaApiCall: {create: async () => null},
  };

  __setPrismaClientForTests(fakeClient);

  const first = await getPrismaClient();
  const second = await getPrismaClient();

  assert.equal(first, fakeClient);
  assert.equal(second, fakeClient);
  __resetPrismaClientForTests();
});

test('saveBananaCalculation no-ops when model is unavailable', async () => {
  __setPrismaClientForTests({});

  await saveBananaCalculation({
    purchases: 4,
    multiplier: 3,
    banana: 45,
    message: 'banana payload',
    source: 'proxy',
  });

  __resetPrismaClientForTests();
});

test('saveBananaCalculation writes mapped payload', async () => {
  const bananaCalculation = createSpy();
  __setPrismaClientForTests({bananaCalculation});

  await saveBananaCalculation({
    purchases: 9,
    multiplier: 2,
    banana: 140,
    message: 'banana profile',
    source: 'native-c',
  });

  assert.equal(bananaCalculation.calls.length, 1);
  assert.deepEqual(bananaCalculation.calls[0], {
    data: {
      purchases: 9,
      multiplier: 2,
      banana: 140,
      message: 'banana profile',
      source: 'native-c',
    },
  });
  __resetPrismaClientForTests();
});

test('saveBananaCalculation swallows Prisma create failures', async () => {
  const bananaCalculation = createSpy({throws: true});
  __setPrismaClientForTests({bananaCalculation});

  await saveBananaCalculation({
    purchases: 2,
    multiplier: 1,
    banana: 20,
    message: 'fallback',
    source: 'proxy',
  });

  assert.equal(bananaCalculation.calls.length, 1);
  __resetPrismaClientForTests();
});

test('saveNotBananaClassification writes serialized payload', async () => {
  const notBananaClassification = createSpy();
  __setPrismaClientForTests({notBananaClassification});

  await saveNotBananaClassification({
    classification: 'NOT_BANANA',
    bananaProbability: 0.1,
    notBananaProbability: 0.9,
    junkConfidence: 0.88,
    actorCount: 2,
    entityCount: 3,
    requestBody: undefined,
    responseBody: {classification: 'NOT_BANANA'},
    source: 'classifier',
  });

  assert.equal(notBananaClassification.calls.length, 1);
  assert.deepEqual(notBananaClassification.calls[0], {
    data: {
      classification: 'NOT_BANANA',
      bananaProbability: 0.1,
      notBananaProbability: 0.9,
      junkConfidence: 0.88,
      actorCount: 2,
      entityCount: 3,
      requestJson: 'null',
      responseJson: JSON.stringify({classification: 'NOT_BANANA'}),
      source: 'classifier',
    },
  });
  __resetPrismaClientForTests();
});

test(
    'saveNotBananaClassification swallows Prisma create failures', async () => {
      const notBananaClassification = createSpy({throws: true});
      __setPrismaClientForTests({notBananaClassification});

      await saveNotBananaClassification({
        classification: 'MAYBE_BANANA',
        bananaProbability: 0.45,
        notBananaProbability: 0.55,
        junkConfidence: 0.4,
        actorCount: 1,
        entityCount: 0,
        requestBody: {junk: true},
        responseBody: {classification: 'MAYBE_BANANA'},
        source: 'classifier',
      });

      assert.equal(notBananaClassification.calls.length, 1);
      __resetPrismaClientForTests();
    });

test('logDomainCall no-ops when Prisma client is unavailable', async () => {
  __setPrismaClientForTests(null);

  await logDomainCall('banana', '/banana', 200, {purchases: 2}, {banana: 20});

  __resetPrismaClientForTests();
});

test('logDomainCall routes each domain to matching model', async () => {
  const bananaApiCall = createSpy();
  const batchApiCall = createSpy();
  const ripenessApiCall = createSpy();
  const mlApiCall = createSpy();
  const notBananaApiCall = createSpy();

  __setPrismaClientForTests({
    bananaApiCall,
    batchApiCall,
    ripenessApiCall,
    mlApiCall,
    notBananaApiCall,
  });

  const calls: Array<{domain: DomainName; route: string}> = [
    {domain: 'banana', route: '/banana'},
    {domain: 'batch', route: '/batches/create'},
    {domain: 'ripeness', route: '/ripeness/predict'},
    {domain: 'ml', route: '/ml/regression'},
    {domain: 'not-banana', route: '/not-banana/junk'},
  ];

  for (const call of calls) {
    await logDomainCall(call.domain, call.route, 202, {request: call.route}, {
      response: call.route,
    });
  }

  assert.equal(bananaApiCall.calls.length, 1);
  assert.equal(batchApiCall.calls.length, 1);
  assert.equal(ripenessApiCall.calls.length, 1);
  assert.equal(mlApiCall.calls.length, 1);
  assert.equal(notBananaApiCall.calls.length, 1);

  assert.deepEqual(bananaApiCall.calls[0], {
    data: {
      route: '/banana',
      statusCode: 202,
      requestJson: JSON.stringify({request: '/banana'}),
      responseJson: JSON.stringify({response: '/banana'}),
    },
  });

  __resetPrismaClientForTests();
});

test(
    'logDomainCall swallows model errors and ignores unknown domains',
    async () => {
      const bananaApiCall = createSpy({throws: true});
      const batchApiCall = createSpy();
      __setPrismaClientForTests({bananaApiCall, batchApiCall});

      await logDomainCall('banana', '/banana', 500, {a: 1}, {b: 2});
      await logDomainCall('unknown' as DomainName, '/unknown', 204, {}, {});

      assert.equal(bananaApiCall.calls.length, 1);
      assert.equal(batchApiCall.calls.length, 0);
      __resetPrismaClientForTests();
    });
