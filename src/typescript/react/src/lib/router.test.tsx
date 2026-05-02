/**
 * router.test.tsx — route tree smoke tests (feature 052).
 */
import { describe, it, expect } from "bun:test";

describe("router", () => {
  it("exports a router object", async () => {
    const mod = await import("./router");
    expect(mod.router).toBeDefined();
    expect(typeof mod.router.navigate).toBe("function");
  });

  it("router has routes defined", async () => {
    const mod = await import("./router");
    // createBrowserRouter stores routes internally; verify the object shape
    expect(mod.router.routes).toBeDefined();
    expect(Array.isArray(mod.router.routes)).toBe(true);
    expect(mod.router.routes.length).toBeGreaterThan(0);
  });
});
