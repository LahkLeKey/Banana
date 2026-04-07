const path = require("path");
const ffi = require("ffi-napi");
const ref = require("ref-napi");

const intType = ref.types.int;
const intPtr = ref.refType(intType);

const breakdownStruct = {
  purchases: intType,
  multiplier: intType,
  points: intType,
};

const libraryDir = process.env.CINTEROP_NATIVE_PATH || path.resolve(__dirname, "../../build/native/bin/Release");
const libraryPath = process.platform === "win32"
  ? path.join(libraryDir, "cinterop_native.dll")
  : process.platform === "darwin"
  ? path.join(libraryDir, "libcinterop_native.dylib")
  : path.join(libraryDir, "libcinterop_native.so");

const native = ffi.Library(libraryPath, {
  cinterop_calculate_points: ["int", ["int", "int", intPtr]],
});

function calculatePoints(purchases, multiplier) {
  const out = ref.alloc(intType);
  const status = native.cinterop_calculate_points(purchases, multiplier, out);
  if (status !== 0) {
    throw new Error(`native status=${status}`);
  }
  return out.deref();
}

module.exports = {
  calculatePoints,
  breakdownStruct,
};
