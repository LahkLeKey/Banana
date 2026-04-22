import {createRequire} from 'node:module';
import path from 'node:path';

import type {BananaBatchCreateRequest, BananaBatchResponse} from '../../contracts/http.js';

type NativeBatchBridge = {
  lib: {
    banana_create_batch: (
        batchId: string, originFarm: string, storageTempC: number,
        ethyleneExposure: number, estimatedShelfLifeDays: number,
        outPayloadPtr: unknown) => number;
    banana_get_batch_status: (batchId: string, outPayloadPtr: unknown) =>
        number;
    banana_free: (pointer: unknown) => void;
  };
  ref: {
    types: {char: unknown}; alloc: (type: unknown) => {
      deref: () => unknown
    };
    refType: (type: unknown) => unknown;
    isNull: (pointer: unknown) => boolean;
    readCString: (pointer: unknown, offset: number) => string;
  };
};

type NativeStatus = 0|1|2|3|4|5|6;

const STATUS = {
  OK: 0,
  INVALID_ARGUMENT: 1,
  OVERFLOW: 2,
  INTERNAL_ERROR: 3,
  DB_ERROR: 4,
  DB_NOT_CONFIGURED: 5,
  NOT_FOUND: 6,
} as const satisfies Record<string, NativeStatus>;

const STATUS_MESSAGE: Record<NativeStatus, string> = {
  [STATUS.OK]: 'ok',
  [STATUS.INVALID_ARGUMENT]: 'invalid argument',
  [STATUS.OVERFLOW]: 'overflow',
  [STATUS.INTERNAL_ERROR]: 'internal error',
  [STATUS.DB_ERROR]: 'database error',
  [STATUS.DB_NOT_CONFIGURED]: 'database not configured',
  [STATUS.NOT_FOUND]: 'not found',
};

const nodeRequire = createRequire(import.meta.url);
let nativeBridgePromise: Promise<NativeBatchBridge|null>|null = null;
let nativeBridgePath: string|null = null;

function resolveLibraryPath(nativePath?: string): string {
  const dir = nativePath || process.env.BANANA_NATIVE_PATH ||
      path.resolve(process.cwd(), '../../../build/native/bin/Release');

  if (process.platform === 'win32') {
    return path.join(dir, 'banana_native.dll');
  }

  if (process.platform === 'darwin') {
    return path.join(dir, 'libbanana_native.dylib');
  }

  return path.join(dir, 'libbanana_native.so');
}

function toStatusError(status: number): Error {
  const nativeStatus =
      (status in STATUS_MESSAGE ? status : STATUS.INTERNAL_ERROR) as
      NativeStatus;
  const message = STATUS_MESSAGE[nativeStatus];
  const error = new Error(`native status=${status} (${message})`);
  (error as Error & {status?: number}).status = status;
  return error;
}

function readNativeString(
    context: NativeBatchBridge, pointerRef: {deref: () => unknown}): string {
  const pointer = pointerRef.deref();
  if (context.ref.isNull(pointer)) {
    return '';
  }

  const value = context.ref.readCString(pointer, 0);
  context.lib.banana_free(pointer);
  return value;
}

function parseBatchPayload(payloadText: string): BananaBatchResponse {
  let payload: unknown;
  try {
    payload = JSON.parse(payloadText);
  } catch {
    throw new Error('native returned invalid batch JSON payload');
  }

  const record = payload as Record<string, unknown>;
  if (typeof record.batchId !== 'string' ||
      typeof record.originFarm !== 'string' ||
      typeof record.exportStatus !== 'string' ||
      typeof record.storageTempC !== 'number' ||
      typeof record.ethyleneExposure !== 'number' ||
      typeof record.estimatedShelfLifeDays !== 'number') {
    throw new Error('native returned batch payload with unexpected shape');
  }

  return {
    batchId: record.batchId,
    originFarm: record.originFarm,
    exportStatus: record.exportStatus,
    storageTempC: record.storageTempC,
    ethyleneExposure: record.ethyleneExposure,
    estimatedShelfLifeDays: record.estimatedShelfLifeDays,
  };
}

async function loadNativeBridge(nativePath?: string):
    Promise<NativeBatchBridge|null> {
  const libraryPath = resolveLibraryPath(nativePath);

  if (nativeBridgePromise && nativeBridgePath === libraryPath) {
    const cachedContext = await nativeBridgePromise;
    if (cachedContext) {
      return cachedContext;
    }

    nativeBridgePromise = null;
  }

  nativeBridgePath = libraryPath;
  nativeBridgePromise = (async () => {
    try {
      const ffi = nodeRequire('ffi-napi') as {
        Library: (libraryPath: string, signatures: Record<string, unknown>) =>
            unknown
      };
      const ref = nodeRequire('ref-napi') as NativeBatchBridge['ref'];

      const lib = ffi.Library(libraryPath, {
        banana_create_batch:
            ['int', ['string', 'string', 'double', 'double', 'int', 'pointer']],
        banana_get_batch_status: ['int', ['string', 'pointer']],
        banana_free: ['void', ['pointer']],
      }) as NativeBatchBridge['lib'];

      return {lib, ref};
    } catch {
      return null;
    }
  })();

  return nativeBridgePromise;
}

function assertEstimatedShelfLifeDays(value: number): void {
  if (!Number.isInteger(value)) {
    throw new TypeError('estimatedShelfLifeDays must be an integer');
  }

  if (value < -2147483648 || value > 2147483647) {
    throw new RangeError('estimatedShelfLifeDays must fit in int32');
  }
}

export async function tryCreateBatchViaNative(
    request: BananaBatchCreateRequest,
    options: {nativePath?: string} = {}): Promise<BananaBatchResponse|null> {
  assertEstimatedShelfLifeDays(request.estimatedShelfLifeDays);

  const context = await loadNativeBridge(options.nativePath);
  if (!context) {
    return null;
  }

  const payloadPtr =
      context.ref.alloc(context.ref.refType(context.ref.types.char));
  const status = context.lib.banana_create_batch(
      request.batchId, request.originFarm, request.storageTempC,
      request.ethyleneExposure, request.estimatedShelfLifeDays, payloadPtr);
  if (status !== STATUS.OK) {
    throw toStatusError(status);
  }

  const payloadText = readNativeString(context, payloadPtr);
  return parseBatchPayload(payloadText);
}

export async function tryGetBatchStatusViaNative(
    batchId: string,
    options: {nativePath?: string} = {}): Promise<BananaBatchResponse|null> {
  const context = await loadNativeBridge(options.nativePath);
  if (!context) {
    return null;
  }

  const payloadPtr =
      context.ref.alloc(context.ref.refType(context.ref.types.char));
  const status = context.lib.banana_get_batch_status(batchId, payloadPtr);
  if (status !== STATUS.OK) {
    throw toStatusError(status);
  }

  const payloadText = readNativeString(context, payloadPtr);
  return parseBatchPayload(payloadText);
}
