<!-- breadcrumb: Getting Started > Repository Layout -->

# Repository Layout

> [Home](../Home.md) › [Getting Started](README.md) › Repository Layout

Related pages: [First Day Checklist](first-day-checklist.md), [Architecture Overview](../architecture/overview.md)

This page shows the current high-level project layout.

```text
.github/
  agents/
  instructions/
  prompts/
  skills/
  workflows/

.specify/
  specs/
  wiki/
    human-reference/      ← AI-consumable mirror of .wiki/ human sections
    _templates/           ← Page templates

.wiki/                    ← Human-facing wiki (section-based layout)
  Home.md
  getting-started/
  architecture/
  operations/
  security/
  data/
  governance/
  ai-reference/           ← Auto-generated AI audit content
  _templates/             ← Page templates

docs/
  developer-onboarding.md
  ai-customization.md
  banana-ddd.md

src/
  native/
    core/
      domain/
      dal/
        domain/
        internal/
    wrapper/
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
    shared/
      ui/

tests/
  native/
  unit/
  integration/

scripts/
  launch-container-channels-with-wsl2-mobile.sh
  compose-mobile-emulators-wsl2.sh
  wiki-scaffold.sh
  wiki-consume-into-specify.sh

docker/
```

## Notes

- `.wiki/` is gitignored at root but individual human-section pages are tracked with `git add -f`.
- `.specify/wiki/human-reference/` is the tracked AI-consumable mirror; this is what CI and agents consume.
- Native DAL lives under `src/native/core/dal` — there is no top-level `src/native/dal`.
- The AI customization layer in `.github` is first-class project infrastructure.
- Mobile runtime channels are launched from Windows into Ubuntu WSL2; Android emulator is supported on Ubuntu, while iOS uses web-preview fallback on Ubuntu.
