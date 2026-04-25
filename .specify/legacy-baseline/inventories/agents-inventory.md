# Agents Inventory (v1 snapshot)

Source: `.github/agents/*.agent.md` (28 entries; one is the playbook). Verbatim
content preserved at `../github-ai-surface/agents/`.

## Top-level orchestrators

- `banana-sdlc` — full SDLC orchestrator across discovery → release.
- `banana-planner` — read-only planning.
- `banana-reviewer` — change review.

## Domain orchestrators (call helpers)

- `csharp-api-agent` (parents: api-pipeline + api-interop)
- `native-c-agent` (parents: native-core + native-dal + native-wrapper)
- `react-agent` (parents: react-ui + electron + mobile)
- `infrastructure-agent` (parents: compose-runtime + workflow)
- `integration-agent` — cross-domain validation.

## Helper agents (smallest scope; preferred per `banana-agent-decomposition` skill)

- `api-pipeline-agent` — controllers/services/middleware/DI.
- `api-interop-agent` — P/Invoke + status mapping.
- `native-core-agent` — `src/native/core/`.
- `native-dal-agent` — `src/native/core/dal/` (Postgres-gated).
- `native-wrapper-agent` — `src/native/wrapper/` ABI.
- `react-ui-agent` — `src/typescript/react/src/` + `@banana/ui`.
- `electron-agent` — Electron main/preload/bridge.
- `mobile-runtime-agent` — Android/iOS WSLg flows.
- `compose-runtime-agent` — compose + scripts.
- `workflow-agent` — `.github/workflows/`.
- `banana-classifier-agent` — banana vs not-banana specialization.
- `technical-writer-agent` — wiki + docs split (human casual, AI verbose).
- `test-triage-agent` — failure isolation + routing.
- `value-risk-prioritization-agent` — backlog scoring.

## Spec Kit Git agents

- `speckit.git.commit`
- `speckit.git.feature`
- `speckit.git.initialize`
- `speckit.git.remote`
- `speckit.git.validate`

## Reference

- `domain-teaming-playbook.md` — non-agent reference doc on team composition rules; not a runnable agent.

## v2 Constraints (spec 013)

- The 14 helper agents are the canonical "narrowest helper" surface called
  out in `.github/copilot-instructions.md`. Renaming is OK; collapsing
  helpers requires explicit spec update.
- Parent/helper relationships above MUST stay loosely coupled — parents
  invoke helpers, helpers don't reach back up.
