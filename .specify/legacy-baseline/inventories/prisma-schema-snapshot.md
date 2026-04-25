# Prisma Schema Snapshot (v1)

Source: `src/typescript/api/prisma/schema.prisma`. Prisma 7.

## Datasource pattern

`schema.prisma` declares `datasource db { provider = "postgresql" }` with **no `url` field** (Prisma 7 lints reject inline `url`). The URL is supplied via `prisma.config.ts`:

```ts
import { defineConfig } from "prisma/config";
export default defineConfig({
  datasource: { url: env("DATABASE_URL") },
});
```

Migration status command (reliable from `src/typescript/api/`):
`bunx prisma migrate status --schema prisma/schema.prisma`.

## Generator

```
generator client { provider = "prisma-client-js" }
```

## Models (7)

All `*ApiCall` models share the audit-log shape:

```
model <Domain>ApiCall {
    id           BigInt   @id @default(autoincrement())
    route        String
    statusCode   Int
    requestJson  String
    responseJson String
    createdAt    DateTime @default(now())
}
```

Domains: `BananaApiCall`, `BatchApiCall`, `RipenessApiCall`, `MlApiCall`, `NotBananaApiCall`.

Domain-specific models:

- `BananaCalculation` — purchases/multiplier/banana/message/source/createdAt.
- `NotBananaClassification` — classification, bananaProbability, notBananaProbability, junkConfidence, actorCount, entityCount, requestJson, responseJson, source, createdAt.

## v2 Constraints (spec 008)

- Per-route ownership table in `008/contracts/route-ownership.md` MUST list which Fastify route persists into which model.
- The 5 `*ApiCall` audit tables collapse trivially to a single polymorphic `ApiCall { domain: String, ... }` model in v2 — call it out in `008/spec.md` if pursued.
- Bun is the package manager. `prisma.config.ts` (NOT inline `url`) is the canonical pattern.
