/**
 * vitals.test.ts — Core Web Vitals reporter (feature 061).
 */
import { beforeEach, describe, expect, it, mock } from "bun:test";

describe("initVitals", () => {
  beforeEach(() => {
    // Reset module between tests
    mock.restore();
  });

  it("exports initVitals as a function", async () => {
    const mod = await import("./vitals");
    expect(typeof mod.initVitals).toBe("function");
  });

  it("does not throw when called in a non-browser environment", async () => {
    const mod = await import("./vitals");
    // In the Bun test environment window.location is not a real browser context;
    // initVitals should not throw regardless.
    expect(() => mod.initVitals()).not.toThrow();
  });
});
