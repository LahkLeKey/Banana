import {expect, test} from '@playwright/test';

test('API health check', async ({request}) => {
  const res = await request.get(`${process.env.API_BASE_URL ?? 'http://localhost:8080'}/health`);
  expect(res.status()).toBe(200);
  const body = await res.json();
  expect(body.status).toBe('ok');
});

test('TS API health check', async ({request}) => {
  const res = await request.get(`${process.env.TS_API_BASE_URL ?? 'http://localhost:8081'}/health`);
  expect(res.status()).toBe(200);
});
