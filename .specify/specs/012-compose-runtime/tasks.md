---
description: "Tasks for v2 compose runtime regeneration"
---

# Tasks: Compose Runtime (v2)

## Phase 0 — Script inventory

- [ ] T001 List every `scripts/*.sh` with role: canonical channel | helper | deprecated.
- [ ] T002 Record canonical channel mapping in `contracts/channel-scripts.md`.

## Phase 1 — Catalog

- [ ] T010 Quickstart documents one entry per channel.
- [ ] T011 [P] Cross-reference from `010` and `011` quickstarts.

## Phase 2 — Build hygiene

- [ ] T020 Verify all Dockerfiles copying scripts run `sed -i 's/\r$//'`.
- [ ] T021 Verify `.gitattributes` includes `*.sh text eol=lf`.
- [ ] T022 Verify `.dockerignore` excludes `**/node_modules`.

## Phase 3 — Base image risk

- [ ] T030 Pin all FROM by digest where feasible.
- [ ] T031 Record residual base CVEs as platform risk in `research.md`.

## Phase 4 — API Dockerfile consolidation

- [ ] T040 Wait for `008` ownership decision.
- [ ] T041 If only one API canonical, retire the other Dockerfile and compose service.
