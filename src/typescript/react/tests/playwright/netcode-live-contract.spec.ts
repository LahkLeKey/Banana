import {expect, test} from '@playwright/test';

test(
    'frontend renders live netcode analytics without transport fallback',
    async ({page, request}) => {
      const apiBaseUrl =
          process.env.PLAYWRIGHT_API_BASE_URL ?? 'http://localhost:8081';

      const healthResponse = await request.get(`${apiBaseUrl}/health`);
      expect(healthResponse.status(), 'API health endpoint must be reachable')
          .toBe(200);

      const analyticsResponse =
          await request.post(`${apiBaseUrl}/api/netcode/analytics`, {
            headers: {'content-type': 'application/json'},
            data: {
              callDensity: 10,
              questPercent: 20,
              playerLevel: 30,
              comboStreak: 40,
              branchPressure: 50,
              dependencyPulse: 60,
              workflowDepth: 2,
              networkDimensions: 6,
              modelConfidence: 77,
              policyMomentum: 66,
              k3h4Mode: 'power',
              spectralMode: 'affinity-graph',
            },
          });
      expect(
          analyticsResponse.status(),
          'Netcode analytics endpoint must return success')
          .toBe(200);

      const analyticsJson = (await analyticsResponse.json()) as {
        contractVersion?: number;
        k3h4Runtime?: {mode?: string; spectralActivation?: string;};
        abiLayerCoverage?: {complete?: boolean;};
      };

      expect(analyticsJson.contractVersion).toBe(1);
      expect(analyticsJson.k3h4Runtime?.mode).toBe('power');
      expect(analyticsJson.k3h4Runtime?.spectralActivation)
          .toBe('affinity-graph');
      expect(analyticsJson.abiLayerCoverage?.complete).toBe(true);

      await page.goto('/');

      await expect
          .poll(
              async () => {
                const bodyText = await page.locator('body').innerText();
                return {
                  transportError: bodyText.includes('TRANSPORT-ERROR'),
                  analyticsUnavailable:
                      bodyText.includes('ANALYTICS UNAVAILABLE'),
                  hasAbiCoverage: bodyText.includes('ABI COVERAGE'),
                };
              },
              {
                timeout: 30_000,
                intervals: [1_000, 2_000, 3_000],
              })
          .toMatchObject({
            transportError: false,
            analyticsUnavailable: false,
            hasAbiCoverage: true,
          });
    });
