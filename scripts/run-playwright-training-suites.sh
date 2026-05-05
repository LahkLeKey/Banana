#!/usr/bin/env bash
# run-playwright-training-suites.sh
#
# Runs Playwright reinforcement training suites and emits a JSON session
# summary to artifacts/playwright-training/.
#
# Usage:
#   bash scripts/run-playwright-training-suites.sh [--mode live|mock] [--suite banana|ripeness|all]
#                                                   [--no-apply] [--no-auto-train]
#
# Options:
#   --mode live    Use the live API (requires runtime stack to be up)
#   --mode mock    Stub all API calls (default; suitable for CI)
#   --no-apply     Skip feedback ingestion into corpus (default is apply)
#   --no-auto-train
#                  Skip post-suite training runs (default is auto-train)
#   --suite        Which suite(s) to run: banana, ripeness, or all (default: all)
#
# Output:
#   artifacts/playwright-training/session-<YYYYMMDD-HHMMSS>.json
#   Exit code 0  = all suites passed disagreement guard
#   Exit code 1  = one or more suites failed or errored

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

# ── Defaults ──────────────────────────────────────────────────────────────

MODE="mock"
APPLY=true
AUTO_TRAIN=true
SUITE="all"

# ── Arg parse ─────────────────────────────────────────────────────────────

while [[ $# -gt 0 ]]; do
    case "$1" in
        --mode)
            MODE="$2"; shift 2 ;;
        --apply)
            APPLY=true; shift ;;
        --no-apply)
            APPLY=false; shift ;;
        --auto-train)
            AUTO_TRAIN=true; shift ;;
        --no-auto-train)
            AUTO_TRAIN=false; shift ;;
        --suite)
            SUITE="$2"; shift 2 ;;
        *)
            echo "Unknown argument: $1" >&2
            exit 1 ;;
    esac
done

if [[ "$MODE" != "live" && "$MODE" != "mock" ]]; then
    echo "Error: --mode must be 'live' or 'mock'" >&2
    exit 1
fi

if [[ "$SUITE" != "banana" && "$SUITE" != "ripeness" && "$SUITE" != "all" ]]; then
    echo "Error: --suite must be 'banana', 'ripeness', or 'all'" >&2
    exit 1
fi

# ── Dirs ──────────────────────────────────────────────────────────────────

ARTIFACTS_DIR="${REPO_ROOT}/artifacts/playwright-training"
mkdir -p "${ARTIFACTS_DIR}"

TIMESTAMP="$(date -u '+%Y%m%d-%H%M%S')"
SESSION_FILE="${ARTIFACTS_DIR}/session-${TIMESTAMP}.json"

# ── Resolve playwright config ─────────────────────────────────────────────

PLAYWRIGHT_CONFIG="${REPO_ROOT}/tests/e2e/playwright/playwright.config.ts"
if [[ ! -f "${PLAYWRIGHT_CONFIG}" ]]; then
    echo "Error: playwright.config.ts not found at ${PLAYWRIGHT_CONFIG}" >&2
    exit 1
fi

# ── Build suite pattern ───────────────────────────────────────────────────

case "${SUITE}" in
    banana)   SPEC_PATTERN="tests/e2e/playwright/specs/banana-reinforcement.spec.ts" ;;
    ripeness) SPEC_PATTERN="tests/e2e/playwright/specs/ripeness-reinforcement.spec.ts" ;;
    all)      SPEC_PATTERN="tests/e2e/playwright/specs/*-reinforcement.spec.ts" ;;
esac

# ── Run ───────────────────────────────────────────────────────────────────

echo "=== Playwright Reinforcement Training Suites ==="
echo "  mode   : ${MODE}"
echo "  suite  : ${SUITE}"
echo "  apply  : ${APPLY}"
echo "  train  : ${AUTO_TRAIN}"
echo "  output : ${SESSION_FILE}"
echo ""

export BANANA_TRAINING_MODE="${MODE}"

PYTHON=""
if command -v python3 >/dev/null 2>&1; then
    PYTHON="$(command -v python3)"
elif command -v python >/dev/null 2>&1; then
    PYTHON="$(command -v python)"
elif [[ -x "${REPO_ROOT}/.venv/bin/python" ]]; then
    PYTHON="${REPO_ROOT}/.venv/bin/python"
elif [[ -x "${REPO_ROOT}/.venv/Scripts/python.exe" ]]; then
    PYTHON="${REPO_ROOT}/.venv/Scripts/python.exe"
else
    echo "Error: Python interpreter not found (.venv, python3, or python)." >&2
    exit 1
fi

to_python_path() {
    local p="$1"
    local python_lower
    python_lower="$(echo "${PYTHON}" | tr '[:upper:]' '[:lower:]')"
    if [[ "${python_lower}" == *.exe ]] && command -v cygpath >/dev/null 2>&1; then
        cygpath -w "$p"
    else
        echo "$p"
    fi
}

cd "${REPO_ROOT}"

EXIT_CODE=0
LOG_FILE="${ARTIFACTS_DIR}/playwright-${TIMESTAMP}.log"

PLAYWRIGHT_CONFIG_FROM_REACT="../../../tests/e2e/playwright/playwright.config.ts"
SPEC_ARGS=()
case "${SUITE}" in
    banana)
        SPEC_ARGS+=("banana-reinforcement.spec.ts")
        ;;
    ripeness)
        SPEC_ARGS+=("ripeness-reinforcement.spec.ts")
        ;;
    all)
        SPEC_ARGS+=(".*reinforcement\\.spec\\.ts")
        ;;
esac

(
    cd "${REPO_ROOT}/src/typescript/react"
    bunx playwright test \
        --config="${PLAYWRIGHT_CONFIG_FROM_REACT}" \
        "${SPEC_ARGS[@]}" \
        --reporter=json
) 2>&1 | tee "${LOG_FILE}" || EXIT_CODE=$?

# ── Emit session summary JSON ─────────────────────────────────────────────

BANANA_INBOX="${REPO_ROOT}/data/not-banana/feedback/inbox.json"
RIPENESS_INBOX="${REPO_ROOT}/data/ripeness/feedback/inbox.json"

banana_count=0
ripeness_count=0

if [[ -f "${BANANA_INBOX}" ]]; then
    BANANA_INBOX_PY="$(to_python_path "${BANANA_INBOX}")"
    banana_count=$(
        "${PYTHON}" -c "import json; d=json.load(open('${BANANA_INBOX_PY}', encoding='utf-8')); print(sum(1 for e in d if isinstance(e, dict) and e.get('source')=='playwright-reinforcement'))" \
            2>/dev/null || echo 0
    )
fi
if [[ -f "${RIPENESS_INBOX}" ]]; then
    RIPENESS_INBOX_PY="$(to_python_path "${RIPENESS_INBOX}")"
    ripeness_count=$(
        "${PYTHON}" -c "import json; d=json.load(open('${RIPENESS_INBOX_PY}', encoding='utf-8')); print(sum(1 for e in d if isinstance(e, dict) and e.get('source')=='playwright-reinforcement'))" \
            2>/dev/null || echo 0
    )
fi

SESSION_FILE_PY="$(to_python_path "${SESSION_FILE}")"

"${PYTHON}" - <<PY
import json
from datetime import datetime, timezone
from pathlib import Path

session_path = Path("${SESSION_FILE_PY}")
session_path.parent.mkdir(parents=True, exist_ok=True)

summary = {
    "session_id": "pw-${TIMESTAMP}",
    "mode": "${MODE}",
    "suite": "${SUITE}",
    "exit_code": ${EXIT_CODE},
    "banana_feedback_count": ${banana_count},
    "ripeness_feedback_count": ${ripeness_count},
    "generated_at_utc": datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z"),
}
with session_path.open("w", encoding="utf-8") as f:
    json.dump(summary, f, indent=2)
    f.write("\n")
print(json.dumps(summary, indent=2))
PY

echo ""
echo "=== Session summary written to: ${SESSION_FILE} ==="

# ── Optional apply and train ──────────────────────────────────────────────

if [[ "${APPLY}" == "true" && "${EXIT_CODE}" == "0" ]]; then
    echo ""
    echo "=== Applying not-banana feedback into corpus (autonomous mode) ==="

    STATUS_FILTER="${BANANA_FEEDBACK_STATUS_FILTER:-pending,pending-positive,approved}"
    NB_APPLY_REPORT="${ARTIFACTS_DIR}/not-banana-apply-${TIMESTAMP}.json"

    if [[ -f "${REPO_ROOT}/scripts/apply-not-banana-feedback.py" ]]; then
        "${PYTHON}" "${REPO_ROOT}/scripts/apply-not-banana-feedback.py" \
            --corpus "${REPO_ROOT}/data/not-banana/corpus.json" \
            --feedback "${BANANA_INBOX}" \
            --status-filter "${STATUS_FILTER}" \
            --report "${NB_APPLY_REPORT}" \
            --consume \
            --no-require-human-review \
            --minimum-human-reviewers 1
    else
        echo "  (apply-not-banana-feedback.py not found; skipping ingestion)"
    fi

    if [[ "${AUTO_TRAIN}" == "true" ]]; then
        echo ""
        echo "=== Retraining models (autonomous mode) ==="

        TRAIN_PROFILE="${BANANA_TRAINING_PROFILE:-ci}"

        if [[ -f "${REPO_ROOT}/scripts/train-banana-model.py" ]]; then
            "${PYTHON}" "${REPO_ROOT}/scripts/train-banana-model.py" \
                --corpus "${REPO_ROOT}/data/banana/corpus.json" \
                --output "${REPO_ROOT}/artifacts/training/banana/local" \
                --training-profile "${TRAIN_PROFILE}" \
                --session-mode single \
                --max-sessions 1
        else
            echo "  (train-banana-model.py not found; skipping banana training)"
        fi

        if [[ -f "${REPO_ROOT}/scripts/score-banana-generalization-benchmark.py" ]]; then
            "${PYTHON}" "${REPO_ROOT}/scripts/score-banana-generalization-benchmark.py" \
                --benchmark "${REPO_ROOT}/data/banana/generalization-benchmark.json" \
                --vocabulary "${REPO_ROOT}/artifacts/training/banana/local/vocabulary.json" \
                --report "${REPO_ROOT}/artifacts/training/banana/local/generalization-benchmark.json" \
                --strict
        fi

        if [[ -f "${REPO_ROOT}/scripts/score-banana-explanations.py" ]]; then
            "${PYTHON}" "${REPO_ROOT}/scripts/score-banana-explanations.py" \
                --rubric "${REPO_ROOT}/data/banana/explanation-rubric.json" \
                --prompts "${REPO_ROOT}/data/banana/explanation-prompts.json" \
                --report "${REPO_ROOT}/artifacts/training/banana/local/explanation-quality.json" \
                --strict
        fi

        REPLAY_PACK="${REPO_ROOT}/data/not-banana/replay/contrastive-replay-pack.json"
        if [[ -f "${REPO_ROOT}/scripts/build-contrastive-replay-pack.py" ]]; then
            "${PYTHON}" "${REPO_ROOT}/scripts/build-contrastive-replay-pack.py" \
                --feedback "${BANANA_INBOX}" \
                --output "${REPLAY_PACK}" \
                --max-entries 50
        fi

        if [[ -f "${REPO_ROOT}/scripts/train-not-banana-model.py" ]]; then
            "${PYTHON}" "${REPO_ROOT}/scripts/train-not-banana-model.py" \
                --corpus "${REPO_ROOT}/data/not-banana/corpus.json" \
                --output "${REPO_ROOT}/artifacts/training/not-banana/local" \
                --training-profile "${TRAIN_PROFILE}" \
                --session-mode single \
                --max-sessions 1 \
                --replay-pack "${REPLAY_PACK}"
        else
            echo "  (train-not-banana-model.py not found; skipping not-banana training)"
        fi

        if [[ -f "${REPO_ROOT}/scripts/score-contrastive-negatives.py" ]]; then
            "${PYTHON}" "${REPO_ROOT}/scripts/score-contrastive-negatives.py" \
                --dataset "${REPO_ROOT}/data/not-banana/hard-negatives.json" \
                --vocabulary "${REPO_ROOT}/artifacts/training/not-banana/local/vocabulary.json" \
                --report "${REPO_ROOT}/artifacts/training/not-banana/local/contrastive-negatives.json" \
                --strict
        fi

        if [[ -f "${REPO_ROOT}/scripts/train-ripeness-model.py" ]]; then
            "${PYTHON}" "${REPO_ROOT}/scripts/train-ripeness-model.py" \
                --corpus "${REPO_ROOT}/data/ripeness/corpus.json" \
                --output "${REPO_ROOT}/artifacts/training/ripeness/local" \
                --training-profile "${TRAIN_PROFILE}" \
                --session-mode single \
                --max-sessions 1
        else
            echo "  (train-ripeness-model.py not found; skipping ripeness training)"
        fi

        CURRICULUM_INGEST="${BANANA_CURRICULUM_INGEST_STAGED:-false}"
        if [[ "${CURRICULUM_INGEST}" == "true" ]]; then
            echo ""
            echo "=== Autonomous curriculum expansion (staged ingest) ==="
            if [[ -f "${REPO_ROOT}/scripts/mine-banana-knowledge-gaps.py" ]]; then
                "${PYTHON}" "${REPO_ROOT}/scripts/mine-banana-knowledge-gaps.py" \
                    --ontology "${REPO_ROOT}/data/banana/ontology-foundation.json" \
                    --coverage-report "${REPO_ROOT}/artifacts/training/banana/local/concept-coverage-summary.json" \
                    --generalization-report "${REPO_ROOT}/artifacts/training/banana/local/generalization-benchmark.json" \
                    --output "${REPO_ROOT}/artifacts/training/banana/local/knowledge-gaps.json"
            fi

            if [[ -f "${REPO_ROOT}/scripts/generate-banana-curriculum-candidates.py" ]]; then
                "${PYTHON}" "${REPO_ROOT}/scripts/generate-banana-curriculum-candidates.py" \
                    --gaps "${REPO_ROOT}/artifacts/training/banana/local/knowledge-gaps.json" \
                    --staging "${REPO_ROOT}/data/banana/curriculum/staging.json" \
                    --max-new 10
            fi

            if [[ -f "${REPO_ROOT}/scripts/ingest-banana-curriculum-staging.py" ]]; then
                "${PYTHON}" "${REPO_ROOT}/scripts/ingest-banana-curriculum-staging.py" \
                    --corpus "${REPO_ROOT}/data/banana/corpus.json" \
                    --staging "${REPO_ROOT}/data/banana/curriculum/staging.json" \
                    --max-accept 10 \
                    --report "${REPO_ROOT}/artifacts/training/banana/local/curriculum-ingest-summary.json"
            fi
        fi
    fi
fi

exit "${EXIT_CODE}"
