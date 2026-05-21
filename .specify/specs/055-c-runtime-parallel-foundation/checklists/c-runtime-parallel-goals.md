# C Runtime Parallel Goals Checklist: C Runtime Parallel Foundation

**Purpose**: Track architecture and requirement quality for parallel tick decomposition and C-first runtime ownership.
**Created**: 2026-05-19
**Feature**: ../spec.md

## Tick Architecture

- [ ] CHK001 Tick pipeline is decomposed into explicit C stage boundaries (pre-update, simulation, controller sync, render submission)
- [ ] CHK002 Each stage documents thread-safety assumptions and shared-state mutation rules
- [ ] CHK003 Parallelization candidates are identified with fallback serial behavior defined
- [ ] CHK004 Deterministic ordering guarantees are specified for all gameplay-impacting stages

## C-First Runtime Ownership

- [ ] CHK005 TypeScript responsibilities are reduced to transport/UI glue and documented explicitly
- [ ] CHK006 Native C APIs own remote player reconciliation lifecycle (activate, update, stale/deactivate)
- [ ] CHK007 Native APIs expose required observability for active player entities and sync status
- [ ] CHK008 Removed TypeScript simulation logic is mapped to replacement C ownership points

## DDD and SOLID Quality

- [ ] CHK009 Native modules are split by bounded domain responsibility instead of file-size growth
- [ ] CHK010 Domain policy code is separated from adapter/interop glue code
- [ ] CHK011 Single Responsibility is enforced for each extracted C module and header surface
- [ ] CHK012 Dependency direction keeps higher-level orchestration independent from low-level adapter details

## Renderer-Agnostic Boundaries

- [ ] CHK013 Simulation/state contracts avoid renderer-specific branches for core tick logic
- [ ] CHK014 Renderer integration is specified as adapter behavior consuming native state output
- [ ] CHK015 Future renderer migration risks are listed with mitigation checkpoints

## Validation And Delivery

- [ ] CHK016 Native configure/build and relevant tests are listed as required gates
- [ ] CHK017 Two-client authoritative runtime validation criteria are documented and measurable
- [ ] CHK018 Native coverage run (`scripts/run-native-c-tests-with-coverage.sh`) is captured with pass/fail evidence
- [ ] CHK019 Spec artifacts include traceability from user stories to requirements and checklist items
- [ ] CHK020 Heartbeat/evidence updates are captured for each major migration step

## Notes

- Mark items complete with [x] as implementation lands.
- Keep this checklist updated when scope or ownership boundaries change.
