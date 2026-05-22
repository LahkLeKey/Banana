# Contract: Constitution-Aligned Feature Delivery

## Purpose
Define the artifact-level interface for feature delivery under the Banana Engineer constitution.

## Inputs
- Feature description provided by user.
- Active constitution in `.specify/memory/constitution.md`.
- Active templates under `.specify/templates/`.

## Required Outputs
1. `spec.md`
- Must include mandatory sections from template.
- Must include constitution alignment with disclosure, contracts, quality gates, and evidence.

2. `plan.md`
- Must include populated technical context with no unresolved clarifications.
- Must pass constitution gate checks before and after design artifacts are produced.

3. `research.md`
- Must contain explicit decisions with rationale and alternatives.

4. `data-model.md`
- Must define entities, validation rules, and state transitions where applicable.

5. `quickstart.md`
- Must provide reproducible steps for planning and validation in this repository.

6. `tasks.md` (next command phase)
- Must map implementation tasks to user stories and quality gates.

## Validation Rules
- No unresolved `NEEDS CLARIFICATION` markers in final planning artifacts.
- Every constitutional principle must be represented in at least one explicit gate or deliverable.
- Artifact paths must be workspace-relative and point to existing files.

## Failure Conditions
- Missing mandatory sections in `spec.md` or `plan.md`.
- Constitution check gate not satisfied.
- Missing evidence mapping for disclosure alignment and reproducible runtime validation.
