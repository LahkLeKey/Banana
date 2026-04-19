---
name: banana-discovery
description: Discover Banana architecture, touched domains, runtime contracts, and validation entry points before planning or implementation.
argument-hint: Describe the request, suspected area, or files you want mapped.
---

# Banana Discovery

Use this skill when you need to map a request onto Banana's actual architecture before planning or coding.

## What To Do

1. Start from the repo-wide guidance in [../../copilot-instructions.md](../../copilot-instructions.md).
2. Read the architecture summary in [../../../docs/developer-onboarding.md](../../../docs/developer-onboarding.md).
3. Identify which of Banana's four domains are touched: native C, ASP.NET, React/Electron, or delivery/runtime.
4. Map the touched files to the narrowest helper agents before naming a broader coordinating agent.
5. Locate the real entry points for build, run, test, and validation before proposing changes.
6. Call out cross-layer contracts up front, especially `BANANA_PG_CONNECTION`, `BANANA_NATIVE_PATH`, and `VITE_BANANA_API_BASE_URL`.

## Output

- Touched domains
- Recommended helper or coordinating agents
- Relevant files and folders
- Existing entry points to reuse
- Cross-layer contracts to preserve
- Recommended implementation and validation path

## Resources

- Architecture map: [architecture-map.md](./architecture-map.md)
- Helper routing: [../banana-agent-decomposition/helper-matrix.md](../banana-agent-decomposition/helper-matrix.md)

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](../../agents/domain-teaming-playbook.md) for ownership boundaries, handoff packet expectations, and escalation rules.
- Identify the primary owner, supporting helpers, and validation owner before implementation.
- When handing work to another agent, include objective, ownership reason, touched files, contract impacts, validation state, and open risks.
- Preserve context continuity by carrying forward confirmed assumptions and prior validation signals.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.

## Runtime Contract Lessons (2026-04)

- Treat `Banana Channels (Container Driven)` as the canonical one-click local runtime entry point: run `DETACH=1 scripts/compose-apps.sh`, then launch local desktop Electron with `npm run start:ui` in `src/typescript/electron`.
- Distinguish channels clearly: compose `electron-example` is a smoke container (`npm run smoke`) and is not the desktop UI runtime channel.
- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- In `docker/react.Dockerfile`, copy shared UI package files before install and run `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install so shared deps resolve at runtime.
- Keep Electron container builds reproducible with lockfile install (`npm ci --omit=dev`) and compatibility flags required by current native modules (`CXXFLAGS=-fpermissive`).
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.

