# Composition strategy -- gated cascade (013 SPIKE, R-01 expansion)

## Strategy

```
Right Brain (binary) -> verdict
   if banana_score in [0.35, 0.65]:
       escalate -> Full Brain (transformer) -> final verdict
       Left Brain (regression) -> reported alongside as calibration magnitude
   else:
       Right Brain verdict is final
       Left Brain (regression) -> reported alongside as calibration magnitude
```

- **Final label / final score** = whichever brain produced the final verdict
  in the cascade (Right or Full).
- **Calibration magnitude** = Left Brain `banana_score` (always reported,
  never the source of the label).
- **Tie-break / failure mode**: if Right Brain returns non-OK, fall through
  directly to Full Brain. If Full Brain also returns non-OK, return the
  Left Brain regression score with `label="unknown"` and a non-OK status.

## Walked anchors (010 + 011 + 012 evidence)

| Payload (text)                                        | Right `banana_score` (011) | Escalate? | Full `banana_score` (012) | Final label | Notes                                                                 |
|-------------------------------------------------------|----------------------------|-----------|---------------------------|-------------|------------------------------------------------------------------------|
| `ripe banana peel smoothie banana bunch banana bread` | clearly > 0.65             | no        | n/a (~0.95 if probed)     | banana      | Right wins outright; Full Brain not invoked -> diagnostics cost zero. |
| `banana banana banana banana banana banana banana banana` | clearly > 0.65         | no        | n/a (~0.94 if probed)     | banana      | Right wins outright.                                                   |
| `plastic engine oil junk waste motor oil`             | clearly < 0.35             | no        | n/a (~0.08 if probed)     | not_banana  | Right wins outright.                                                   |
| `yellow plastic toy shaped like a banana` (decoy)     | ~0.50 (in band)            | YES       | ~0.44 -> not_banana       | not_banana  | Cascade kill: Right was permissive, Full corrects.                     |
| `yellow fruit on the counter maybe`                   | ~0.55 (in band)            | YES       | ~0.83 -> banana           | banana      | Cascade confirms ambiguous yellow-fruit = banana.                      |
| `""` (empty)                                          | ~0.50 (in band)            | YES       | ~0.65 -> banana           | banana      | Cascade default-on-empty -> banana (matches transformer prior).        |

Right Brain anchors are taken from
[011-right-brain-binary-runnable/analysis/us3-calibration-evidence.md](../../011-right-brain-binary-runnable/analysis/us3-calibration-evidence.md).
Full Brain anchors are taken from
[012-full-brain-transformer-runnable/README.md](../../012-full-brain-transformer-runnable/README.md).

## Why a band of [0.35, 0.65]?

- The 011 anchors place "clearly banana" payloads above ~0.78 and "clearly
  not banana" payloads below ~0.20. A 0.35-0.65 escalation band leaves
  clear-cut cases on the cheap path while routing every documented
  ambiguous anchor (the decoy at ~0.50, ambiguous yellow at ~0.55,
  empty at ~0.50) to the Full Brain.
- The band is symmetric around 0.5 so the cascade has no left/right bias.
- Bandwidth is wide enough (0.30) to absorb feature-extraction noise but
  narrow enough to avoid escalating clearly-decided payloads.

## Cost guarantee

- The Full Brain `_ex` ABI is invoked on the escalation tier only and is
  called WITHOUT diagnostics (`log_attention=0`, `out_embedding=NULL`,
  `out_attention_weights=NULL`) by default, preserving the FB-R03
  default-off equivalence locked in 012.
- Diagnostics surface (embedding + attention) is reserved for an opt-in
  controller flag, NOT enabled on the hot path.

## Open question (downstream)

- Should the cascade band be configurable via `appsettings.json` or be
  baked in? Recommendation: bake in for the first managed slice (014),
  expose configurable later only if a real consumer requests it.
