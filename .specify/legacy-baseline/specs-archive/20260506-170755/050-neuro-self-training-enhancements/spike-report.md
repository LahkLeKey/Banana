# Spike Report: Neuroscience-Inspired Self-Training Enhancements

**Spec**: [spec.md](spec.md)
**Plan**: [plan.md](plan.md)
**Date**: 2026-05-02
**Status**: Read-only research; no code changed.

## Goal

Identify the smallest set of biologically-inspired training mechanisms that
(1) integrate cleanly with the existing Banana autonomous self-training cycle,
(2) reduce catastrophic forgetting under git-event-driven incremental updates,
(3) run on existing CPU CI runners without new heavy dependencies.

## Mechanisms surveyed

| Mechanism | Brain analogue | What it gives us | Cost / risk |
|---|---|---|---|
| Episodic replay buffer (reservoir-sampled) | Hippocampal short-term store + rehearsal | Anti-forgetting via interleaving old + new samples; cheap, file-based | Storage growth → bounded by reservoir cap |
| Sleep-style consolidation (NREM nightly + REM weekly) | Hippocampo-neocortical replay during sleep; REM creative recombination | Long-horizon stability; off-peak compute; weekly surprise triage | Adds two cron workflows to maintain |
| Elastic Weight Consolidation (EWC) | Synaptic stability — important weights resist change | Quadratic regularizer that protects prior tasks; mature, single Fisher matrix per snapshot | Cold-start case until 2 snapshots exist |
| Predictive-coding surprise weighting | Cortical prediction-error neurons; precision weighting | Sample harder/novel items more often; uses existing model confidence | Requires last-run artifact present |
| Dopamine-style reward modulation of LR | Three-factor learning rules; neuromodulation | Adaptive learning rate based on validation-accuracy delta; bounded scalar | Needs rolling metric history |
| Spike-timing-dependent plasticity (STDP) | Precise spike timing | Powerful but requires spiking nets — out of scope for v1 | Architectural mismatch |
| Generative replay with deep generator | Neocortical pseudo-pattern rehearsal | Avoids storing raw samples | Adds a generator model to maintain — defer |
| Joint-Embedding Predictive Architecture (JEPA) | World-model prediction in latent space | Strong representation learning | Architectural change — defer past v1 |

## References (read-only research)

- Catastrophic interference / forgetting and rehearsal mechanisms — McCloskey & Cohen (1989); French (1999); Robins (1995); Wikipedia: *Catastrophic interference*.
- Elastic Weight Consolidation — Kirkpatrick et al., PNAS 2017 (`arXiv:1612.00796`).
- Synaptic Intelligence — Zenke, Poole, Ganguli, ICML 2017 (`arXiv:1703.04200`).
- Memory Aware Synapses — Aljundi et al., ECCV 2018 (`arXiv:1711.09601`).
- Sleep-replay / spontaneous replay — Golden et al., PLOS Comp Bio 2022; Tadros et al., Nature Comm 2022; Bazhenov 2020.
- Brain-inspired generative replay — van de Ven, Siegelmann, Tolias, Nature Comm 2020.
- Predictive coding — Rao & Ballard 1999; Friston 2010; Wikipedia: *Predictive coding*.
- Hebbian learning + neuromodulation — Wikipedia: *Hebbian theory*; three-factor learning rules.
- Self-supervised learning landscape (context for non-goals) — Wikipedia: *Self-supervised learning*; LeCun JEPA 2022.

## Selected scope for v1

In: replay buffer, EWC, sleep-style consolidation crons, surprise weighting, reward-modulated LR, anti-forgetting guard.
Deferred: STDP, generative replay with learned generator, JEPA-style world models, BCM/Oja stabilization, spiking nets.

## Trigger surfaces

- Git events: `pull_request` (closed && merged) on `main`, `push` on `main`, `workflow_dispatch`. Scope by changed paths under `data/<model>/corpus.json` and `scripts/train-*-model.py`, plus `train:<model>` PR labels.
- Periodic: nightly UTC cron `mode=nrem` (replay-only consolidation, refit Fisher), weekly Sunday cron `mode=rem` (high-surprise triage + creative recombination, opens triage issue).

## Constraint check

- CPU-only, runs in current CI minute budget per model (≤ 10 min target).
- No new GH permissions; reuses existing PR opener (049 path).
- Preserves AI-contract markers and pre-flight validator from 049.
- No new mobile/Electron/React surface — out of scope.

## Open questions resolved by assumptions

- Replay storage location: in-repo `data/<model>/replay/buffer.jsonl` capped at 2048 lines per model; large payload bodies stay referenced by ID, not embedded.
- Cold-start Fisher: zero penalty until two consolidation snapshots exist; recorded as `ewc=cold-start` in evidence.
- Concurrency: single in-flight run per model via `concurrency.group=neuro-train-<model>`.
