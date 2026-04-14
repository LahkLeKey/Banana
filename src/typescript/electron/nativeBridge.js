const path = require("path");
const ffi = require("ffi-napi");
const ref = require("ref-napi");

const intType = ref.types.int;
const intPtr = ref.refType(intType);
const charPtr = ref.refType(ref.types.char);

const STATUS = {
  OK: 0,
  INVALID_ARGUMENT: 1,
  OVERFLOW: 2,
  INTERNAL_ERROR: 3,
  DB_ERROR: 4,
  DB_NOT_CONFIGURED: 5,
};

const STATUS_MESSAGE = {
  [STATUS.INVALID_ARGUMENT]: "invalid argument",
  [STATUS.OVERFLOW]: "overflow",
  [STATUS.INTERNAL_ERROR]: "internal error",
  [STATUS.DB_ERROR]: "database error",
  [STATUS.DB_NOT_CONFIGURED]: "database not configured",
};

const libraryDir = process.env.BANANA_ENV_NATIVE_PATH
  || process.env.BANANA_NATIVE_PATH
  || path.resolve(__dirname, "../../../build/native/bin/Release");
const libraryPath = process.platform === "win32"
  ? path.join(libraryDir, "banana_native.dll")
  : process.platform === "darwin"
  ? path.join(libraryDir, "libbanana_native.dylib")
  : path.join(libraryDir, "libbanana_native.so");

const native = ffi.Library(libraryPath, {
  banana_calculate_banana: ["int", ["int", "int", intPtr]],
  banana_calculate_banana_with_breakdown: ["int", ["int", "int", "pointer"]],
  banana_create_banana_message: ["int", ["int", "int", "pointer"]],
  banana_db_query_banana: ["int", ["int", "int", "pointer", intPtr]],
  banana_free: ["void", ["pointer"]],
});

function assertInt32(name, value) {
  if (!Number.isInteger(value)) {
    throw new TypeError(`${name} must be an integer`);
  }
  if (value < -2147483648 || value > 2147483647) {
    throw new RangeError(`${name} must fit in int32`);
  }
}

function toStatusError(status) {
  const text = STATUS_MESSAGE[status] || "unknown native error";
  const error = new Error(`native status=${status} (${text})`);
  error.status = status;
  return error;
}

function calculateBanana(purchases, multiplier) {
  assertInt32("purchases", purchases);
  assertInt32("multiplier", multiplier);

  const out = ref.alloc(intType);
  const status = native.banana_calculate_banana(purchases, multiplier, out);
  if (status !== STATUS.OK) {
    throw toStatusError(status);
  }

  return out.deref();
}

function calculateBananaWithBreakdown(purchases, multiplier) {
  assertInt32("purchases", purchases);
  assertInt32("multiplier", multiplier);

  const out = Buffer.alloc(12);
  const status = native.banana_calculate_banana_with_breakdown(purchases, multiplier, out);
  if (status !== STATUS.OK) {
    throw toStatusError(status);
  }

  return {
    purchases: out.readInt32LE(0),
    multiplier: out.readInt32LE(4),
    banana: out.readInt32LE(8),
  };
}

function readNativeString(pointerRef) {
  const pointer = pointerRef.deref();
  if (ref.isNull(pointer)) {
    return "";
  }

  const value = ref.readCString(pointer, 0);
  native.banana_free(pointer);
  return value;
}

function createBananaMessage(purchases, multiplier) {
  assertInt32("purchases", purchases);
  assertInt32("multiplier", multiplier);

  const out = ref.alloc(charPtr);
  const status = native.banana_create_banana_message(purchases, multiplier, out);
  if (status !== STATUS.OK) {
    throw toStatusError(status);
  }

  return readNativeString(out);
}

function queryBananaDatabase(purchases, multiplier) {
  assertInt32("purchases", purchases);
  assertInt32("multiplier", multiplier);

  const payloadPtr = ref.alloc(charPtr);
  const rowCount = ref.alloc(intType);
  const status = native.banana_db_query_banana(purchases, multiplier, payloadPtr, rowCount);
  if (status !== STATUS.OK) {
    throw toStatusError(status);
  }

  const payloadText = readNativeString(payloadPtr);
  return {
    payload: payloadText,
    rowCount: rowCount.deref(),
  };
}

function runNativeScenario(purchases, multiplier, options = {}) {
  const result = {
    calculation: calculateBanana(purchases, multiplier),
    breakdown: calculateBananaWithBreakdown(purchases, multiplier),
    message: createBananaMessage(purchases, multiplier),
  };

  if (options.includeDatabase) {
    result.database = queryBananaDatabase(purchases, multiplier);
  }

  return result;
}

module.exports = {
  calculateBanana,
  calculateBananaWithBreakdown,
  createBananaMessage,
  libraryPath,
  queryBananaDatabase,
  runNativeScenario,
  STATUS,
};
