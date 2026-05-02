<!-- breadcrumb: Security > Threat Model -->

# Threat Model — Banana API

> [Home](../Home.md) › [Security](README.md) › Threat Model

> **Status:** Living document — update when new attack surfaces are added or mitigated.
> **Owner:** Security-focused sprint (#098)

---

## Scope

This document covers the Banana API attack surface as of Sprint 2:

| System | Surface |
|--------|---------|
| ASP.NET API (port 5000) | REST endpoints, CORS, JWT auth, native interop |
| Fastify TS API (port 8081) | REST endpoints, CORS, JWT auth, Prisma/Postgres |
| React SPA | XSS, CSP, CSRF, token storage |
| Native C library | Memory safety, integer overflow, format strings |
| Docker Compose stack | Container escape, network exposure, secret management |

---

## STRIDE Analysis

### Spoofing
- **Threat:** Forged JWT tokens impersonating admin/operator users.
- **Mitigation:** HS256 key from `BANANA_JWT_SECRET` (≥32 byte random); rotate via env update; short expiry (8h default). See `src/middleware/auth.ts`, `Program.cs`.
- **Residual risk:** Symmetric key; for multi-service federation consider RS256. Tracked as deferred (#063 phase 2).

### Tampering
- **Threat:** Request body manipulation to alter classification results or inject SQL.
- **Mitigation:** Zod schema validation on all Fastify routes; ASP.NET model binding + `[ApiController]` validation; Prisma parameterized queries (no raw SQL in hot paths).
- **Residual risk:** Native C `sprintf` use — see sanitizer findings from #073 CI lane.

### Repudiation
- **Threat:** Operator denies promoting a malicious model release.
- **Mitigation:** `ModelRelease.promotedBy` + `createdAt` immutable append; HMAC signature over `manifestJson`. Audit log route (#069 deferred).

### Information Disclosure
- **Threat:** Stack traces or internal error details leaked in API responses.
- **Mitigation:** ASP.NET: no `UseDeveloperExceptionPage` in production; Fastify: `logger: true` writes to stdout only, not response body.
- **Residual risk:** Fastify error serialization includes `message` field — review for sensitive path leaks.

### Denial of Service
- **Threat:** Unbounded classification batches consuming native memory; large corpus uploads.
- **Mitigation:** Batch size enforced in `InputValidationStep`; `Content-Length` checks in ASP.NET; rate limiting deferred (#068).
- **Residual risk:** No rate limiting yet — tracked as high priority (#068 Sprint 3).

### Elevation of Privilege
- **Threat:** `viewer` role accessing `admin`-only endpoints.
- **Mitigation:** `requireRole()` preHandler (Fastify), `[Authorize(Policy="AdminOnly")]` (ASP.NET). `ProtectedRoute` in React.
- **Residual risk:** Routes not yet decorated with role guards — must audit before production (#063 follow-on).

---

## Data Flow Diagram

```
Browser (React SPA)
  │  HTTPS + JWT Bearer
  ▼
ASP.NET API  ──── BANANA_NATIVE_PATH ────► libbanana_native.so (C)
  │
  │  internal REST
  ▼
Fastify TS API ──── Prisma ────► PostgreSQL
  │
  └─── pg-boss jobs (model warmup, corpus rebuild)
```

---

## Secret Inventory

| Secret | Location | Rotation |
|--------|----------|----------|
| `BANANA_JWT_SECRET` | Env / Docker secret | On breach or quarterly |
| `BANANA_PG_CONNECTION` / `DATABASE_URL` | Env / Docker secret | On breach |
| `MODEL_SIGNING_KEY` | Env (feature 079) | On breach |
| GitHub Actions PAT | GH encrypted secret | On breach |

---

## Open Risks (tracked)

| Risk | Spec | Priority |
|------|------|----------|
| No rate limiting | #068 | Sprint 3 |
| No audit log | #069 | Sprint 3 |
| Vault/secret rotation automation | #082 | Sprint 3 |
| Formal pentest | #099 | Deferred |
| PII data handling | #096 | Deferred |
