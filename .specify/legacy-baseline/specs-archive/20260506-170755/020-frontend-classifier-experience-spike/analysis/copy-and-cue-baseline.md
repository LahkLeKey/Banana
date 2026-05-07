# 020 -- Copy + Cue Baseline

Canonical copy + cues that the three follow-up slices (023, 024, 025)
MUST adopt verbatim. Drift = bug.

## Verdict copy

| Verdict label | Display text | Tone |
|---------------|--------------|------|
| `banana` (confident) | "Banana." | Plain assertion |
| `banana` (escalated) | "Banana -- needs a closer look." | Plain + signal |
| `not_banana` (confident) | "Not a banana." | Plain assertion |
| `not_banana` (escalated) | "Not a banana -- needs a closer look." | Plain + signal |
| (no verdict yet) | "Send a sample to get a verdict." | Empty-state prompt |

## Escalation cue

- **Visual primitive**: small inline indicator next to the verdict text. Not a modal, not a toast.
- **Copy**: "needs a closer look" appended to the verdict text.
- **Affordance**: a small "Why?" link that, when activated, opens an inline panel showing the ensemble embedding summary returned by `/ml/ensemble/embedding` (slice 017). Panel is collapsible, defaults closed.
- **Color**: amber accent (token name TBD by SPIKE 021; until tokens exist, use Tailwind `amber-700` on web/desktop and `#b45309` on React Native).

## Error wording

Use the exact strings below (one per failure mode):

| Failure | Customer-facing string |
|---------|------------------------|
| Network unreachable | "Couldn't reach the banana service. Try again." |
| Request timed out | "That took too long. Try again." |
| Server error (5xx) | "Banana service is having a moment. Try again." |
| Invalid input (4xx) | "That sample isn't quite right. Adjust and try again." |
| Unknown | "Something went wrong. Try again." |

## Retry affordance

- Button copy: "Try again"
- Placement: immediately after the error text, on the same row when space allows.
- Behavior: reuses the **last submitted draft** (not the current input). Submitting a new draft clears the retry button.

## Notes

- The follow-up slices may NOT introduce alternate copy ("Looks like a banana", "Hmm, not sure", etc.). Adding a new state requires a new row in this baseline first.
- The escalation panel content is informational only -- no actions, no sharing, no rating in this scope. Those are deferred to a future SPIKE if customer demand surfaces.
