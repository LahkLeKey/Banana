# Triage Intake: Issue #239

- Source issue: https://github.com/LahkLeKey/Banana/issues/239
- Source title: epic: enforce jaccard similarity and confusion-matrix quality gates
- Source labels: automation,model-training,triage-idea,copilot-suggestion,ai-generated,source-ai-issue,epic,epic-decomposed,agent:integration-agent,speckit-driven
- Intake source: ai
- Target agent: not-specified

## Idea

## Objective
Establish quantitative evaluation gates using Jaccard similarity and confusion matrix metrics across scraped and Faker datasets.

## Scope
- Define evaluation harness comparing primary (scraped) and secondary (Faker) datasets.
- Compute Jaccard similarity for overlap/coverage signals across label sets or feature buckets.
- Compute confusion matrix metrics for binary classifier quality against shared datasets.
- Set threshold policies and fail conditions for CI/training workflows.

## Acceptance Criteria
- Jaccard and confusion-matrix metrics are produced in every evaluation run.
- Quality thresholds are explicit and enforced.
- Evaluation outputs are exported for management and engineering review.

## Cloud Orchestration Contract

- Entry workflow: Orchestrate Triage Idea Cloud
- Pull request labels: automation,triaged-item,copilot-auto-approve,copilot-autonomous-cycle,speckit-driven,agent:workflow-agent,source-ai-issue,epic,agent:integration-agent
- Base branch: main
- Branch prefix: triage

## Epic Decomposition

- Decomposed: true
- First task issue: #261 (https://github.com/LahkLeKey/Banana/issues/261)
- First task workflow run: https://github.com/LahkLeKey/Banana/actions/runs/24922887485
