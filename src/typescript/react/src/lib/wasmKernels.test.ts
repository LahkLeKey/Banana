import { describe, expect, test } from "bun:test";

import {
  type BinaryClassifyResult,
  classifyBananaBinary,
  predictRipeness,
  type RipenessResult,
} from "./wasmKernels";
import type { BananaWasmModule } from "./wasmTypes";

function createJsonModule(binaryJson: string, ripenessJson: string): BananaWasmModule {
  let nextPtr = 1024;
  const numericMemory = new Map<number, number>();
  const stringMemory = new Map<number, string>();

  const alloc = (size: number): number => {
    const ptr = nextPtr;
    nextPtr += Math.max(size, 4);
    return ptr;
  };

  return {
    _banana_calculate_banana: () => 0,
    _banana_calculate_banana_with_breakdown: () => 0,
    _banana_predict_banana_regression_score: () => 0,
    _banana_classify_banana_binary: (_inputJsonPtr, outJsonPtrPtr) => {
      const outJsonPtr = alloc(binaryJson.length + 1);
      stringMemory.set(outJsonPtr, binaryJson);
      numericMemory.set(outJsonPtrPtr, outJsonPtr);
      return 0;
    },
    _banana_classify_banana_binary_with_threshold: (_inputJsonPtr, _threshold, outJsonPtrPtr) => {
      const outJsonPtr = alloc(binaryJson.length + 1);
      stringMemory.set(outJsonPtr, binaryJson);
      numericMemory.set(outJsonPtrPtr, outJsonPtr);
      return 0;
    },
    _banana_predict_banana_ripeness: (_inputJsonPtr, outJsonPtrPtr) => {
      const outJsonPtr = alloc(ripenessJson.length + 1);
      stringMemory.set(outJsonPtr, ripenessJson);
      numericMemory.set(outJsonPtrPtr, outJsonPtr);
      return 0;
    },
    _banana_free: (ptr) => {
      stringMemory.delete(ptr);
    },
    _malloc: alloc,
    _free: () => {},
    UTF8ToString: (ptr) => stringMemory.get(ptr) ?? "",
    stringToUTF8: (str, outPtr) => {
      stringMemory.set(outPtr, str);
    },
    lengthBytesUTF8: (str) => str.length,
    setValue: (ptr, value) => {
      numericMemory.set(ptr, value);
    },
    getValue: (ptr) => numericMemory.get(ptr) ?? 0,
  } as BananaWasmModule;
}

describe("wasmKernels JSON safety", () => {
  test("classifyBananaBinary returns structured error on invalid JSON", () => {
    const mod = createJsonModule("{bad_json", '{"label":"ripe","confidence":0.9,"model":"x"}');
    const result = classifyBananaBinary(mod, '{"text":"banana"}');

    expect(result.ok).toBe(false);
    if (!result.ok) {
      expect(result.code).toBe(3);
      expect(result.message).toContain("invalid_json_output");
    }
  });

  test("predictRipeness returns parsed data for valid JSON", () => {
    const expected: RipenessResult = { label: "ripe", confidence: 0.91, model: "ripeness-v1" };
    const mod = createJsonModule(
      '{"label":"banana","confidence":0.8,"banana_score":0.8,"not_banana_score":0.2}',
      JSON.stringify(expected)
    );

    const result = predictRipeness(mod, '{"text":"yellow banana"}');
    expect(result.ok).toBe(true);
    if (result.ok) {
      expect(result.data).toEqual(expected);
    }
  });

  test("classifyBananaBinary parses valid JSON", () => {
    const expected: BinaryClassifyResult = {
      label: "banana",
      confidence: 0.82,
      banana_score: 0.82,
      not_banana_score: 0.18,
    };
    const mod = createJsonModule(
      JSON.stringify(expected),
      '{"label":"ripe","confidence":0.9,"model":"x"}'
    );

    const result = classifyBananaBinary(mod, '{"text":"banana"}');
    expect(result.ok).toBe(true);
    if (result.ok) {
      expect(result.data).toEqual(expected);
    }
  });
});
