# Triage Intake: Issue #237

- Source issue: https://github.com/LahkLeKey/Banana/issues/237
- Source title: epic: share regression corpus features with binary classification pipeline
- Source labels: automation,model-training,triage-idea,copilot-suggestion,ai-generated,source-ai-issue,epic,epic-decomposed,agent:banana-classifier-agent,speckit-driven
- Intake source: ai
- Target agent: not-specified

## Idea

## Objective
Enable the binary classification model to consume compatible data slices from the regression scraper corpus without duplicating ingestion logic.

## Scope
- Define shared dataset contract between regression and binary model pipelines.
- Add transformation rules to project regression-ready samples into binary labels/features.
- Version the shared contract to prevent silent drift between pipelines.
- Add compatibility tests across both model training paths.

## Acceptance Criteria
- Binary classification training can ingest shared regression-derived data slice.
- Contract compatibility checks fail fast on schema drift.
- Both pipelines can run from the same source refresh cycle.

## Cloud Orchestration Contract

- Entry workflow: Orchestrate Triage Idea Cloud
- Pull request labels: automation,triaged-item,copilot-auto-approve,copilot-autonomous-cycle,speckit-driven,agent:workflow-agent,source-ai-issue,epic,agent:banana-classifier-agent
- Base branch: main
- Branch prefix: triage

## Epic Decomposition

- Decomposed: true
- First task issue: #264 (https://github.com/LahkLeKey/Banana/issues/264)
- First task workflow run: https://github.com/LahkLeKey/Banana/actions/runs/24922897820
