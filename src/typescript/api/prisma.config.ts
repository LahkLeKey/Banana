// prisma.config.ts (Prisma 7 - replaces inline `datasource.url` in
// schema.prisma). Spec 008 contract.
import { defineConfig } from "@prisma/config";

export default defineConfig({
  datasource: {
    url: process.env.DATABASE_URL,
  },
  schema: "prisma/schema.prisma",
});
