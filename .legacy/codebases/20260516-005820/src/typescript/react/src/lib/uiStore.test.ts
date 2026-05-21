/**
 * Feature 053 — typed state baseline tests.
 *
 * Verifies the Zustand UI store contract (initial values, setters, reset).
 * Server-state hook tests live with their owning hook file.
 */
import { beforeEach, describe, expect, it } from "bun:test";
import { useUiStore } from "./uiStore";

describe("uiStore (feature 053)", () => {
  beforeEach(() => {
    useUiStore.getState().reset();
  });

  it("seeds with system theme and closed escalation panel", () => {
    const state = useUiStore.getState();
    expect(state.themeOverride).toBe("system");
    expect(state.escalationPanelOpen).toBe(false);
  });

  it("setThemeOverride mutates only the theme slice", () => {
    useUiStore.getState().setThemeOverride("dark");
    expect(useUiStore.getState().themeOverride).toBe("dark");
    expect(useUiStore.getState().escalationPanelOpen).toBe(false);
  });

  it("setEscalationPanelOpen toggles independently", () => {
    useUiStore.getState().setEscalationPanelOpen(true);
    expect(useUiStore.getState().escalationPanelOpen).toBe(true);
    expect(useUiStore.getState().themeOverride).toBe("system");
  });

  it("reset returns the store to its seed values", () => {
    const s = useUiStore.getState();
    s.setThemeOverride("light");
    s.setEscalationPanelOpen(true);
    s.reset();
    const after = useUiStore.getState();
    expect(after.themeOverride).toBe("system");
    expect(after.escalationPanelOpen).toBe(false);
  });
});
