import {createRequire} from 'node:module';
import path from 'node:path';

import type {BananaRequest, BananaResponse} from '../../contracts/http.js';

type NativeBridgeContext = {
  lib: {
    banana_calculate_banana_with_breakdown: (
        purchases: number, multiplier: number, outBreakdown: Buffer) => number;
    banana_create_banana_message:
        (purchases: number, multiplier: number, outMessagePtr: unknown) =>
            number;
    banana_db_query_banana_profile:
        (purchases: number, multiplier: number, outPayloadPtr: unknown,
         outRowCount: unknown) => number;
    banana_free: (pointer: unknown) => void;
  };
  ref: {
    types: {int: unknown; char: unknown}; alloc: (type: unknown) => {
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

let nativeBridgePromise: Promise<NativeBridgeContext|null>|null = null;
let nativeBridgePath: string|null = null;
const nodeRequire = createRequire(import.meta.url);

function assertInt32(name: string, value: number): void {
  if (!Number.isInteger(value)) {
    throw new TypeError(`${name} must be an integer`);
  }

  if (value < -2147483648 || value > 2147483647) {
    throw new RangeError(`${name} must fit in int32`);
  }
}

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

async function loadNativeBridge(nativePath?: string):
    Promise<NativeBridgeContext|null> {
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
      const ref = nodeRequire('ref-napi') as NativeBridgeContext['ref'];
      const intType = ref.types.int;
      const intPtr = ref.refType(intType);

      const lib = ffi.Library(libraryPath, {
        banana_calculate_banana_with_breakdown:
            ['int', ['int', 'int', 'pointer']],
        banana_create_banana_message: ['int', ['int', 'int', 'pointer']],
        banana_db_query_banana_profile:
            ['int', ['int', 'int', 'pointer', intPtr]],
        banana_free: ['void', ['pointer']],
      }) as NativeBridgeContext['lib'];

      return {lib, ref};
    } catch {
      return null;
    }
  })();

  return nativeBridgePromise;
}

function readNativeString(
    context: NativeBridgeContext, pointerRef: {deref: () => unknown}): string {
  const pointer = pointerRef.deref();
  if (context.ref.isNull(pointer)) {
    return '';
  }

  const value = context.ref.readCString(pointer, 0);
  context.lib.banana_free(pointer);
  return value;
}

export type NativeBananaCalculationResult = {
  response: BananaResponse;
  metadata: {dbContract?: string; dbSource?: string; dbRowCount?: string;};
};

export async function tryCalculateBananaViaNative(
    request: BananaRequest,
    options: {nativePath?: string; includeDbQuery?: boolean} = {}):
    Promise<NativeBananaCalculationResult|null> {
  const context = await loadNativeBridge(options.nativePath);
  if (!context) {
    return null;
  }

  assertInt32('purchases', request.purchases);
  assertInt32('multiplier', request.multiplier);

  const breakdownBuffer = Buffer.alloc(12);
  const breakdownStatus = context.lib.banana_calculate_banana_with_breakdown(
      request.purchases, request.multiplier, breakdownBuffer);
  if (breakdownStatus !== STATUS.OK) {
    throw toStatusError(breakdownStatus);
  }

  const messagePtr =
      context.ref.alloc(context.ref.refType(context.ref.types.char));
  const messageStatus = context.lib.banana_create_banana_message(
      request.purchases, request.multiplier, messagePtr);
  if (messageStatus !== STATUS.OK) {
    throw toStatusError(messageStatus);
  }

  const response: BananaResponse = {
    purchases: breakdownBuffer.readInt32LE(0),
    multiplier: breakdownBuffer.readInt32LE(4),
    banana: breakdownBuffer.readInt32LE(8),
    message: readNativeString(context, messagePtr),
  };

  const metadata: NativeBananaCalculationResult['metadata'] = {};

  if (options.includeDbQuery ?? true) {
    const payloadPtr =
        context.ref.alloc(context.ref.refType(context.ref.types.char));
    const rowCount = context.ref.alloc(context.ref.types.int);
    const dbStatus = context.lib.banana_db_query_banana_profile(
        request.purchases, request.multiplier, payloadPtr, rowCount);

    if (dbStatus === STATUS.OK) {
      readNativeString(context, payloadPtr);
      metadata.dbContract = 'BananaProfileProjection';
      metadata.dbSource = 'native-fastify';
      metadata.dbRowCount = String(rowCount.deref());
    }
  }

  return {response, metadata};
}
