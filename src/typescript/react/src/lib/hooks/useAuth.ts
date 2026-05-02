/**
 * useAuth.ts — Auth state hook (feature 063).
 *
 * Wraps token storage / claims parsing for React consumers.
 * Token is kept in memory (not localStorage) to avoid XSS exposure.
 * Persists across page refreshes via sessionStorage (tab-scoped).
 */
import { create } from "zustand";
import { createJSONStorage, persist } from "zustand/middleware";

export type BananaRole = "admin" | "operator" | "viewer";

export interface AuthState {
  token: string | null;
  sub: string | null;
  role: BananaRole | null;
  setToken: (token: string) => void;
  clearToken: () => void;
}

interface JwtPayload {
  sub?: string;
  role?: string;
  exp?: number;
}

function parseJwt(token: string): JwtPayload | null {
  try {
    const payload = token.split(".")[1];
    if (!payload) return null;
    return JSON.parse(atob(payload.replace(/-/g, "+").replace(/_/g, "/")));
  } catch {
    return null;
  }
}

function isExpired(payload: JwtPayload): boolean {
  if (!payload.exp) return false;
  return payload.exp * 1000 < Date.now();
}

export const useAuth = create<AuthState>()(
  persist(
    (set) => ({
      token: null,
      sub: null,
      role: null,
      setToken(token: string) {
        const claims = parseJwt(token);
        if (!claims || isExpired(claims)) {
          set({ token: null, sub: null, role: null });
          return;
        }
        set({
          token,
          sub: claims.sub ?? null,
          role: (claims.role as BananaRole) ?? null,
        });
      },
      clearToken() {
        set({ token: null, sub: null, role: null });
      },
    }),
    {
      name: "banana-auth",
      storage: createJSONStorage(() => sessionStorage),
      // Only persist token; role+sub re-derived on rehydration
      partialize: (state) => ({ token: state.token }),
      onRehydrateStorage: () => (state) => {
        if (state?.token) {
          // Re-parse claims from persisted token
          const claims = parseJwt(state.token);
          if (!claims || isExpired(claims)) {
            state.clearToken();
          } else {
            state.sub = claims.sub ?? null;
            state.role = (claims.role as BananaRole) ?? null;
          }
        }
      },
    }
  )
);
