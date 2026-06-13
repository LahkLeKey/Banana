# API Evidence Index

This directory stores API validation evidence artifacts by feature.

## Feature 035: Native Hypersphere K-Means

Capture API and React presentation-consumer evidence under:

- `035-native-hypersphere-kmeans/<timestamp>/api-contract.log`
- `035-native-hypersphere-kmeans/<timestamp>/react-consumer.log`

Recommended commands:

```bash
cd src/typescript/api
bun test src/routes/netcode.contract.test.ts src/routes/netcode.integration.test.ts src/services/nativeNetcode.fail-fast.test.ts | tee ../../../artifacts/api/035-native-hypersphere-kmeans/<timestamp>/api-contract.log

cd ../react
bun test src/domain/notebook/useNetcodeSession.test.ts src/domain/notebook/network-domain.test.ts | tee ../../../artifacts/api/035-native-hypersphere-kmeans/<timestamp>/react-consumer.log
```

## Naming Guidance

Use `YYYYMMDD-HHMMSS` timestamps and keep each run self-contained with command output.
