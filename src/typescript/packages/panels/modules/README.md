# @banana/panels Module Packages

Public npm modules for teams that want to consume only specific panel capabilities.

## Install Pattern

Install a module package with the core dependency:

```bash
npm install @banana/panels-<module> @banana/panels react react-dom
```

## Module Catalog

| Module Package | Import |
| --- | --- |
| `@banana/panels-chrome` | `import {...} from '@banana/panels-chrome'` |
| `@banana/panels-dock-layout-store` | `import {...} from '@banana/panels-dock-layout-store'` |
| `@banana/panels-drag-snap` | `import {...} from '@banana/panels-drag-snap'` |
| `@banana/panels-header` | `import {...} from '@banana/panels-header'` |
| `@banana/panels-host` | `import {...} from '@banana/panels-host'` |
| `@banana/panels-interactions` | `import {...} from '@banana/panels-interactions'` |
| `@banana/panels-panel-base` | `import {...} from '@banana/panels-panel-base'` |
| `@banana/panels-panel-overlay` | `import {...} from '@banana/panels-panel-overlay'` |
| `@banana/panels-panel-pipeline` | `import {...} from '@banana/panels-panel-pipeline'` |
| `@banana/panels-panel-variant-pipeline` | `import {...} from '@banana/panels-panel-variant-pipeline'` |
| `@banana/panels-raf-buffered-dispatch` | `import {...} from '@banana/panels-raf-buffered-dispatch'` |
| `@banana/panels-refs` | `import {...} from '@banana/panels-refs'` |
| `@banana/panels-resizable-dock-grid` | `import {...} from '@banana/panels-resizable-dock-grid'` |
| `@banana/panels-resizable-panel` | `import {...} from '@banana/panels-resizable-panel'` |
| `@banana/panels-resize` | `import {...} from '@banana/panels-resize'` |
| `@banana/panels-stage` | `import {...} from '@banana/panels-stage'` |
| `@banana/panels-styles` | `import {...} from '@banana/panels-styles'` |
| `@banana/panels-view-model` | `import {...} from '@banana/panels-view-model'` |

## Example

```bash
npm install @banana/panels-panel-overlay @banana/panels react react-dom
```

```ts
import {PanelOverlay} from '@banana/panels-panel-overlay';
```

## Storybook Per Module (Maintainers)

Each module package includes local Storybook scaffolding:

- `.env.example`
- `.storybook/main.ts`
- `.storybook/preview.ts`
- `src/stories/__package.stories.tsx`

From a module package directory (for example `modules/panel-base`):

```bash
cp .env.example .env
bun run storybook
```

Build static docs:

```bash
bun run build-storybook
```
