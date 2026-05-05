#!/usr/bin/env bash
# run-playwright-training-suites.sh
#
# Runs Playwright reinforcement training suites and emits a JSON session
# summary to artifacts/playwright-training/.
#
# Usage:
#   bash scripts/run-playwright-training-suites.sh [--mode live|mock] [--apply] [--suite banana|ripeness|all]
#
# Options:
#   --mode live    Use the live API (requires runtime stack to be up)
#   --mode mock    Stub all API calls (default; suitable for CI)
#   --apply        Immediately invoke the feedback apply pipeline after suites
#                  complete (calls scripts/orchestrate-not-banana-feedback-loop.sh
#                  and equivalent for ripeness if present)
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
APPLY=false
SUITE="all"

# ── Arg parse ─────────────────────────────────────────────────────────────

while [[ $# -gt 0 ]]; do
    case "$1" in
        --mode)
            MODE="$2"; shift 2 ;;
        --apply)
            APPLY=true; shift ;;
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
echo "  output : ${SESSION_FILE}"
echo ""

export BANANA_TRAINING_MODE="${MODE}"

cd "${REPO_ROOT}"

EXIT_CODE=0
bun run --cwd src/typescript/react \
    exec playwright test \
    --config="${PLAYWRIGHT_CONFIG}" \
    ${SPEC_PATTERN} \
    --reporter=json \
    2>&1 | tee /tmp/pw-reinforcement-output.txt || EXIT_CODE=$?

# ── Emit session summary JSON ─────────────────────────────────────────────

BANANA_INBOX="${REPO_ROOT}/data/not-banana/feedback/inbox.json"
RIPENESS_INBOX="${REPO_ROOT}/data/ripeness/feedback/inbox.json"

banana_count=0
ripeness_count=0

if [[ -f "${BANANA_INBOX}" ]]; then
    banana_count=$(python3 -c "import json,sys; d=json.load(open('${BANANA_INBOX}')); print(sum(1 for e in d if e.get('source')=='playwright-reinforcement'))" 2>/dev/null || echo 0)
fi
if [[ -f "${RIPENESS_INBOX}" ]]; then
    ripeness_count=$(python3 -c "import json,sys; d=json.load(open('${RIPENESS_INBOX}')); print(sum(1 for e in d if e.get('source')=='playwright-reinforcement'))" 2>/dev/null || echo 0)
fi

python3 - <<PY
import json, datetime

summary = {
    "session_id": "pw-${TIMESTAMP}",
    "mode": "${MODE}",
    "suite": "${SUITE}",
    "exit_code": ${EXIT_CODE},
    "banana_feedback_count": ${banana_count},
    "ripeness_feedback_count": ${ripeness_count},
    "generated_at_utc": datetime.datetime.utcnow().isoformat() + "Z",
}
with open("${SESSION_FILE}", "w") as f:
    json.dump(summary, f, indent=2)
    f.write("\n")
print(json.dumps(summary, indent=2))
PY

echo ""
echo "=== Session summary written to: ${SESSION_FILE} ==="

# ── Optional apply ────────────────────────────────────────────────────────

if [[ "${APPLY}" == "true" && "${EXIT_CODE}" == "0" ]]; then
    echo ""
    echo "=== Applying not-banana feedback loop ==="
    if [[ -f "${REPO_ROOT}/scripts/orchestrate-not-banana-feedback-loop.sh" ]]; then
        bash "${REPO_ROOT}/scripts/orchestrate-not-banana-feedback-loop.sh"
    else
        echo "  (orchestrate-not-banana-feedback-loop.sh not found — skipping)"
    fi
fi

exit "${EXIT_CODE}"
