// @ts-nocheck -- bun:test types are not part of app tsconfig.

import { describe, expect, test } from "bun:test";

import { computeMoveAxes, isMovementKey } from "../GameEnginePage";

describe("GameEnginePage controller input mapping", () => {
  test("recognizes WASD and Arrow keys as movement keys", () => {
    expect(isMovementKey("w")).toBe(true);
    expect(isMovementKey("A")).toBe(true);
    expect(isMovementKey("arrowleft")).toBe(true);
    expect(isMovementKey("ArrowUp")).toBe(true);
    expect(isMovementKey("enter")).toBe(false);
    expect(isMovementKey("escape")).toBe(false);
  });

  test("maps right movement keys to positive X", () => {
    const keys = new Set(["d"]);
    expect(computeMoveAxes(keys)).toEqual({ moveX: 1, moveZ: 0 });
  });

  test("maps left movement keys to negative X", () => {
    const keys = new Set(["arrowleft"]);
    expect(computeMoveAxes(keys)).toEqual({ moveX: -1, moveZ: 0 });
  });

  test("maps forward movement keys to positive Z", () => {
    const keys = new Set(["w"]);
    expect(computeMoveAxes(keys)).toEqual({ moveX: 0, moveZ: 1 });
  });

  test("maps backward movement keys to negative Z", () => {
    const keys = new Set(["s"]);
    expect(computeMoveAxes(keys)).toEqual({ moveX: 0, moveZ: -1 });
  });

  test("cancels opposite directions to zero", () => {
    const keys = new Set(["a", "d", "arrowup", "arrowdown"]);
    expect(computeMoveAxes(keys)).toEqual({ moveX: 0, moveZ: 0 });
  });
});
