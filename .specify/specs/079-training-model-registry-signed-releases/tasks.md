# Tasks — 079 Model Registry + Signed Releases

- [x] T001 — Add ModelRelease and ActiveModelRelease Prisma models with sha256 hash + signature fields
- [x] T002 — Implement registry routes in src/routes/registry.ts
- [x] T003 — POST /registry/releases — create a new signed release with semver + artifact hash
- [x] T004 — GET /registry/releases — list all releases per model name
- [x] T005 — GET /registry/releases/:id — get a single release
- [x] T006 — POST /registry/releases/:id/promote — set as active release with signature verification
- [x] T007 — GET /registry/active — list all current active releases
- [x] T008 — Wire registryRoutes into index.ts
- [x] T009 — Validate artifactHash as SHA256 hex and signature as non-empty in create schema
- [x] T010 — Add activeModelReleases relation to ModelRelease model
