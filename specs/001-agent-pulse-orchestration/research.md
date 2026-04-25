# Research: Agent Pulse Orchestration

## Decision 1: Externalize default autonomous increment plan into a versioned JSON file

- Decision: Store the default increment catalog in a repository file and load it through `BANANA_SDLC_INCREMENT_PLAN_PATH` when no dispatch override JSON is supplied.
- Rationale: A dedicated file is easier to review, diff, and maintain than a long inline YAML string; this directly improves management readability and operational safety.
- Alternatives considered:
  - Keep inline workflow JSON literal. Rejected because it is hard to audit and error-prone to edit.
  - Split JSON across multiple workflow env variables. Rejected because it remains YAML-heavy and harder to validate.

## Decision 2: Use one ownership-labeled increment per participating agent

- Decision: Expand the default plan to include agent-scoped increments with explicit `agent:*` labels and contributor attribution.
- Rationale: The existing PR orchestration path already derives contributor identity from `agent:*` labels; using this contract yields management-visible ownership across PR and commit activity.
- Alternatives considered:
  - Keep only classifier increments and infer ownership from file paths. Rejected because it does not produce explicit multi-agent visibility.
  - Use a single "meta-agent" increment. Rejected because it hides ownership detail.

## Decision 3: Generate management-facing per-agent activity records in docs

- Decision: Add a script that writes immutable per-run snapshot markdown files in per-agent folders and keep a static management index that maps agents to deep-dive roots.
- Rationale: Management can deep dive by agent without opening workflow internals; snapshot files are conflict-safe across multiple autonomous increments.
- Alternatives considered:
  - Keep records only in runtime artifacts. Rejected because artifacts are less discoverable for leadership and trend review.
  - Post comments only in issues/PRs. Rejected because comments are fragmented and not agent-indexed.

## Decision 4: Preserve existing not-banana training increments while adding visibility increments

- Decision: Keep existing feedback/training increments in the default plan and add additional management visibility increments for other agent lanes.
- Rationale: Maintains current model-improvement behavior while delivering the requested broader suite visibility.
- Alternatives considered:
  - Replace existing training increments with visibility-only increments. Rejected because it regresses existing autonomous behavior.

## Decision 5: Validate via existing governance checks and dry-run orchestration

- Decision: Use `scripts/validate-ai-contracts.py` and a local `BANANA_LOCAL_DRY_RUN=true` orchestration execution for verification.
- Rationale: Reuses existing quality gates without introducing bespoke validation tooling.
- Alternatives considered:
  - Add new one-off validation scripts. Rejected because existing checks already cover this workflow surface.

## Decision 6: Use a native C deterministic lane model to guide plan rendering

- Decision: Introduce `banana_agent_pulse_model.c` + CLI output and render final increment plans through a Python bridge script.
- Rationale: This satisfies the requirement for a custom native deterministic model while keeping JSON transformation maintainable and testable.
- Alternatives considered:
  - Keep deterministic logic only in Python. Rejected because the request explicitly requires a custom C native implementation.
  - Move full JSON parsing/rewriting into C. Rejected because this increases native complexity with low value versus a C model + Python renderer split.
