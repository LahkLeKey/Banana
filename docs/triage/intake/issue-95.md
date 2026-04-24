# Triage Intake: Issue #95

- Source issue: https://github.com/LahkLeKey/Banana/issues/95
- Source title: epic: rule-of-three refactor gating policy
- Source labels: automation,triage-idea,human-triage,ai-generated,source-ai-issue,epic,epic-decomposed,agent:workflow-agent,speckit-driven
- Intake source: ai
- Target agent: not-specified

## Idea

<!-- banana-intake-source: human -->
<!-- banana-target-agent: workflow-agent -->

## Objective
Establish and enforce a rule-of-three gating policy so refactors are triggered by repeated proven patterns, reducing speculative churn and preserving delivery velocity.

## Architecture Scope
- Policy definition in repository docs and workflow contracts
- CI and automation guardrails for refactor proposals
- PR/issue intake signals and triage checks for policy compliance
- Reporting paths for exemptions, overrides, and retrospective learning

## User Story Slices
- As a maintainer, I can see a clear definition of when refactoring is allowed.
- As a reviewer, I can verify rule-of-three evidence in PRs proposing refactors.
- As an automation workflow, I can flag or block refactors missing required evidence.
- As a maintainer, I can request a documented exception with explicit risk acceptance.

## Bootstrap Tasks
- Define policy language with examples of qualifying repeated patterns.
- Add PR template/checklist fields for rule-of-three evidence.
- Add workflow validation to detect missing refactor evidence markers.
- Add exemption path with required reviewer sign-off and audit trail.

## Constraints and Risks
- Must not block urgent bugfixes that require minimal safe refactors.
- Must keep enforcement transparent and actionable, not opaque.
- Must avoid false positives for files with generated code or mechanical updates.
- Must preserve existing LahkLeKey human-approval merge governance.

## Acceptance Criteria
- Policy document and automation contract are merged and referenced from contributor workflows.
- Refactor PRs require explicit evidence of three repeated instances or an approved exemption.
- CI reports policy failures with actionable remediation guidance.
- Exemptions are auditable with rationale and approver identity.
- Policy effectiveness is reviewed after initial rollout with measurable signals.

## Cloud Orchestration Contract

- Entry workflow: Orchestrate Triage Idea Cloud
- Pull request labels: automation,triaged-item,copilot-auto-approve,copilot-autonomous-cycle,speckit-driven,agent:workflow-agent,source-ai-issue,epic
- Base branch: main
- Branch prefix: triage

## Epic Decomposition

- Decomposed: true
- First task issue: #107 (https://github.com/LahkLeKey/Banana/issues/107)
- First task workflow run: https://github.com/LahkLeKey/Banana/actions/runs/24893501916
