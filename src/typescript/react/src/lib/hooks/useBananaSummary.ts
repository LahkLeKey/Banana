/**
 * Feature 053 — typed state baseline.
 *
 * First TanStack Query hook in the migration. Wraps `fetchBananaSummary`
 * so callers can drop the local `useState` + `useEffect` + `try/catch`
 * pattern and get cache + retry + background refresh for free.
 *
 * Subsequent slices migrate `predictRipeness`, `fetchEnsembleVerdictWith
 * Embedding`, `createChatSession`, and `sendChatMessage` using the same
 * pattern.
 */
import { type UseQueryResult, useQuery } from "@tanstack/react-query";
import { type BananaSummaryResponse, fetchBananaSummary } from "./api";

export const bananaSummaryKey = (baseUrl: string) => ["banana", "summary", baseUrl] as const;

export function useBananaSummary(
  baseUrl: string,
  enabled = true
): UseQueryResult<BananaSummaryResponse, Error> {
  return useQuery({
    queryKey: bananaSummaryKey(baseUrl),
    queryFn: () => fetchBananaSummary(baseUrl),
    enabled: enabled && Boolean(baseUrl),
  });
}
