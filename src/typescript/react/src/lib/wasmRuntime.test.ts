import { describe, expect, test } from "bun:test";

import { detectWasmSimd, getKernelVariant, WASM_INIT_TIMEOUT_MS } from "./wasmRuntime";
import { WASM_STATUS_MESSAGES, wasmStatusMessage } from "./wasmTypes";

describe("detectWasmSimd", () => {
  test("returns a boolean without throwing", () => {
    const result = detectWasmSimd();
    expect(typeof result).toBe("boolean");
  });

  test("result is cached — subsequent calls return same value", () => {
    const first = detectWasmSimd();
    const second = detectWasmSimd();
    expect(first).toBe(second);
  });
});

describe("getKernelVariant", () => {
  test("returns simd or scalar", () => {
    const variant = getKernelVariant();
    expect(["simd", "scalar"]).toContain(variant);
  });
});

describe("WASM_INIT_TIMEOUT_MS", () => {
  test("is a positive number", () => {
    expect(typeof WASM_INIT_TIMEOUT_MS).toBe("number");
    expect(WASM_INIT_TIMEOUT_MS).toBeGreaterThan(0);
  });
});

describe("wasmStatusMessage", () => {
  test("maps known codes", () => {
    expect(wasmStatusMessage(0)).toBe("ok");
    expect(wasmStatusMessage(1)).toBe("invalid_argument");
    expect(wasmStatusMessage(2)).toBe("overflow");
    expect(wasmStatusMessage(3)).toBe("internal_error");
    expect(wasmStatusMessage(7)).toBe("buffer_too_small");
  });

  test("returns unknown_N for unmapped codes", () => {
    expect(wasmStatusMessage(99)).toBe("unknown_99");
  });

  test("WASM_STATUS_MESSAGES has entries for codes 0-8", () => {
    for (let i = 0; i <= 8; i++) {
      expect(typeof WASM_STATUS_MESSAGES[i]).toBe("string");
    }
  });
});
