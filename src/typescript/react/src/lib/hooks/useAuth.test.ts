/**
 * useAuth.test.ts — unit tests for useAuth store (feature 063).
 */
import { beforeEach, describe, expect, it } from "bun:test";
import { useAuth } from "./useAuth";

// Encode a minimal JWT with given payload (no signature validation needed for client tests)
function makeToken(payload: object, expOffsetSec = 3600): string {
  const header = btoa(JSON.stringify({ alg: "HS256", typ: "JWT" }));
  const body = btoa(
    JSON.stringify({ ...payload, exp: Math.floor(Date.now() / 1000) + expOffsetSec })
  );
  return `${header}.${body}.fakesig`;
}

describe("useAuth", () => {
  beforeEach(() => {
    useAuth.getState().clearToken();
  });

  it("sets token and parses claims", () => {
    const token = makeToken({ sub: "alice", role: "operator" });
    useAuth.getState().setToken(token);
    const state = useAuth.getState();
    expect(state.token).toBe(token);
    expect(state.sub).toBe("alice");
    expect(state.role).toBe("operator");
  });

  it("rejects expired tokens", () => {
    const token = makeToken({ sub: "bob", role: "admin" }, -10); // expired 10s ago
    useAuth.getState().setToken(token);
    expect(useAuth.getState().token).toBeNull();
  });

  it("clearToken resets all fields", () => {
    const token = makeToken({ sub: "carol", role: "viewer" });
    useAuth.getState().setToken(token);
    useAuth.getState().clearToken();
    const state = useAuth.getState();
    expect(state.token).toBeNull();
    expect(state.sub).toBeNull();
    expect(state.role).toBeNull();
  });
});
