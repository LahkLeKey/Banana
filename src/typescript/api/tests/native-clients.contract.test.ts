import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import test from 'node:test';

import {tryCalculateBananaViaNative} from '../src/domains/banana/nativeClient.js';
import {tryCreateBatchViaNative, tryGetBatchStatusViaNative} from '../src/domains/batch/nativeClient.js';

const LOCAL_NATIVE_DIR =
    path.resolve(process.cwd(), '../../../build/native/bin/Release');
const LOCAL_NATIVE_LIBRARY = process.platform === 'win32' ?
    path.join(LOCAL_NATIVE_DIR, 'banana_native.dll') :
    process.platform === 'darwin' ?
    path.join(LOCAL_NATIVE_DIR, 'libbanana_native.dylib') :
    path.join(LOCAL_NATIVE_DIR, 'libbanana_native.so');
const MISSING_NATIVE_DIR = path.join(LOCAL_NATIVE_DIR, 'missing-native-path');
const HAS_LOCAL_NATIVE_LIBRARY = fs.existsSync(LOCAL_NATIVE_LIBRARY);

function isNativeStatusError(
    error: unknown, expectedStatus: number): error is Error&{
  status: number
}
{
  if (!(error instanceof Error)) {
    return false;
  }

  const status = (error as Error & {status?: unknown}).status;
  return typeof status === 'number' && status === expectedStatus;
}

test('banana native client returns null when bridge cannot load', async () => {
  const result = await tryCalculateBananaViaNative(
      {purchases: 3, multiplier: 2}, {nativePath: MISSING_NATIVE_DIR});

  assert.equal(result, null);
});

test(
    'banana native client retries failed bridge load for same path',
    async () => {
      const first = await tryCalculateBananaViaNative(
          {purchases: 3, multiplier: 2}, {nativePath: MISSING_NATIVE_DIR});
      const second = await tryCalculateBananaViaNative(
          {purchases: 3, multiplier: 2}, {nativePath: MISSING_NATIVE_DIR});

      assert.equal(first, null);
      assert.equal(second, null);
    });

test(
    'banana native client reuses loaded bridge for repeated calls',
    {skip: !HAS_LOCAL_NATIVE_LIBRARY}, async () => {
      const first = await tryCalculateBananaViaNative(
          {purchases: 5, multiplier: 2},
          {nativePath: LOCAL_NATIVE_DIR, includeDbQuery: false});
      const second = await tryCalculateBananaViaNative(
          {purchases: 6, multiplier: 2},
          {nativePath: LOCAL_NATIVE_DIR, includeDbQuery: false});

      assert.notEqual(first, null);
      assert.notEqual(second, null);
      assert.equal(first.response.purchases, 5);
      assert.equal(second.response.purchases, 6);
    });

test('native clients resolve darwin and linux library names', async () => {
  const originalPlatform = process.platform;

  try {
    (process as unknown as {platform: string}).platform = 'darwin';
    const darwinBanana = await tryCalculateBananaViaNative(
        {purchases: 1, multiplier: 1}, {nativePath: MISSING_NATIVE_DIR});
    assert.equal(darwinBanana, null);

    (process as unknown as {platform: string}).platform = 'linux';
    const linuxBatch = await tryCreateBatchViaNative(
        {
          batchId: 'platform-linux-native-client',
          originFarm: 'farm-a',
          storageTempC: 12,
          ethyleneExposure: 0.2,
          estimatedShelfLifeDays: 7,
        },
        {nativePath: MISSING_NATIVE_DIR});
    assert.equal(linuxBatch, null);
  } finally {
    (process as unknown as {platform: string}).platform = originalPlatform;
  }
});

test(
    'banana native client validates int32 input constraints',
    {skip: !HAS_LOCAL_NATIVE_LIBRARY}, async () => {
      await assert.rejects(
          () => tryCalculateBananaViaNative(
              {purchases: 1.5, multiplier: 2}, {nativePath: LOCAL_NATIVE_DIR}),
          {name: 'TypeError'});

      await assert.rejects(
          () => tryCalculateBananaViaNative(
              {purchases: 2147483648, multiplier: 2},
              {nativePath: LOCAL_NATIVE_DIR}),
          {name: 'RangeError'});
    });

test(
    'banana native client returns response and skips db metadata when requested',
    {skip: !HAS_LOCAL_NATIVE_LIBRARY}, async () => {
      const result = await tryCalculateBananaViaNative(
          {purchases: 7, multiplier: 3},
          {nativePath: LOCAL_NATIVE_DIR, includeDbQuery: false});

      assert.notEqual(result, null);
      assert.equal(result.response.purchases, 7);
      assert.equal(result.response.multiplier, 3);
      assert.equal(typeof result.response.banana, 'number');
      assert.equal(result.response.banana > 0, true);
      assert.equal(typeof result.response.message, 'string');
      assert.deepEqual(result.metadata, {});
    });

test(
    'banana native client surfaces native status errors',
    {skip: !HAS_LOCAL_NATIVE_LIBRARY}, async () => {
      await assert.rejects(
          () => tryCalculateBananaViaNative(
              {purchases: -2, multiplier: 3},
              {nativePath: LOCAL_NATIVE_DIR, includeDbQuery: false}),
          (error) => {
            assert.equal(isNativeStatusError(error, 1), true);
            return true;
          });
    });

test('batch native client returns null when bridge cannot load', async () => {
  const createResult = await tryCreateBatchViaNative(
      {
        batchId: 'native-client-missing-lib',
        originFarm: 'farm-a',
        storageTempC: 12,
        ethyleneExposure: 0.2,
        estimatedShelfLifeDays: 7,
      },
      {nativePath: MISSING_NATIVE_DIR});

  const getResult = await tryGetBatchStatusViaNative(
      'native-client-missing-lib', {nativePath: MISSING_NATIVE_DIR});

  assert.equal(createResult, null);
  assert.equal(getResult, null);
});

test(
    'batch native client retries failed bridge load for same path',
    async () => {
      const first = await tryGetBatchStatusViaNative(
          'native-client-retry-missing-lib', {nativePath: MISSING_NATIVE_DIR});
      const second = await tryGetBatchStatusViaNative(
          'native-client-retry-missing-lib', {nativePath: MISSING_NATIVE_DIR});

      assert.equal(first, null);
      assert.equal(second, null);
    });

test(
    'batch native client validates estimated shelf-life int32 constraints',
    {skip: !HAS_LOCAL_NATIVE_LIBRARY}, async () => {
      await assert.rejects(
          () => tryCreateBatchViaNative(
              {
                batchId: 'native-client-bad-shelf-life',
                originFarm: 'farm-a',
                storageTempC: 12,
                ethyleneExposure: 0.2,
                estimatedShelfLifeDays: 7.5,
              },
              {nativePath: LOCAL_NATIVE_DIR}),
          {name: 'TypeError'});

      await assert.rejects(
          () => tryCreateBatchViaNative(
              {
                batchId: 'native-client-range-shelf-life',
                originFarm: 'farm-a',
                storageTempC: 12,
                ethyleneExposure: 0.2,
                estimatedShelfLifeDays: 2147483648,
              },
              {nativePath: LOCAL_NATIVE_DIR}),
          {name: 'RangeError'});
    });

test(
    'batch native client creates and retrieves batch payloads',
    {skip: !HAS_LOCAL_NATIVE_LIBRARY}, async () => {
      const batchId = `native-client-batch-${Date.now()}`;
      const request = {
        batchId,
        originFarm: 'farm-a',
        storageTempC: 12,
        ethyleneExposure: 0.2,
        estimatedShelfLifeDays: 7,
      };

      const created = await tryCreateBatchViaNative(
          request, {nativePath: LOCAL_NATIVE_DIR});
      assert.notEqual(created, null);
      assert.equal(created.batchId, batchId);
      assert.equal(created.originFarm, request.originFarm);
      assert.equal(typeof created.exportStatus, 'string');

      const loaded = await tryGetBatchStatusViaNative(
          batchId, {nativePath: LOCAL_NATIVE_DIR});
      assert.notEqual(loaded, null);
      assert.equal(loaded.batchId, batchId);
      assert.equal(loaded.originFarm, request.originFarm);
    });

test(
    'batch native client maps native status errors for invalid and missing batches',
    {skip: !HAS_LOCAL_NATIVE_LIBRARY}, async () => {
      await assert.rejects(
          () => tryCreateBatchViaNative(
              {
                batchId: '',
                originFarm: 'farm-a',
                storageTempC: 12,
                ethyleneExposure: 0.2,
                estimatedShelfLifeDays: 7,
              },
              {nativePath: LOCAL_NATIVE_DIR}),
          (error) => {
            assert.equal(isNativeStatusError(error, 1), true);
            return true;
          });

      await assert.rejects(
          () => tryGetBatchStatusViaNative(
              'native-client-batch-does-not-exist',
              {nativePath: LOCAL_NATIVE_DIR}),
          (error) => {
            assert.equal(isNativeStatusError(error, 6), true);
            return true;
          });
    });
