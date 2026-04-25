# Quickstart: Backlog Risk-Value Prioritization

## 1. Validate AI customization contracts

```bash
cd /c/Github/Banana
python scripts/validate-ai-contracts.py
```

## 2. Capture ready backlog candidates

```bash
cd /c/Github/Banana
GH_PAGER=cat gh issue list --state open --limit 200 --search "label:status:ready" --json number,title,labels,updatedAt
```

## 3. Run prioritization scoring

```bash
cd /c/Github/Banana
python scripts/prioritize-backlog-risk-value.py \
  --policy docs/automation/backlog/prioritization-policy.json \
  --input artifacts/sdlc-orchestration/open-ready-issues.json \
  --output-json artifacts/sdlc-orchestration/backlog-priority-snapshot-latest.json \
  --output-md docs/automation/backlog/latest-priority-slate.md
```

## 4. Verify immediate queue is dependency-safe

- Confirm `immediate_queue` contains no blocked issues.
- Confirm top recommendations include owner helper labels and validation owner.

## 5. Inspect dual-track outputs

- Human summary: `docs/automation/backlog/latest-priority-slate.md`
- AI-audit snapshot: `artifacts/sdlc-orchestration/backlog-priority-snapshot-latest.json`
