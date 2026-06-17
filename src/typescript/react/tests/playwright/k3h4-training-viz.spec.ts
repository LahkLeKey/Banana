import {expect, test} from '@playwright/test';

/**
 * K3H4 training integration smoke — covers the three new endpoints added in
 * the 036-k3h4-transformer-viz training integration feature:
 *
 *   POST /api/netcode/k3h4/training-session
 *   GET  /api/netcode/k3h4/training-session/:id/confidence
 *   GET  /api/netcode/k3h4/training-session/:id/epoch/:n/geometry
 *
 * These tests run against the live compose stack (api-overworld on :8081)
 * and the React shell (react-overworld on :5173).
 */

const API_BASE = process.env.PLAYWRIGHT_API_BASE_URL ?? 'http://localhost:8081';

test.describe('K3H4 training integration API', () => {
  test(
      'POST training-session creates a session with default power mode',
      async ({request}) => {
        const res = await request.post(
            `${API_BASE}/api/netcode/k3h4/training-session`, {
              headers: {'content-type': 'application/json'},
              data: {},
            });

        expect(res.status(), 'training-session must return 201').toBe(201);
        const json = await res.json() as {
          sessionId?: string;
          mode?: string;
          createdAtUtc?: string;
        };
        expect(json.sessionId).toBeTruthy();
        expect(json.mode).toBe('power');
        expect(typeof json.createdAtUtc).toBe('string');
      });

  test(
      'POST training-session accepts explicit multiplicative mode',
      async ({request}) => {
        const res = await request.post(
            `${API_BASE}/api/netcode/k3h4/training-session`, {
              headers: {'content-type': 'application/json'},
              data: {mode: 'multiplicative'},
            });

        expect(res.status()).toBe(201);
        const json = await res.json() as {mode?: string};
        expect(json.mode).toBe('multiplicative');
      });

  test('POST training-session rejects invalid mode', async ({request}) => {
    const res =
        await request.post(`${API_BASE}/api/netcode/k3h4/training-session`, {
          headers: {'content-type': 'application/json'},
          data: {mode: 'turbo'},
        });
    expect(res.status()).toBe(400);
    const json = await res.json() as {error?: string};
    expect(json.error).toBe('invalid_mode');
  });

  test(
      'GET confidence returns pending status for fresh session',
      async ({request}) => {
        // Create a fresh session first.
        const createRes = await request.post(
            `${API_BASE}/api/netcode/k3h4/training-session`, {
              headers: {'content-type': 'application/json'},
              data: {mode: 'power'},
            });
        expect(createRes.status()).toBe(201);
        const {sessionId} = await createRes.json() as {sessionId: string};

        const confRes =
            await request.get(`${API_BASE}/api/netcode/k3h4/training-session/${
                sessionId}/confidence`);
        expect(confRes.status()).toBe(200);
        const conf = await confRes.json() as {
          contractVersion?: number;
          sessionId?: string;
          status?: string;
          mode?: string;
          epochs?: unknown[];
        };
        expect(conf.contractVersion).toBe(1);
        expect(conf.sessionId).toBe(sessionId);
        expect(conf.status).toBe('pending');
        expect(conf.mode).toBe('power');
        expect(Array.isArray(conf.epochs)).toBe(true);
        expect(conf.epochs).toHaveLength(0);
      });

  test('GET confidence returns 404 for unknown session', async ({request}) => {
    // Session ID must be ≤ 15 chars and match [A-Za-z0-9_-]+
    const res = await request.get(`${
        API_BASE}/api/netcode/k3h4/training-session/no-sess-ghost/confidence`);
    expect(res.status()).toBe(404);
    const json = await res.json() as {error?: string};
    expect(json.error).toBe('session_not_found');
  });

  test('GET confidence rejects invalid mode param', async ({request}) => {
    const createRes =
        await request.post(`${API_BASE}/api/netcode/k3h4/training-session`, {
          headers: {'content-type': 'application/json'},
          data: {},
        });
    const {sessionId} = await createRes.json() as {sessionId: string};

    const res =
        await request.get(`${API_BASE}/api/netcode/k3h4/training-session/${
            sessionId}/confidence?mode=bad`);
    expect(res.status()).toBe(400);
    const json = await res.json() as {error?: string};
    expect(json.error).toBe('invalid_mode');
  });

  test(
      'GET epoch geometry returns 404 for missing artifact',
      async ({request}) => {
        const createRes = await request.post(
            `${API_BASE}/api/netcode/k3h4/training-session`, {
              headers: {'content-type': 'application/json'},
              data: {},
            });
        const {sessionId} = await createRes.json() as {sessionId: string};

        const res =
            await request.get(`${API_BASE}/api/netcode/k3h4/training-session/${
                sessionId}/epoch/0/geometry`);
        expect(res.status()).toBe(404);
        const json = await res.json() as {error?: string};
        expect(json.error).toBe('artifact_not_found');
      });

  test('GET epoch geometry returns 404 for unknown session', async ({request}) => {
    // Session ID must be ≤ 15 chars
    const res = await request.get(`${
        API_BASE}/api/netcode/k3h4/training-session/no-sess-geo99/epoch/0/geometry`);
    expect(res.status()).toBe(404);
    const json = await res.json() as {error?: string};
    expect(json.error).toBe('session_not_found');
  });

  test('GET epoch geometry rejects invalid epoch index', async ({request}) => {
    const res = await request.get(`${
        API_BASE}/api/netcode/k3h4/training-session/sess01/epoch/nan/geometry`);
    expect(res.status()).toBe(400);
    const json = await res.json() as {error?: string};
    expect(json.error).toBe('invalid_epoch');
  });

  test('GET scaling-benchmark returns series data', async ({request}) => {
    const res =
        await request.get(`${API_BASE}/api/netcode/k3h4/scaling-benchmark`);
    // The CTest benchmark may not have run in a fresh compose stack, so both
    // 200 (data available) and 404 (not yet generated) are acceptable.
    expect([200, 404]).toContain(res.status());
    if (res.status() === 200) {
      const json = await res.json() as {
        contractVersion?: number;
        series?: unknown[];
      };
      expect(json.contractVersion).toBe(1);
      expect(Array.isArray(json.series)).toBe(true);
      expect((json.series ?? []).length).toBeGreaterThanOrEqual(1);
    }
  });
});

test.describe('K3H4 training UI presence', () => {
  test('React shell loads without transport errors', async ({page}) => {
    await page.goto('/');

    await expect
        .poll(
            async () => {
              const bodyText = await page.locator('body').innerText();
              return {
                transportError: bodyText.includes('TRANSPORT-ERROR'),
                analyticsUnavailable:
                    bodyText.includes('ANALYTICS UNAVAILABLE'),
              };
            },
            {timeout: 30_000, intervals: [1_000, 2_000, 3_000]},
            )
        .toMatchObject({
          transportError: false,
          analyticsUnavailable: false,
        });
  });

  test(
      'default pinned panels are minimal — shell renders without node panels',
      async ({page}) => {
        await page.goto('/');

        // Wait for the shell to stabilise.
        await page.waitForLoadState('networkidle', {timeout: 20_000})
            .catch(() => {/* tolerate slow hydration */});

        // All panels are pinned, but only the visualizations window is shown
        // by default. Node sub-panels start hidden and are opened on demand.
        // Verify no transport or unavailability errors surface.
        const bodyText = await page.locator('body').innerText();
        expect(
            bodyText.includes('TRANSPORT-ERROR'),
            'no transport error when node panels are closed by default')
            .toBe(false);
        expect(
            bodyText.includes('ANALYTICS UNAVAILABLE'),
            'analytics not unavailable when node panels are closed')
            .toBe(false);
      });
});
