/**
 * auth.test.ts — unit tests for JWT middleware (feature 063).
 */
import { beforeEach, describe, expect, it } from "bun:test";
import type { BananaJwtPayload } from "../middleware/auth.ts";
import { requireRole, signToken, verifyToken } from "../middleware/auth.ts";

// Set secret for all tests
process.env.BANANA_JWT_SECRET = "test-secret-banana-jwt-1234";

describe("signToken / verifyToken", () => {
  it("round-trips a valid token", async () => {
    const token = await signToken({ sub: "user1", role: "operator" });
    const claims = await verifyToken(token);
    expect(claims.sub).toBe("user1");
    expect(claims.role).toBe("operator");
  });

  it("rejects a tampered token", async () => {
    const token = await signToken({ sub: "user1", role: "admin" });
    const parts = token.split(".");
    const tampered = parts[0] + "." + parts[1] + ".badsignature";
    await expect(verifyToken(tampered)).rejects.toThrow();
  });
});

describe("requireRole preHandler", () => {
  function makeReqReply(token?: string) {
    const sent: unknown[] = [];
    const statusCodes: number[] = [];
    const req = {
      headers: token ? { authorization: `Bearer ${token}` } : {},
    } as Parameters<ReturnType<typeof requireRole>>[0];
    const reply = {
      status(code: number) {
        statusCodes.push(code);
        return this;
      },
      send(body: unknown) {
        sent.push(body);
        return this;
      },
    } as Parameters<ReturnType<typeof requireRole>>[1];
    return { req, reply, sent, statusCodes };
  }

  it("allows a sufficient role", async () => {
    const token = await signToken({ sub: "op", role: "operator" });
    const { req, reply, statusCodes } = makeReqReply(token);
    const handler = requireRole("viewer");
    await handler(req, reply);
    expect(statusCodes).toHaveLength(0); // no rejection
  });

  it("rejects missing token with 401", async () => {
    const { req, reply, statusCodes } = makeReqReply();
    const handler = requireRole("viewer");
    await handler(req, reply);
    expect(statusCodes[0]).toBe(401);
  });

  it("rejects insufficient role with 403", async () => {
    const token = await signToken({ sub: "viewer1", role: "viewer" });
    const { req, reply, statusCodes } = makeReqReply(token);
    const handler = requireRole("admin");
    await handler(req, reply);
    expect(statusCodes[0]).toBe(403);
  });
});
