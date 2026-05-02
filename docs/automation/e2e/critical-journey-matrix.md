# Critical Journey Matrix Specification

Parent: #313
Epic: #305
Related implementation slices: #371, #372, #344, #373, #367

## Purpose

Define the approved critical user journeys, expected outcomes, and deterministic automation traceability for Banana runtime coverage.

Until the dedicated compose-driven E2E harness from #312 is in place, these journeys are executed through the existing HTTP integration harness in `tests/integration/BananaPipelineIntegrationTests.cs`.

Issue #367 adds the first dedicated E2E runner contracts scaffold under `tests/e2e`, which now defines the scenario contract IDs, default runtime settings, and baseline contract validation rules for these same journeys.

## Deterministic Fixture Contract

- Scenario host uses `CreateFactoryWithFakeNative()` to isolate each test run from external native and database variability.
- Scenario payloads use explicit IDs and fixed telemetry values.
- Journey assertions validate exact HTTP statuses and key contract fields.
- State reset between scenarios is enforced with fresh scenario hosts per test and validated by `DataResetE2E_CJ007_BatchState_DoesNotLeakAcrossScenarioHosts`.
- The compose-driven E2E harness in #312 should preserve this reset behavior as an explicit contract.

## Journey Matrix

| Journey ID | Journey | Runtime Surface | Deterministic Preconditions | Expected Outcome | Automated Scenario | Status |
| --- | --- | --- | --- | --- | --- | --- |
| CJ001 | Banana runtime console core success flow | `/health` -> `/banana` -> `/batches/create` -> `/batches/{id}/status` -> `/ripeness/predict` | Fresh in-memory host via `CreateFactoryWithFakeNative()`; fixed payload values | Health returns 200; banana payload computes deterministically; batch status returns persisted contract; ripeness returns `YELLOW` with fixed shelf life | `CoreSuccessE2E_CJ001_BananaConsoleJourney_ReturnsExpectedPayloads` | Implemented (#372) |
| CJ002 | Not-banana polymorphic classification success | `/not-banana/junk` | Fresh in-memory host; actor/entity/junk payload includes deterministic banana-signal tokens | Response is 200 with normalized actor/entity payload and `MAYBE_BANANA` classification | `CoreSuccessE2E_CJ002_NotBananaJourney_ReturnsPolymorphicClassification` | Implemented (#372) |
| CJ003 | Input validation failure handling | `/banana` | Fresh in-memory host; invalid query values (`purchases < 0`) | Response is 400 with client-safe error payload | `GetBanana_WhenValidationFails_ReturnsBadRequest` | Implemented (existing) |
| CJ004 | Native interop failure handling | `/banana` | Fresh in-memory host; `multiplier = 777` to inject native failure | Response is 500 and error middleware returns native-failure contract | `GetBanana_WhenNativeStepThrowsNativeInteropException_ReturnsInternalServerError` | Implemented (existing) |
| CJ005 | Data-access failure handling | `/banana` | Host configured with `CreateFactoryWithFailingDatabase()` | Response is 503 and error middleware returns database-failure contract | `GetBanana_WhenDatabaseStepThrowsDatabaseAccessException_ReturnsServiceUnavailable` | Implemented (existing) |
| CJ006 | Failure then recovery within same runtime session | `/banana` | Single client performs one injected failure request followed by a valid request | First response indicates native failure; second response succeeds without host reset | `FailureRecoveryE2E_CJ006_BananaPipeline_RecoversAfterNativeFailureInSameSession` | Implemented (#344) |
| CJ007 | Deterministic scenario reset between runs | `/batches/create` -> `/batches/{id}/status` | First scenario creates a batch; second scenario uses a fresh host and requests the same batch ID | Scenario one returns 200; scenario two returns 404, proving no state leakage | `DataResetE2E_CJ007_BatchState_DoesNotLeakAcrossScenarioHosts` | Implemented (#373) |

## Traceability Rules

- Every new critical journey must receive a stable `CJ###` ID before implementation.
- Every automated scenario must include the journey ID in its test method name.
- Matrix status must be updated in the same change set as scenario implementation.
- `Planned` entries are not eligible for 100% critical-journey completeness until they reach `Implemented`.
