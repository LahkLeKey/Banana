# Quickstart: Human Left/Right Brain Training Workbench

## 1. Pre-validate contracts

- python scripts/validate-ai-contracts.py
- bash scripts/validate-spec-tasks-parity.sh --all

## 2. Start API and React surfaces for workbench development

- Run existing app entrypoints used for local React + API workflows in this repo.

## 3. Execute a left-brain training session via workbench

- Choose lane left-brain.
- Choose corpus and ci/local profile.
- Run single-session training.
- Confirm metrics and threshold outcomes render.

## 4. Execute a right-brain training session via workbench

- Choose lane right-brain.
- Run the same end-to-end flow.
- Confirm persisted session reference created in data/.

## 5. Promote candidate/stable and verify audit trail

- Mark a passed run candidate.
- Promote to stable.
- Confirm operator action history is recorded.
