# 020 -- Customer Journeys

Top customer intents that span the three frontend channels, derived from
the channel-inventory matrix.

| # | Intent | Primary channel | Secondary channels | Today gap |
|---|--------|-----------------|---------------------|-----------|
| J1 | "Paste a description, get a verdict" | React (web) | Electron (paste-classify), RN (paste-classify) | Electron has no surface; RN has no verdict call |
| J2 | "Capture/share an image, get a verdict" | RN (camera + share-sheet) | Electron (drop-to-classify) | Neither channel has the entry; React intentionally text-only for now |
| J3 | "Tell me whether the verdict is confident or escalated" | All three | -- | No channel surfaces the escalation status; badge variant is the only signal in React |
| J4 | "Show me my last few verdicts" | All three | -- | None of the channels persist verdict history |
| J5 | "Recover gracefully when the request fails" | All three | -- | Errors are inline text only; no retry button, no draft preservation |

Notes:

- J1 and J2 are explicit customer entry points; J3/J4/J5 are quality
  attributes that ride on every entry.
- J4 (history) requires persistence, which is the SPIKE 022 surface; the
  follow-up slices in this SPIKE focus on **today-session** history
  only and defer cross-session history to SPIKE 022.
- J5 retry behavior overlaps with SPIKE 022 (offline/queueing); the
  follow-up slices implement the **inline retry button + draft
  preservation**, deferring background queueing to SPIKE 022.
