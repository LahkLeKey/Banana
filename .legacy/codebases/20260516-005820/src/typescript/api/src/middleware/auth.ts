/**
 * auth.ts — JWT authentication middleware for Fastify (feature 063).
 *
 * Roles: admin | operator | viewer
 * Token source: Authorization: Bearer <token>
 * Signing algorithm: HS256, key from BANANA_JWT_SECRET env var.
 *
 * Routes opt in by calling `await request.jwtVerify()` or using the
 * `requireRole(role)` preHandler helper exported below.
 *
 * Non-authenticated routes (e.g. /health, /registry/releases GET) remain
 * open — the middleware only validates when explicitly invoked.
 */
import type { FastifyInstance, FastifyReply, FastifyRequest } from "fastify";
import * as jose from "jose";

export type BananaRole = "admin" | "operator" | "viewer";

export interface BananaJwtPayload {
  sub: string;
  role: BananaRole;
  iat?: number;
  exp?: number;
}

const ALG = "HS256";

function getSecret(): Uint8Array {
  const s = process.env.BANANA_JWT_SECRET;
  if (!s) throw new Error("BANANA_JWT_SECRET env var is not set");
  return new TextEncoder().encode(s);
}

/** Sign a token (used in tests / token issuance endpoint). */
export async function signToken(
  payload: Omit<BananaJwtPayload, "iat" | "exp">,
  expiresIn = "8h"
): Promise<string> {
  return new jose.SignJWT({ ...payload })
    .setProtectedHeader({ alg: ALG })
    .setIssuedAt()
    .setExpirationTime(expiresIn)
    .sign(getSecret());
}

/** Verify a raw Bearer token string. Throws on invalid / expired. */
export async function verifyToken(token: string): Promise<BananaJwtPayload> {
  const { payload } = await jose.jwtVerify(token, getSecret(), {
    algorithms: [ALG],
  });
  return payload as unknown as BananaJwtPayload;
}

/**
 * Fastify preHandler factory — verifies JWT and enforces minimum role.
 * Role hierarchy: admin > operator > viewer.
 */
const ROLE_WEIGHT: Record<BananaRole, number> = {
  admin: 30,
  operator: 20,
  viewer: 10,
};

export function requireRole(
  minRole: BananaRole
): (req: FastifyRequest, reply: FastifyReply) => Promise<void> {
  return async (req, reply) => {
    const authHeader = req.headers.authorization;
    if (!authHeader?.startsWith("Bearer ")) {
      return reply.status(401).send({ error: "Missing Bearer token" });
    }
    const token = authHeader.slice(7);
    let claims: BananaJwtPayload;
    try {
      claims = await verifyToken(token);
    } catch {
      return reply.status(401).send({ error: "Invalid or expired token" });
    }
    const weight = ROLE_WEIGHT[claims.role] ?? 0;
    if (weight < ROLE_WEIGHT[minRole]) {
      return reply.status(403).send({
        error: `Role '${claims.role}' does not meet required role '${minRole}'`,
      });
    }
    // Attach claims for downstream handlers
    (req as FastifyRequest & { jwtClaims: BananaJwtPayload }).jwtClaims = claims;
  };
}

/**
 * Register a token-issuance endpoint (dev/internal use only).
 * In production, gate with BANANA_AUTH_TOKEN_ENDPOINT_ENABLED=true.
 */
export async function registerAuthRoutes(app: FastifyInstance) {
  if (process.env.BANANA_AUTH_TOKEN_ENDPOINT_ENABLED !== "true") return;

  app.post("/auth/token", async (req, reply) => {
    const { sub, role } = req.body as { sub?: string; role?: string };
    if (!sub || !role || !["admin", "operator", "viewer"].includes(role)) {
      return reply.status(400).send({ error: "sub and valid role required" });
    }
    const token = await signToken({ sub, role: role as BananaRole });
    return reply.send({ token });
  });
}
