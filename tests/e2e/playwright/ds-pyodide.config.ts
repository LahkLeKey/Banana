/**
 * Config moved to src/typescript/react/ds-pyodide.config.ts
 *
 * The config must live alongside the React package so that @playwright/test
 * resolves from src/typescript/react/node_modules.
 *
 * Run via (from src/typescript/react/):
 *   bun run test:ds-e2e
 */

// Re-export the canonical config so any tooling that still points here works.
export {default} from "../../../src/typescript/react/ds-pyodide.config";
