# Quickstart: Regression Corpus Sharing and Evaluation Gates

## 1. Validate workflow and AI contracts

```bash
cd /c/Github/Banana
python scripts/validate-ai-contracts.py
```

## 2. Build scraper-driven regression corpus

```bash
cd /c/Github/Banana
python scripts/scrape-regression-corpus.py \
  --policy .specify/specs/002-regression-binary-faker-metrics/contracts/scraper-regression-corpus.md \
  --output data/regression-corpus/latest.jsonl
```

## 3. Project shared regression-to-binary dataset

```bash
cd /c/Github/Banana
python scripts/project-shared-regression-binary-dataset.py \
  --input data/regression-corpus/latest.jsonl \
  --contract .specify/specs/002-regression-binary-faker-metrics/contracts/regression-binary-shared-dataset.md \
  --output artifacts/model-data/shared-binary-dataset.jsonl
```

## 4. Generate deterministic Faker secondary dataset

```bash
cd /c/Github/Banana
python scripts/generate-faker-secondary-dataset.py \
  --contract .specify/specs/002-regression-binary-faker-metrics/contracts/faker-secondary-dataset.md \
  --seed 22402 \
  --output artifacts/model-data/faker-secondary-dataset.jsonl
```

## 5. Evaluate quality gates (Jaccard + confusion matrix)

```bash
cd /c/Github/Banana
python scripts/evaluate-regression-binary-quality-gates.py \
  --primary artifacts/model-data/shared-binary-dataset.jsonl \
  --secondary artifacts/model-data/faker-secondary-dataset.jsonl \
  --contract .specify/specs/002-regression-binary-faker-metrics/contracts/jaccard-confusion-gates.md \
  --output artifacts/model-data/evaluation-report.json
```

## 6. Verify dual-track report outputs

- Human-readable summary path is published and linked from the runbook entry.
- AI-audit detail path is published with full metric and threshold trace context.
