const path = require("path");
const ffi = require("ffi-napi");
const ref = require("ref-napi");

const intType = ref.types.int;
const intPtr = ref.refType(intType);

const breakdownStruct = {
  purchases: intType,
  multiplier: intType,
  banana: intType,
};

const libraryDir = process.env.BANANA_NATIVE_PATH || path.resolve(__dirname, "../../build/native/bin/Release");
const libraryPath = process.platform === "win32"
  ? path.join(libraryDir, "banana_native.dll")
  : process.platform === "darwin"
  ? path.join(libraryDir, "libbanana_native.dylib")
  : path.join(libraryDir, "libbanana_native.so");

const native = ffi.Library(libraryPath, {
  banana_calculate_banana: ["int", ["int", "int", intPtr]],
});

function calculateBanana(purchases, multiplier) {
  const out = ref.alloc(intType);
  const status = native.banana_calculate_banana(purchases, multiplier, out);
  if (status !== 0) {
    throw new Error(`native status=${status}`);
  }
  return out.deref();
}

module.exports = {
  calculateBanana,
  breakdownStruct,
};
