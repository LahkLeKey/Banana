/**
 * Feature 053 — typed state baseline tests.
 *
 * Verifies the QueryClient defaults so future migrations cannot silently
 * regress retry / staleTime behavior the resilience layer relies on.
 */
import { describe, expect, it } from "bun:test";
import { createBananaQueryClient } from "./queryClient";

describe("createBananaQueryClient (feature 053)", () => {
  it("applies the documented defaults", () => {
    const client = createBananaQueryClient();
    const defaults = client.getDefaultOptions();
    expect(defaults.queries?.staleTime).toBe(60_000);
    expect(defaults.queries?.gcTime).toBe(5 * 60_000);
    expect(defaults.queries?.retry).toBe(2);
    expect(defaults.queries?.refetchOnWindowFocus).toBe(false);
    expect(defaults.mutations?.retry).toBe(0);
  });
});
