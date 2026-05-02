/**
 * Feature 053 — typed state baseline.
 *
 * Zustand store for client-only UI state. Server state lives in TanStack
 * Query; this store is for local preferences (theme override, last-opened
 * panel, etc.) that need to survive route changes once feature 052 lands.
 *
 * Persistence is intentionally minimal in v1: state lives in memory only.
 * Feature 059 (theming) and 063 (auth) will add localStorage persistence
 * once the contract for what is safe to persist is decided.
 */
import { create } from "zustand";

export type ThemeOverride = "system" | "light" | "dark";

export type UiState = {
    themeOverride: ThemeOverride;
    setThemeOverride: (override: ThemeOverride) => void;
    escalationPanelOpen: boolean;
    setEscalationPanelOpen: (open: boolean) => void;
    reset: () => void;
};

const INITIAL = {
    themeOverride: "system" as ThemeOverride,
    escalationPanelOpen: false,
};

export const useUiStore = create<UiState>((set) => ({
    ...INITIAL,
    setThemeOverride: (override) => set({ themeOverride: override }),
    setEscalationPanelOpen: (open) => set({ escalationPanelOpen: open }),
    reset: () => set({ ...INITIAL }),
}));
