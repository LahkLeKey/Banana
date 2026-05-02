// prisma.config.ts (Prisma 7 — replaces inline `datasource.url` in
// schema.prisma). Spec 008 contract.
import { defineConfig } from "prisma";

export default defineConfig({
  // @ts-expect-error — `env()` resolved by Prisma at build time.
  datasource: { url: env("DATABASE_URL") },
  schema: "prisma/schema.prisma",
});
