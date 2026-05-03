# Tasks: API Rate Limit Verification and Testing (149)

**Input**: Spec at `.specify/specs/149-api-rate-limit-verification/spec.md`
**Prerequisites**: Spec 114 (rate limit middleware deployed)

- [ ] T001 Create `tests/unit/rate-limit.test.ts` with test suite for rate limit behavior.
- [ ] T002 Test: send 100 requests from same IP; verify 429 response after limit threshold.
- [ ] T003 Test: different endpoints have independent rate limits (if configured).
- [ ] T004 Test: rate limit resets after configured window (e.g., 60 sec).
- [ ] T005 Run `npm test -- tests/unit/rate-limit.test.ts` and verify all tests pass.
- [ ] T006 Add CI step in `.github/workflows/api-tests.yml`: rate limit test must pass before merge.
- [ ] T007 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/149-api-rate-limit-verification` and confirm `OK`.
