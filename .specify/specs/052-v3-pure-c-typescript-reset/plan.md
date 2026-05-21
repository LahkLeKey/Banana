# Implementation Plan: [FEATURE]

**Branch**: `[###-feature-name]` | **Date**: [DATE] | **Spec**: [link]
**Jurisdiction**: [federal | domain | package] | **Agent of Record**: [agent/team]
**Input**: Feature specification from `.specify/specs/[###-feature-name]/spec.md`

**Note**: This template is filled in by the `specify plan` command. See `.specify/templates/plan-template.md` for the execution workflow.

## Summary

[Extract from feature spec: primary requirement + technical approach from research]

## Authority Chain

- **Governing Spec**: `.specify/specs/[###-feature-name]/spec.md`
- **Bounded Context**: [domain governed by this plan]
- **Executing Authority**: [agent/team implementing the plan]
- **Superseded Inputs**: [none | prior specs/docs absorbed by this feature]
- **Archive Action**: [none | archive prior specs from active baseline in same change]

## Technical Context

<!--
  ACTION REQUIRED: Replace the content in this section with the technical details
  for the project. The structure here is presented in advisory capacity to guide
  the iteration process.
-->

**Language/Version**: [e.g., Python 3.11, Swift 5.9, Rust 1.75 or NEEDS CLARIFICATION]
**Primary Dependencies**: [e.g., FastAPI, UIKit, LLVM or NEEDS CLARIFICATION]
**Storage**: [if applicable, e.g., PostgreSQL, CoreData, files or N/A]
**Testing**: [e.g., pytest, XCTest, cargo test or NEEDS CLARIFICATION]
**Target Platform**: [e.g., Linux server, iOS 15+, WASM or NEEDS CLARIFICATION]
**Project Type**: [e.g., library/cli/web-service/mobile-app/compiler/desktop-app or NEEDS CLARIFICATION]
**Performance Goals**: [domain-specific, e.g., 1000 req/s, 10k lines/sec, 60 fps or NEEDS CLARIFICATION]
**Constraints**: [domain-specific, e.g., <200ms p95, <100MB memory, offline-capable or NEEDS CLARIFICATION]
**Scale/Scope**: [domain-specific, e.g., 10k users, 1M LOC, 50 screens or NEEDS CLARIFICATION]

## Constitution Check

*GATE: Must pass before research. Re-check after design.*

[Gates determined based on constitution file]

- If confidence in the next code-improving step is below 80%, stop and resolve the uncertainty with targeted Q/A before continuing. Record the resulting constraint, assumption, or decision in this plan.

## Orchestration Preflight

- Run extension health preflight before implementation orchestration:
  - `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
- Run confidence gate before major execution slices:
  - `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --threshold 80 --notes "startup gate"`
- Append heartbeat evidence per major step in `.specify/specs/[###-feature-name]/heartbeat-log.md`.
- If confidence drops below 80, pause and request human input before continuing.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/[###-feature]/
├── plan.md              # This file (specify plan command output)
├── research.md          # research output (specify plan command)
├── data-model.md        # design output (specify plan command)
├── quickstart.md        # design output (specify plan command)
├── contracts/           # design output (specify plan command)
└── tasks.md             # tasks output (specify tasks command - NOT created by specify plan)
```

### Source Code (repository root)
<!--
  ACTION REQUIRED: Replace the placeholder tree below with the concrete layout
  for this feature. Delete unused options and expand the chosen structure with
  real paths (e.g., apps/admin, packages/something). The delivered plan must
  not include Option labels.
-->

```text
# [REMOVE IF UNUSED] Option 1: Single project (DEFAULT)
src/
├── models/
├── services/
├── cli/
└── lib/

tests/
├── contract/
├── integration/
└── unit/

# [REMOVE IF UNUSED] Option 2: Web application (when "frontend" + "backend" detected)
backend/
├── src/
│   ├── models/
│   ├── services/
│   └── api/
└── tests/

frontend/
├── src/
│   ├── components/
│   ├── pages/
│   └── services/
└── tests/

# [REMOVE IF UNUSED] Option 3: Mobile + API (when "iOS/Android" detected)
api/
└── [same as backend above]

ios/ or android/
└── [platform-specific structure: feature modules, UI flows, platform tests]
```

**Structure Decision**: [Document the selected structure and reference the real
directories captured above]

## DDD/SOLID Decomposition

- **Domain Policy**: Identify immutable orchestration rules (confidence threshold, checkpoint contract, sequencing policy).
- **Application Flow**: Describe how scripts/pipelines compose those rules without reimplementing policy.
- **Infrastructure Adapters**: List concrete scripts/artifacts that persist evidence and execute external commands.
- **Single Responsibility**: Ensure each script/module owns one concern and is testable in isolation.

## Domain-Contract Test Decomposition

- **Bounded Test Suites**: Name native and engine test targets after the domain they verify (for example physics, AI FSM, navigation, controller, render, world/signals) instead of phase labels.
- **Suite Ownership**: Keep each suite focused on one responsibility and expose a thin orchestrator target only for aggregate execution.
- **Testable Boundaries**: Describe the minimum unit of behavior for each suite so tests mirror the codebase's bounded contexts and SOLID seams.
- **Coverage Contract**: Call out the exact coverage/static-analysis gates required for native slices, including 100% line coverage and leak/static checks when applicable.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| [e.g., 4th project] | [current need] | [why 3 projects insufficient] |
| [e.g., Repository pattern] | [specific problem] | [why direct DB access insufficient] |
