# Panels Module Packages

This folder contains publish-ready thin wrapper packages for individual panel modules.

Each module package:

- Depends on `@banana/panels` for implementation source.
- Declares its own `tailwindcss` dependency for primitive orchestration.
- Can be published independently for package-manager installs.

Examples:

- `@banana/panels-resizable-panel`
- `@banana/panels-resizable-dock-grid`
- `@banana/panels-styles`
- `@banana/panels-interactions`
- `@banana/panels-panel-base`
- `@banana/panels-panel-overlay`
- `@banana/panels-panel-pipeline`
- `@banana/panels-panel-variant-pipeline`
- `@banana/panels-dock-layout-store`
