# Architecture Diagrams

> Read the [Wiki Home](Home.md) for more details.

Related pages: [How A Request Works](How-A-Request-Works.md), [Repository Layout](Repository-Layout.md)

This page centralizes the system and request-flow diagrams.

## System Architecture

```mermaid
flowchart LR
  Client[REST Clients] --> Api[ASP.NET API]
  React[React App] --> Api
  ReactNativeWeb[React Native Web App] --> Api
  Electron[Electron Smoke App] --> Api

  WindowsLauncher[Windows Launch Profiles] --> WslDesktop[scripts/launch-container-channels-with-wsl2-electron.sh]
  WindowsLauncher --> WslMobile[scripts/launch-container-channels-with-wsl2-mobile.sh]
  WslDesktop --> UbuntuDesktop[scripts/compose-electron-desktop-wsl2.sh]
  WslMobile --> UbuntuMobile[scripts/compose-mobile-emulators-wsl2.sh]
  UbuntuMobile --> AndroidEmulator[Android Emulator via WSLg]
  UbuntuMobile --> IosPreview[iOS Style Web Preview]

  Api --> Service[Controllers and Services]
  Service --> Pipeline[Pipeline Executor for GET /banana]
  Service --> Interop[Native Interop Client]
  Pipeline --> DataAccess[Data Access Step]
  Pipeline --> Interop

  DataAccess --> Managed[Managed Npgsql Path]
  DataAccess --> NativeDbPath[Native DAL Path]

  Interop --> Wrapper[Native Wrapper ABI]
  NativeDbPath --> Wrapper

  Wrapper --> CoreDomain[Native Core Domain]
  Wrapper --> CoreDal[Native Core DAL Domain]

  CoreDal --> Postgres[(PostgreSQL)]
  Managed --> Postgres

  Guidance[.github Agents, Skills, Prompts] -.workflow guidance.-> Service
```

## HTTP Route Flow

```mermaid
flowchart TD
  A[GET /banana] --> B[BananaController]
  B --> C[BananaService]
  C --> D[PipelineExecutor]
  D --> E[ValidationStep]
  E --> F[DatabaseAccessStep]
  F --> G[NativeCalculationStep]
  G --> H[PostProcessingStep]
  H --> I[AuditStep]

  J[POST /batches/create] --> K[BatchController]
  K --> L[BatchService]
  L --> M[INativeBananaClient]

  N["GET /batches/:id/status"] --> K

  O[POST /ripeness/predict] --> P[RipenessController]
  P --> Q[RipenessService]
  Q --> M

  G --> M
  M --> R[NativeMethods]
  R --> S[banana_wrapper exports]
```

## Native Domain Composition

```mermaid
flowchart TB
  Wrapper[Wrapper Modules] --> Profile[banana_profile and projections]
  Wrapper --> Processing[banana_processing]
  Wrapper --> SupplyChain[banana_supply_chain]
  Wrapper --> Logistics[banana_logistics]
  Wrapper --> Ripeness[banana_ripeness]

  Profile --> DalProfile[core dal profile projection]
  Processing --> CoreProcessing[core domain processing]
  SupplyChain --> CoreSupplyChain[core domain supply chain]
  Logistics --> CoreLogistics[core domain logistics]
  Ripeness --> CoreLifecycle[core domain lifecycle and ripeness]
```
