import {defineConfig} from '@playwright/test';

export default defineConfig({
  testDir: './tests/synthetic/specs',
  timeout: 30_000,
  retries: 2,
  use: {
    baseURL: process.env.SYNTHETIC_BASE_URL ?? 'http://localhost:5173',
  },
});
