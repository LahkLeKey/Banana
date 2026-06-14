import {defineConfig} from '@playwright/test';

export default defineConfig({
  testDir: './tests/playwright',
  timeout: 45_000,
  expect: {
    timeout: 20_000,
  },
  workers: 1,
  retries: 0,
  reporter: 'line',
  use: {
    baseURL: process.env.PLAYWRIGHT_BASE_URL ?? 'http://localhost:5173',
    trace: 'retain-on-failure',
  },
});
