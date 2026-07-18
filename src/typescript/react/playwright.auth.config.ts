import {defineConfig} from '@playwright/test';

export default defineConfig({
  testDir: './tests/playwright',
  testMatch: ['login-auth-entry.spec.ts'],
  timeout: 45_000,
  expect: {
    timeout: 20_000,
  },
  workers: 1,
  retries: 0,
  reporter: 'line',
  use: {
    baseURL: 'http://127.0.0.1:4173',
    trace: 'retain-on-failure',
  },
  webServer: {
    command: 'bun run dev -- --host 127.0.0.1 --port 4173',
    url: 'http://127.0.0.1:4173/login',
    reuseExistingServer: true,
    timeout: 120_000,
  },
});