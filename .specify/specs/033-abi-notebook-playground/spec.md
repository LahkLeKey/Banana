# Feature Specification: Notebook MMO Gameplay Client

**Feature Branch**: `033-abi-notebook-playground`

**Created**: 2026-06-08

**Status**: Draft

**Input**: User request to use notebooks as the primary gameplay renderer/client, split bloated playground page into shared components, introduce a main menu flow, and plan API orchestration alignment.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Notebook-first gameplay shell (Priority: P1)

As a player/developer, I can launch into a notebook-first gameplay shell from a main menu so notebook assets become the primary client runtime surface.

**Why this priority**: This establishes the new gameplay-client identity and unblocks all downstream flows.

**Independent Test**: Open `/` and verify the main menu renders, displays notebook status, and routes into notebook render surface without raw JSON dump UI.

**Acceptance Scenarios**:

1. **Given** notebook assets exist, **When** user loads `/`, **Then** a main menu with a launch action into notebook gameplay appears.
2. **Given** notebook assets are missing, **When** user loads `/`, **Then** menu shows actionable regeneration guidance and avoids hard crashes.

---

### User Story 2 - Shared dashboard component architecture (Priority: P2)

As a frontend maintainer, I can evolve notebook gameplay UI through small shared components instead of one large page file.

**Why this priority**: Prevents page bloat/churn and improves iteration speed for MMO client UX.

**Independent Test**: Verify playground route compiles with extracted component modules and no behavior regression in notebook launch flow.

**Acceptance Scenarios**:

1. **Given** componentized layout, **When** I change menu/metrics/list panels, **Then** changes stay isolated to component files.
2. **Given** manifest and notebook data, **When** explorer renders, **Then** shared components consume typed props without page-level duplication.

---

### User Story 3 - API orchestration-ready notebook client (Priority: P3)

As an integration engineer, I can wire notebook gameplay client actions to API orchestration progressively without redesigning the UI shell.

**Why this priority**: Aligns current notebook client with future MMO orchestration/API flows.

**Independent Test**: Confirm defined service hooks/interfaces exist and menu actions can call API-backed loaders with graceful fallback.

**Acceptance Scenarios**:

1. **Given** API is reachable, **When** menu requests manifest/session metadata, **Then** data hydrates client panels.
2. **Given** API is unavailable, **When** client loads, **Then** local notebook artifacts still power fallback mode.

---

### Edge Cases

- Manifest exists but notebook file is missing or malformed.
- Notebook contains cells without source headers.
- API returns partial payloads for session/menu metadata.
- Large file catalogs exceed simple list rendering and need filtering/pagination.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST expose a main-menu-first notebook gameplay client route at `/`.
- **FR-002**: System MUST decompose current playground page into shared components (menu, stats, source list, code surface, health panel).
- **FR-003**: System MUST load notebook manifest and notebook document into typed client state.
- **FR-004**: System MUST provide searchable file navigation and selectable code preview from notebook cells.
- **FR-005**: System MUST keep notebook generation flow (`scaffold-abi-notebook-workflow.sh`) as source of truth for notebook artifacts.
- **FR-006**: System MUST define API orchestration integration points for menu/session/notebook metadata retrieval.
- **FR-007**: System MUST preserve fallback operation from static `/notebooks/*` assets when API services are unavailable.

### Key Entities *(include if feature involves data)*

- **GameplayMenuModel**: Launch actions, client status, notebook readiness state.
- **NotebookRuntimeManifest**: Generated timestamp, source root, file catalog, render limits.
- **NotebookCellRuntimeIndex**: Mapping from source path to notebook code payload for renderer view.
- **NotebookClientSessionState**: API-backed metadata and local fallback status for gameplay shell.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Main menu and notebook gameplay shell render at `/` without raw JSON iframe dependency.
- **SC-002**: `DataSciencePlaygroundPage` responsibilities are split into shared components with no single monolithic control surface.
- **SC-003**: Notebook explorer interactions (filter/select/render) stay responsive for 291+ indexed files.
- **SC-004**: API-orchestration hooks are documented and scaffolded with typed fallbacks to local notebook assets.

## Iteration Checklist

- [ ] Stage 1: Splash route (`/`) renders and transitions to engine boot.
- [ ] Stage 2: Engine boot route (`/banana-engine`) transitions to login.
- [ ] Stage 3: Login route (`/login`) supports guest entry without Steam account.
- [ ] Stage 4: API route scaffold exists for guest auth bootstrap (`POST /auth/guest/start`).
- [ ] Stage 5: Game main menu route (`/game-main-menu`) enforces authenticated session.
- [ ] Stage 6: Character select route (`/select-character`) stores selected operator profile.
- [ ] Stage 7: Notebook client route (`/notebooks`) enforces both authenticated session and selected character.
- [ ] Stage 8: Session validation/logout support guest sessions and Steam sessions.
- [ ] Stage 9: Add telemetry checkpoints for each onboarding stage transition.
- [ ] Stage 10: Add production hardening pass (rate limits, guest alias validation policy, abuse controls).

## Assumptions

- React continues as the current web gameplay shell runtime.
- Notebook files remain generated artifacts, not edited directly by users.
- API orchestration integration can be incremental and backward-compatible with local assets.
- Existing routes like `/marketing` remain available during transition.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: No storefront claims changed; this is internal gameplay client UX evolution.
- **Cross-Domain Contracts**: Touches scripts artifact pipeline, React client shell, and future API orchestration interfaces.
- **Quality Gates**: React build/smoke, notebook generation validation, and integration fallback checks.
- **Delivery Evidence**: Notebook artifacts, dashboard render snapshots, and command logs for generation/build validations.

## Storefront & Release Claims *(required when public Steam copy is affected)*

- Not applicable in this feature phase.
