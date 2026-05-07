# Contract: Chat Bootstrap Runtime Failure Classification and Recovery

## Purpose

Define required behavior for frontend chat bootstrap when runtime/API transport failures occur, including classification boundaries and recovery expectations.

## Inputs and Dependencies

- Frontend runtime API base contract: `VITE_BANANA_API_BASE_URL`
- Runtime startup contract: compose `runtime` profile must be healthy before `apps`
- Verification surface: VS Code integrated browser rendered state

## Classification Contract

1. Bootstrap errors caused by endpoint reachability/fetch transport failures must be classified as `bootstrap_transport`.
2. Missing/invalid API-base resolution must be classified as `config_resolution`.
3. `bootstrap_transport` and `config_resolution` must not share the same user-facing remediation message.

## Recovery Contract

1. If runtime becomes unavailable after apps start, frontend may enter `bootstrap_transport` failed state.
2. After runtime is restored and readiness checks pass, frontend retry/reload must transition bootstrap back to `bootstrapping` and then `ready`.
3. Recovery must not require manual source edits or ad-hoc environment mutation.

## Validation Contract

1. Healthy path: runtime+apps up in canonical order yields bootstrap ready state and usable chat controls.
2. Failure path: runtime down while app is available yields transport-specific bootstrap failure messaging.
3. Recovery path: runtime restored and page retried/reloaded clears transport failure and restores chat readiness.

## Evidence Expectations

- Include startup order commands used.
- Include rendered-state observations for healthy, failure, and recovery phases.
- Include confirmation that configuration-resolution and transport failures were differentiated.
