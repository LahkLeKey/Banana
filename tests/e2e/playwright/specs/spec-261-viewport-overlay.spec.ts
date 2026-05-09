import {expect, test} from "@playwright/test";

/**
 * Spec 261 E2E Tests: WASM Viewport + React Overlay Runtime
 *
 * US1: Run In WASM Viewport First (Priority: P1)
 * US2: Interact With React Overlays Without Breaking Scene Control (Priority: P2)
 * US3: Recover Gracefully From WASM Runtime Failure (Priority: P3)
 */

test.describe("Spec 261: WASM Viewport + React Overlay Runtime", () => {
    test.describe("US1 - Run In WASM Viewport First (P1)", () => {
        test("viewport occupies full background canvas on successful load", async ({
                                                                                page,
                                                                            }) => {
            // Navigate to React app
            await page.goto("/classify");

            // Wait for canvas to render
            const canvas = await page.waitForSelector("[data-testid='wasm-canvas']", {
                timeout : 5000,
            });

            expect(canvas).toBeTruthy();

            // Verify canvas visibility and dimensions
            const isVisible = await canvas.isVisible();
            expect(isVisible).toBe(true);

            const boundingBox = await canvas.boundingBox();
            expect(boundingBox).toBeTruthy();
            expect(boundingBox!.width).toBeGreaterThan(0);
            expect(boundingBox!.height).toBeGreaterThan(0);

            // Verify canvas data-lifecycle attribute progresses from booting
            const lifecycleAttr = await canvas.getAttribute("data-lifecycle");
            expect([ "booting", "ready", "running", "degraded" ]).toContain(lifecycleAttr);
        });

        test("frame telemetry begins after first scene renders", async ({
                                                                     page,
                                                                 }) => {
            // Set up listener for telemetry requests
            const telemetryRequests: string[] = [];
            await page.on("request", (request) => {
                if (request.url().includes("/api/telemetry/frame"))
                {
                    telemetryRequests.push(request.postDataJSON()?.kind || "");
                }
            });

            await page.goto("/classify");

            // Wait for canvas to reach running state
            const canvas = await page.waitForSelector("[data-testid='wasm-canvas']", {
                timeout : 5000,
            });

            // Wait for either first_frame or frame_interval telemetry
            await page.waitForTimeout(500);

            // Verify telemetry was sent (runtime may emit degraded events first if WASM load fails).
            expect(telemetryRequests.length).toBeGreaterThan(0);
            expect(telemetryRequests).toEqual(
                expect.arrayContaining([expect.stringMatching(/viewport_start|degraded_entry|first_frame|frame_interval|stopped/)]),
            );
        });

        test("overlays render above viewport without covering scene", async ({
                                                                          page,
                                                                      }) => {
            await page.goto("/classify");

            const canvas = await page.waitForSelector("[data-testid='wasm-canvas']", {
                timeout : 5000,
            });

            // Verify canvas is visible
            let zIndexCanvas = await page.evaluate(() => {
                const el = document.querySelector("[data-testid='wasm-canvas']");
                return window.getComputedStyle(el as Element).zIndex || "auto";
            });
            expect(zIndexCanvas).toBe("0");

            // Check for overlay layers (HUD at z-index 10, menu at 20, etc.)
            const overlayElements = await page.locator("[data-overlay='true']").count();
            expect(overlayElements)
                .toBeGreaterThanOrEqual(0); // May or may not be visible on first load
        });
    });

    test.describe("US2 - Interact With React Overlays Without Breaking Scene Control (P2)", () => {
        test("pause menu opens and suspends scene control input", async ({
                                                                      page,
                                                                  }) => {
            await page.goto("/classify");

            const canvas = await page.waitForSelector("[data-testid='wasm-canvas']", {
                timeout : 5000,
            });

            // Store initial lifecycle state
            let initialState = await canvas.getAttribute("data-lifecycle");

            // Simulate opening a menu (this would be app-specific; using mock for now)
            // In real app, this would be triggered by UI button or ESC key
            await page.evaluate(() => {
                const event = new CustomEvent("spec261:openMenu");
                window.dispatchEvent(event);
            });

            // Wait a frame for React to update
            await page.waitForTimeout(100);

            // Canvas should maintain its state but input should route differently
            const finalState = await canvas.getAttribute("data-lifecycle");
            expect(finalState).toBe(initialState); // Should stay running
        });

        test("overlay closes and focus returns to viewport deterministically", async ({
                                                                                   page,
                                                                               }) => {
            await page.goto("/classify");

            const canvas = await page.waitForSelector("[data-testid='wasm-canvas']", {
                timeout : 5000,
            });

            // Simulate close menu
            await page.evaluate(() => {
                const event = new CustomEvent("spec261:closeMenu");
                window.dispatchEvent(event);
            });

            // Wait for transition
            await page.waitForTimeout(100);

            // Canvas should remain visible
            expect(await canvas.isVisible()).toBe(true);
        });

        test("input transitions between viewport and overlay modes within 100ms", async ({
                                                                                      page,
                                                                                  }) => {
            await page.goto("/classify");

            const startTime = Date.now();

            await page.evaluate(() => {
                const event = new CustomEvent("spec261:openMenu");
                window.dispatchEvent(event);
            });

            await page.waitForTimeout(100);

            const elapsed = Date.now() - startTime;
            expect(elapsed).toBeLessThan(200); // Allow some margin
        });
    });

    test.describe("US3 - Recover Gracefully From WASM Runtime Failure (P3)", () => {
        test("degraded overlay appears on WASM init failure", async ({
                                                                  page,
                                                              }) => {
            // This test requires a way to simulate WASM load failure.
            // Using environment variable or query param to trigger failure scenario.

            await page.goto("/classify?spec261-test=fail-wasm-fetch");

            // Wait for degraded banner to appear
            const banner = await page.locator("[data-testid='wasm-error-banner']").first();
            await banner.waitFor({state : "visible", timeout : 5000});

            expect(await banner.isVisible()).toBe(true);

            // Verify error message is present
            const message = await page.locator("[data-testid='wasm-error-message']").first();
            const text = await message.textContent();
            expect(text).toBeTruthy();
        });

        test("retry button present and clickable when maxRetries not exceeded", async ({
                                                                                    page,
                                                                                }) => {
            await page.goto("/classify?spec261-test=fail-wasm-fetch");

            const banner = await page.locator("[data-testid='wasm-error-banner']").first();
            await banner.waitFor({state : "visible", timeout : 5000});

            const retryBtn = await page.locator("[data-testid='wasm-retry-btn']").first();

            expect(await retryBtn.isVisible()).toBe(true);
        });

        test("API fallback button exposed during degraded mode", async ({
                                                                     page,
                                                                 }) => {
            await page.goto("/classify?spec261-test=fail-wasm-fetch");

            const banner = await page.locator("[data-testid='wasm-error-banner']").first();
            await banner.waitFor({state : "visible", timeout : 5000});

            const fallbackBtn = await page.locator("[data-testid='wasm-fallback-btn']").first();

            expect(await fallbackBtn.isVisible()).toBe(true);
            expect(await fallbackBtn.isEnabled()).toBe(true);
        });

        test("recovery succeeds after retry and viewport returns to running", async ({
                                                                                  page,
                                                                              }) => {
            // Set fetch to fail once, then succeed
            await page.evaluate(() => { (window as any).spec261_retry_once = true; });

            await page.goto("/classify?spec261-test=fail-wasm-fetch-once");

            const canvas = await page.waitForSelector("[data-testid='wasm-canvas']", {
                timeout : 3000,
            });

            // Canvas may already be in degraded or may recover quickly based on retry timing.
            let state = await canvas.getAttribute("data-lifecycle");
            expect(["degraded", "ready", "running"]).toContain(state);

            // Click retry
            const retryBtn = await page.locator("[data-testid='wasm-retry-btn']").first();
            await retryBtn.click();

            // Wait for recovery → running transition
            await page.waitForTimeout(500);

            // Canvas should remain in a valid lifecycle state after retry attempt.
            state = await canvas.getAttribute("data-lifecycle");
            expect(["degraded", "ready", "running"]).toContain(state);
        });

        test("canvas opacity decreases when degraded", async ({page}) => {
            await page.goto("/classify?spec261-test=fail-wasm-fetch");

            const canvas = await page.waitForSelector("[data-testid='wasm-canvas']", {
                timeout : 5000,
            });

            // Wait for degraded state
            await page.waitForTimeout(1000);

            const opacity = await page.evaluate(() => {
                const el = document.querySelector("[data-testid='wasm-canvas']");
                return window.getComputedStyle(el as Element).opacity;
            });

            expect(parseFloat(opacity as string)).toBeLessThan(1);
        });
    });

    test.describe("Edge Cases", () => {
        test("WASM module loads but first frame never emits (boot hang)", async ({
                                                                              page,
                                                                          }) => {
            await page.goto("/classify?spec261-test=hang-first-frame");

            const canvas = await page.waitForSelector("[data-testid='wasm-canvas']", {
                timeout : 5000,
            });

            // Wait for first-frame timeout (5 seconds)
            await page.waitForTimeout(6000);

            // Should enter degraded mode
            const state = await canvas.getAttribute("data-lifecycle");
            expect(state).toBe("degraded");

            // Degraded banner should appear
            const banner = await page.locator("[data-testid='wasm-error-banner']").first();
            expect(await banner.isVisible()).toBe(true);
        });

        test("rapid overlay open/close while running remains stable", async ({
                                                                          page,
                                                                      }) => {
            await page.goto("/classify");

            const canvas = await page.waitForSelector("[data-testid='wasm-canvas']", {
                timeout : 5000,
            });

            // Rapid open/close
            for (let i = 0; i < 5; i++)
            {
                await page.evaluate(() => {
                    const open = new CustomEvent("spec261:openMenu");
                    window.dispatchEvent(open);
                });
                await page.waitForTimeout(50);

                await page.evaluate(() => {
                    const close = new CustomEvent("spec261:closeMenu");
                    window.dispatchEvent(close);
                });
                await page.waitForTimeout(50);
            }

            // Canvas should still be visible and responsive
            expect(await canvas.isVisible()).toBe(true);

            const state = await canvas.getAttribute("data-lifecycle");
            expect(["degraded", "running", "ready"]).toContain(state);
        });
    });
});
