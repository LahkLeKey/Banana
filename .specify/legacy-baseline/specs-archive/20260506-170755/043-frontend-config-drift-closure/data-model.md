# Data Model: Frontend Config Drift Closure

## Entity: FrontendApiBaseContract

- Description: Canonical mapping between frontend slice, env source, fallback order, and expected effective runtime value.
- Fields:
  - `slice` (enum): `react-web` | `electron-desktop` | `react-native-web`
  - `primaryEnvKey` (string)
  - `overrideEnvKey` (string)
  - `defaultValue` (string URL)
  - `resolutionOrder` (string[])
  - `expectedRenderedValue` (string URL or pattern)

## Entity: LaunchProfileContract

- Description: Declares required startup sequencing for each VS Code frontend launch profile.
- Fields:
  - `launchName` (string)
  - `preLaunchTask` (string)
  - `requiresRuntimeReady` (boolean)
  - `sliceProfileTask` (string)
  - `postDebugTask` (string)

## Entity: DriftDiagnosticReport

- Description: Structured output from drift diagnostics.
- Fields:
  - `timestamp` (ISO string)
  - `slice` (enum)
  - `layerResults` (object):
    - `launchPath` (pass/fail + details)
    - `composeInterpolation` (pass/fail + details)
    - `containerEnv` (pass/fail + details)
    - `effectiveRuntime` (pass/fail + details)
  - `overallStatus` (pass/fail)
  - `recommendedRemediation` (string[])

## Relationships

- `LaunchProfileContract` references one `FrontendApiBaseContract` per slice.
- `DriftDiagnosticReport` evaluates one `FrontendApiBaseContract` instance at runtime and records per-layer outcomes.
