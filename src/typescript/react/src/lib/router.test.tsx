/**
 * router.test.tsx — route tree smoke tests (feature 052).
 */
import { describe, expect, it } from "bun:test";

describe("router", () => {
  it("exports a router object", async () => {
    const mod = await import("./router");
    expect(mod.router).toBeDefined();
    expect(typeof mod.router.navigate).toBe("function");
  });

  it("router has canonical routes defined", async () => {
    const mod = await import("./router");
    expect(mod.router.routes).toBeDefined();
    expect(Array.isArray(mod.router.routes)).toBe(true);
    expect(mod.router.routes.length).toBeGreaterThan(0);

    const routePaths = mod.router.routes.map((route: { path?: string }) => route.path).filter((path: string | undefined): path is string => Boolean(path));

    expect(routePaths).toContain("/");
    expect(routePaths).toContain("/characters");
    expect(routePaths).toContain("/world-map");
    expect(routePaths).toContain("/legacy");
    expect(routePaths).toContain("/notebooks");
    expect(routePaths).toContain("/banana-engine");
    expect(routePaths).toContain("/session-room");
    expect(routePaths).toContain("/login");
    expect(routePaths).toContain("/profile");
  });
});
