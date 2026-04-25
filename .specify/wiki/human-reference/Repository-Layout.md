# Repository Layout

> Read the [Wiki Home](Home.md) for more details.

Related pages: [First Day Checklist](First-Day-Checklist.md), [Architecture Diagrams](Architecture-Diagrams.md)

This page shows the current high-level project layout.

```text
.github/
  agents/
    mobile-runtime-agent.agent.md
  instructions/
    mobile-runtime.instructions.md
  prompts/
    implement-banana-mobile-runtime-change.prompt.md
    debug-banana-mobile-runtime.prompt.md
  skills/
    banana-mobile-runtime/

docs/
  developer-onboarding.md
  ai-customization.md
  banana-ddd.md
  native-domain-migration-plan.md

src/
  native/
    core/
      domain/
      dal/
        domain/
        internal/
    wrapper/
      domain/
      dal/
    testing/
  c-sharp/
    asp.net/
      Controllers/
      DataAccess/
      Middleware/
      Models/
      NativeInterop/
      Pipeline/
        Steps/
      Services/
  typescript/
    react/
      src/
    electron/
    react-native/

tests/
  native/
  unit/
  integration/

scripts/
  launch-container-channels-with-wsl2-mobile.sh
  compose-mobile-emulators-wsl2.sh
docker/

.wiki/
  Home.md
  First-Day-Checklist.md
  How-A-Request-Works.md
  How-The-Database-Step-Works.md
  Architecture-Diagrams.md
  Build-Run-Test-Commands.md
  WSL2-Mobile-Runtime-Channels.md
  CI-Compose-Notes.md
  Why-We-Use-A-Wrapper.md
  AI-Customization-Map.md
```

## Notes

- `.wiki` is a separate git repository that tracks project wiki pages.
- Native code no longer uses a top-level `src/native/dal` folder. DAL now lives under `src/native/core/dal`.
- The AI customization layer in `.github` is first-class project infrastructure, not optional side docs.
- Mobile runtime channels are launched from Windows into Ubuntu WSL2; Android emulator is supported on Ubuntu, while iOS uses web-preview fallback on Ubuntu.
