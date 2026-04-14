const path = require("path");
const ffi = require("ffi-napi");
const ref = require("ref-napi");

const intType = ref.types.int;
const intPtr = ref.refType(intType);

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
});

function calculateBanana(purchases, multiplier) {
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

module.exports = {
  calculateBanana,
  libraryPath,
  STATUS,
};
