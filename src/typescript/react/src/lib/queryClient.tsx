/**
 * Feature 053 — typed state baseline.
 *
 * Centralized TanStack Query client + provider. Subsequent slices (054
 * streaming, 064 operator dashboard, 065 neuro-trace viewer) consume this
 * provider; v1 ships the provider plus a single `useBananaSummary` hook so
 * the migration pattern is established without rewriting App.tsx in one go.
 */

import { QueryClient, QueryClientProvider } from "@tanstack/react-query";
import { type ReactNode, useState } from "react";

export function createBananaQueryClient(): QueryClient {
  return new QueryClient({
    defaultOptions: {
      queries: {
        // Server state stays fresh for one minute by default; matches
        // the cadence the resilience-bootstrap queue uses for retry
        // bookkeeping. Individual hooks can override.
        staleTime: 60_000,
        gcTime: 5 * 60_000,
        retry: 2,
        refetchOnWindowFocus: false,
      },
      mutations: {
        retry: 0,
      },
    },
  });
}

export function QueryProvider({ children }: { children: ReactNode }): JSX.Element {
  const [client] = useState(() => createBananaQueryClient());
  return <QueryClientProvider client={client}>{children}</QueryClientProvider>;
}
