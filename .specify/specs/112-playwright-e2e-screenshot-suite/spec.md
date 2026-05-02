# Feature Specification: Playwright E2E Screenshot Suite

**Feature Branch**: `feature/112-playwright-e2e-screenshot-suite`
**Created**: 2026-05-02
**Status**: Draft
**Input**: User description: "add new specs for playwright e2e that can take a suite of screenshots and update our main readme, i would like users new to the project to be able to scroll down the readme and see tons of screenshots for what the ui does"

## In Scope *(mandatory)*

- Add a Playwright-based E2E screenshot harness at `tests/e2e/playwright/` that spins up the React app via `bun run preview` (or against the compose `apps` stack) and captures a canonical set of full-page and region screenshots.
- Define a fixed screenshot manifest (`tests/e2e/playwright/screenshots.config.ts`) listing each capture: route, viewport, interaction sequence, and output filename.
- Inject the captured screenshots into a dedicated **UI Gallery** section of the root `README.md` using a CI-driven update script (`scripts/update-readme-screenshots.sh` or `scripts/update-readme-screenshots.ts`).
- A GitHub Actions workflow job (`screenshot-suite` in `.github/workflows/compose-ci.yml` or a new `screenshot-ci.yml`) that: builds the React app, runs the Playwright captures, uploads screenshots as artifacts, and opens a PR (or commits directly on a maintenance branch) to update the README gallery.
- Coverage of the key UI surfaces: ensemble predict form (idle, loading, success, error states), chat panel (empty, with messages), ripeness panel, error boundary fallback.
- Keep screenshots committed to `docs/screenshots/` as the stable anchor so the README gallery works on any GitHub render without depending on CI artifacts.

## Out of Scope *(mandatory)*

- Playwright click-path assertions beyond what is needed to reach the target state for a screenshot (full interaction testing is a separate concern).
- Replacing the existing `Banana.E2eTests` (.NET) runner — that project remains for API contract tests; Playwright is browser-UI only.
- Mobile or Electron screenshots in this slice — those are follow-ups.
- Automated visual regression diffing (pixel comparison); this slice is capture-and-publish only.
- Any ASP.NET, native, or `@banana/ui` contract changes.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - README gallery shows the live UI (Priority: P1)

A developer opens the GitHub repo for the first time, scrolls through the README, and immediately sees labelled screenshots of the ensemble predict form, chat panel, and ripeness panel across their key states so they understand the product without cloning anything.

**Why this priority**: The README is the first-impression surface. Screenshots are the fastest path to project comprehension.

**Independent Test**: Assert that `README.md` contains an `## UI Gallery` section with at least 6 `![...](docs/screenshots/...)` image references after the update script runs.

**Acceptance Scenarios**:

1. **Given** the screenshot suite runs, **When** the update script executes, **Then** `README.md` contains an `## UI Gallery` section with inline image tags pointing to `docs/screenshots/`.
2. **Given** an existing gallery section, **When** the script runs again, **Then** the gallery is replaced in-place (idempotent, no duplicate sections).
3. **Given** a screenshot capture fails, **When** the CI job runs, **Then** the job fails with an actionable error message identifying which capture failed and why.

---

### User Story 2 - Repeatable captures across all key UI states (Priority: P1)

A contributor running the capture script locally gets the same set of screenshots as CI so the README gallery stays consistent between local and remote runs.

**Why this priority**: If CI-only screenshots diverge from local, the gallery becomes stale after any UI change.

**Independent Test**: Run `bun run screenshots` (or `npx playwright test --project=screenshots`) locally and assert the same filenames defined in `screenshots.config.ts` are produced.

**Acceptance Scenarios**:

1. **Given** the React app builds locally, **When** `bun run screenshots` is run, **Then** all files in the manifest are written to `docs/screenshots/`.
2. **Given** a state that requires a network interaction (e.g., ensemble predict success), **When** the harness runs, **Then** the compose API stack (or a Playwright mock server) is used so no real ML inference is required.

---

### User Story 3 - CI job uploads and commits updated gallery (Priority: P2)

The CI job automatically keeps the README gallery up to date whenever the React UI changes so contributors never have to manually run the screenshot suite.

**Why this priority**: Automation ensures the gallery doesn't go stale over time.

**Independent Test**: Verify the CI job artifact contains the expected screenshot files and the committed README diff only touches the `## UI Gallery` section.

**Acceptance Scenarios**:

1. **Given** a React UI change merges to main, **When** the screenshot CI job runs, **Then** a commit or PR is opened updating `docs/screenshots/` and the README gallery section.
2. **Given** no UI changes since the last run, **When** the CI job runs, **Then** no commit is produced (idempotent diffing against last committed screenshots).

---

### Edge Cases

- The React app fails to start: Playwright reports a clear `browserContext.newPage` timeout error; the CI job fails and does not update the README.
- A new route is added without updating `screenshots.config.ts`: the manifest drives capture; new routes are opt-in, so no screenshot drift occurs automatically.
- `docs/screenshots/` does not exist on first run: the script creates the directory.
- `README.md` has no existing gallery section: the script appends the gallery after the first `##` section boundary defined as the insertion anchor.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: A Playwright project MUST be added at `tests/e2e/playwright/` with `playwright.config.ts`, `screenshots.config.ts`, and at least one spec file per panel (`ensemble.spec.ts`, `chat.spec.ts`, `ripeness.spec.ts`, `error-boundary.spec.ts`).
- **FR-002**: Each spec MUST define the minimum interaction sequence needed to reach the target visual state, then call `page.screenshot({ fullPage: false })` (region) or `page.screenshot({ fullPage: true })` as defined per capture in `screenshots.config.ts`.
- **FR-003**: Screenshots MUST be written to `docs/screenshots/<name>.png` using the filename defined in the manifest.
- **FR-004**: A `scripts/update-readme-screenshots.sh` (or `.ts`) script MUST parse `docs/screenshots/` and generate/replace the `## UI Gallery` section in `README.md` with `![<label>](docs/screenshots/<name>.png)` entries grouped by panel.
- **FR-005**: `README.md` MUST contain a stable `<!-- GALLERY_START -->` / `<!-- GALLERY_END -->` marker pair that the update script uses as its insertion target so the gallery never spills outside its block.
- **FR-006**: A `screenshot-suite` CI job MUST build the React app, start a Vite preview server (or compose `apps` profile), run the Playwright captures, and call the update script. The job MUST upload `docs/screenshots/` as a named artifact.
- **FR-007**: The Playwright harness MUST mock or stub the API for ML-inference-dependent states (ensemble success, error) so the suite is hermetic and does not require a running inference server.
- **FR-008**: `package.json` in `src/typescript/react` MUST gain a `screenshots` script entry (`bunx playwright test --project=screenshots`) and a `playwright` dev dependency entry.
- **FR-009**: `docs/screenshots/` MUST be committed to the repository (not .gitignored) so the README gallery renders on GitHub without CI artifact links.
- **FR-010**: The update script MUST be idempotent: running it twice with identical screenshots produces no diff.

### Key Entities

- **Screenshot Manifest** (`screenshots.config.ts`): Array of `{ name, route, viewport, interactions, region }` capture definitions.
- **Gallery Markers**: `<!-- GALLERY_START -->` and `<!-- GALLERY_END -->` HTML comments in `README.md` used as the update script's insertion target.
- **Hermetic Mock Server**: A Playwright `route()` intercept or `msw` handler that satisfies the ensemble and chat API shapes so captures work without a real backend.
- **`docs/screenshots/`**: Committed PNG directory serving as the stable GitHub-render anchor for the README gallery.

## Success Criteria *(mandatory)*

### Validation Lane

```bash
# All Playwright screenshot specs pass
bunx playwright test --project=screenshots --reporter=list
# README gallery section present with expected image refs
python scripts/validate-spec-tasks-parity.py .specify/specs/112-playwright-e2e-screenshot-suite
# README contains gallery markers
grep -c "GALLERY_START" README.md
```

### Measurable Outcomes

- **SC-001**: `bunx playwright test --project=screenshots` exits 0 with at least 10 screenshots captured.
- **SC-002**: `README.md` contains `<!-- GALLERY_START -->`, at least 6 `![` image lines, and `<!-- GALLERY_END -->` after the update script runs.
- **SC-003**: The `screenshot-suite` CI job exits 0 and uploads a `docs/screenshots` artifact containing all manifest-defined PNGs.
- **SC-004**: The update script is idempotent: `git diff README.md` is empty after a second run with unchanged screenshots.
- **SC-005**: All existing `bun test` and `.NET` E2E tests continue to pass (no regressions from adding Playwright).

## Assumptions

- Playwright with Chromium is installable in the CI environment via `bunx playwright install chromium`.
- The React Vite preview server starts within 30 seconds in CI (default Playwright timeout is sufficient).
- API interactions needed for screenshots can be fully mocked with Playwright's `page.route()` intercepts — no real inference server required for captures.
- `docs/screenshots/` PNGs committed to the repo will stay < 5 MB total (Playwright screenshot compression keeps individual captures < 500 KB).
- The README gallery will have the `<!-- GALLERY_START -->` / `<!-- GALLERY_END -->` markers inserted manually (or by the setup task) before the first automated run.
