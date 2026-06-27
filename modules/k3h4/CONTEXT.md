# K3H4 Core Module Context

## Purpose

Define the K3H4 native core as a single module boundary with one public library surface.

## In scope

- K3H4 native source moved from legacy native engine locations
- K3H4 native tests moved with the module
- Public K3H4 native interface consumed by runtime and API integration layers

## Out of scope

- API route and client feature ownership for K3H4
- Gameplay-focused suites outside K3H4 core validation

## Ubiquitous language

- K3H4 core module: The isolated native K3H4 implementation at modules/k3h4/native
- Public module boundary: The only supported include and link surface for external consumers
- One-shot migration: A single change that moves K3H4 core code and rewires all call sites
- Dialogue routing artifact: Deterministic routing output (cluster ids, normalized scores, boundary state, transition hints) produced by K3H4 without mutating game state
- State authority boundary: Separation where gameplay systems own quest/world mutations while K3H4 provides routing evidence only
- Dialogue gate precedence: Deterministic conflict resolution order for stacked dialogue clusters where higher-priority gates always win
- Dialogue response contract: The minimum deterministic payload that validators and phrasing layers consume to produce NPC dialogue safely
- Dialogue cluster shard scope: Deterministic cluster scope key used to route dialogue through NPC/faction/quest/region-specific cluster neighborhoods with fallback
- Dialogue boundary state: Normalized-score classification (`core`, `edge`, `outside`) that deterministically selects response mode versus transition routing
- Dialogue fail-closed fallback: Deterministic deny response mode used when all candidate transitions are blocked by safety/canon gates
- Dialogue memory delta: Bounded, quantized per-NPC state effects (not transcript text) used as routing features for subsequent dialogue turns
- Dialogue routing envelope: Versioned ABI payload contract for dialogue cluster assignments with byte-order and integrity guards
- Dialogue turn fixture: Minimal deterministic single-turn input bundle (`npc identity`, `quest state`, `intent`, `policy context`) used to exercise dialogue routing end-to-end
- Dialogue generative loop: Deterministic local processing chain (`route -> template -> generative surface validation -> transition -> memory delta -> ABI envelope`) used for iteration and evidence capture
- Dialogue canonical turn record: Stable JSON output for one dialogue turn containing routing decision metadata plus a renderable NPC line candidate
- Dialogue hard-block mutation guard: Fail-closed policy override contract that emits deterministic fallback responses and blocks all state mutation when hard-block triggers
- Dialogue CLI command family: K3H4-scoped CLI surface where dialogue iteration commands live under `banana k3h4` rather than a separate top-level namespace
- Dialogue native full-chain entrypoint: Single native ABI call that executes the full deterministic dialogue chain for one turn fixture, with Python acting as transport/validation only
- Dialogue fixture schema: Canonical JSON turn-fixture contract (`schema_version=1`) accepted via stdin or explicit input file for deterministic local replay
- Dialogue fixture authority fields: Minimal turn-fixture field set (`npc_id`, `quest_state_id`, `region_id`, `intent_id`, structured `policy_context`, bounded `prior_memory_delta`) required for dialogue-run execution
- Dialogue turn output contract: Canonical single-turn JSON record containing fixture hash, routing/transition decision metadata, mutation guard flags, bounded memory summary, renderable line candidate, and observability metadata
- Dialogue fixture hash: Deterministic digest of canonicalized turn-fixture input used to prove byte-identical replay behavior
- Dialogue mutation guard flags: Required output booleans (`state_mutation_blocked`, `memory_write_blocked`) that expose hard-block and safe-redirect mutation protection outcomes
- Dialogue deterministic fallback set: Small authored fallback response set keyed by `policy category` and `npc_id`, with stable fixture-hash selection for replay-safe non-generic refusals/redirects
- Dialogue canonical local loop: Standard contributor workflow (`dialogue-sample -> dialogue-run --strict -> dialogue-export -> replay parity check -> hard-block guard check`) for local generative iteration
- Dialogue reason-code taxonomy: Deterministic meaning split where `hard_block` uses a reserved deny-code range and `safe_redirect` must remain outside that range
- Dialogue taxonomy smoke script: One-command local regression check that exercises normal-pass, hard-block-pass, and fail-closed invalid-artifact scenarios against the native library
- Dialogue phase gate: Story-level deterministic acceptance gate that must pass before advancing to the next dialogue runtime phase
- Dialogue pilot slice: First constrained NPC/quest/region deployment used to prove deterministic routing before broad rollout
- Policy classification authority: Hard precedence rule where content protection decisions can override normal dialogue routing
- Response shaping: Severity-tiered policy behavior that selects allow, boundary pushback, refusal, or fail-closed fallback without relying on a giant banned-term list
- Extreme policy category: High-risk class (`minor_safety`, `self_harm`, `real_world_violence`, `hate_or_slur_severe`) that always triggers hard-block fail-closed handling
- Policy confidence boundary zone: Deterministic threshold band that uses neutral safe-redirect behavior for ambiguous high-risk classifications before escalating to hard-block
- Policy mutation guardrail: Requirement that policy override paths cannot mutate quest state, routing memory, relationship state, or world/faction state
- Violence domain split: Canonical distinction between fictional game-world violence and real-world violence for policy routing and severity evaluation
- Prompt-attack policy cluster: High-priority policy family for jailbreak, instruction override, and hidden-rule extraction attempts that must not be treated as normal dialogue intent
- Output policy validation: Reuse of the same policy cluster taxonomy on generated NPC lines to detect unsafe amplification, leakage, or canon-breaking output before emission
- Lexical fast path: Small curated extreme-case hint layer that accelerates obvious detections but does not own canonical policy decisions
- Policy override observability: Deterministic event metadata emitted for non-allow policy decisions so safety gates are auditable and regression-testable
- Extreme-edge acceptance pack: Fixed set of high-risk and control scenarios used as hard gating evidence for policy override correctness

## Core invariants

- Consumers link through one target: banana_k3h4_core
- Consumers use only module-prefixed public includes
- Module internals are not directly included by external code
- Dialogue routing remains read-only with respect to quest/world state; K3H4 does not directly mutate gameplay state
- Dialogue gate resolution is deterministic and short-circuited in this order: safety/canon, quest state, NPC/world legality, intent, personality/emotion, lore/style
- Dialogue response contracts always include identity, selected-cluster stack, gate decision, fact allow/forbid sets, state transition id, response policy, and determinism hashes/versioning fields before any generator phrasing step
- Dialogue routing resolves cluster scope through deterministic fallback (NPC+quest+region -> faction+quest+region -> faction+region -> global archetype) and records the selected scope in the response contract
- Dialogue boundary thresholds are deterministic (`core <= 0.85`, `edge 0.85..1.15`, `outside > 1.15`) with fixed tie-break ordering and required boundary-state emission in response contracts
- Denied transitions resolve via deterministic authored fallback templates (no new facts, no state mutation) and always emit fallback metadata plus deny reason codes in response contracts
- Dialogue memory persists only capped delta fields with explicit TTL/range policies, a fixed entry budget per NPC, deterministic eviction, and no raw transcript persistence in routing memory
- Dialogue routing decode is fail-closed: version/byte-order/size/CRC mismatch rejects the payload, and no partial decode may mutate runtime state
- Dialogue roadmap delivery is phase-gated (routing, templates, generative surface, spectral transitions, bounded memory, ABI/export) with deterministic replay/safety checks required at each phase boundary
- Dialogue local loop accepts only deterministic fixtures and must emit byte-identical canonical turn records for identical inputs
- Dialogue hard-block handling is acceptance-gated to fail-closed deterministic fallback behavior with no quest/world/memory mutation
- Dialogue local iteration commands are grouped under the K3H4 CLI namespace to preserve one operator path for native execution, diagnostics, and artifact export
- Dialogue CLI execution uses one native full-chain entrypoint as the sole stage-order authority; Python orchestration cannot reorder or partially execute dialogue phases
- Dialogue local replay input authority is canonical JSON (`schema_version=1`) with deterministic parsing from stdin or `--input-file`
- Dialogue fixture policy input is structured categorical context with confidence hints; free-text policy directives are not authoritative fixture input
- Dialogue local loop output authority is one canonical turn record that includes decision metadata (`route_cluster_id`, `boundary_state`, `transition_id`, `response_policy`, `deny_reason_code`) plus observability metadata (`policy_category`, `severity`, `confidence_band`, `action_selected`, `decode_path`, `abi_version`)
- Dialogue output always includes a renderable `npc_line_candidate`; hard-block and fail-closed paths must emit deterministic fallback text rather than partial generated output
- Dialogue hard-block and confidence-boundary responses must use deterministic fallback-set selection keyed by `policy category` and `npc_id`, with `fixture_hash` as the stable selector
- Dialogue local workflow is fixed to `banana k3h4` command family and includes deterministic replay validation plus hard-block mutation-guard assertions as required acceptance evidence
- Dialogue local workflow includes a one-command smoke gate (`bash cli/banana/scripts/run-dialogue-taxonomy-smoke.sh`) as the minimum end-to-end taxonomy validation pass before broader iteration
- First pilot slice is fixed to one NPC, one quest-state flow, one region, and a bounded intent/safety set to minimize scope risk while proving deterministic routing and fallback behavior
- Policy classification authority is above dialogue routing; `hard_block` always fail-closes to deterministic fallback behavior and bypasses free-form generation
- Severity tiers govern response shaping: lower tiers may allow in-character boundary responses, while severe tiers refuse or fail-closed with deterministic safety metadata
- Hard-block routing is reserved for extreme policy categories; profanity classes remain dialogue-relevant tone signals unless they escalate to targeted harassment
- Policy confidence thresholds are deterministic: high confidence (`>=0.85`) hard-block fail-closed, boundary confidence (`0.65..0.85`) safe-redirect without unsafe continuation, lower confidence continues constrained dialogue with output validation
- Safe-redirect and hard-block both enforce block-state-mutation and block-memory-write semantics; they differ only in deterministic response form
- `hard_block` output taxonomy is deterministic: `deny_reason_code` must remain in reserved range `9100..9199`, both mutation-block flags must be true, and `memory_delta_applied` must be `0`
- `safe_redirect` output taxonomy is deterministic: it must never use deny codes in reserved hard-block range `9100..9199`, while still enforcing both mutation-block flags
- Game-world violence and real-world violence are separate policy clusters; real-world violence may policy-override into redirect or hard-block, while game-world violence remains dialogue-legal when contextualized as fictional combat
- Prompt-attack policy handling never exposes hidden prompts, policy internals, or routing metadata; it either ignores the malicious instruction and continues safely or refuses deterministically outside the normal intent path
- Input and output policy validation share one canonical cluster taxonomy; unsafe generated output never self-repairs recursively and instead degrades to a boring deterministic fallback under the same mutation guardrails
- Lexical hints can short-circuit obvious extreme cases and feed classifier features, but semantic policy clusters remain the canonical policy authority over long-term behavior
- Every policy override emits stable machine-readable metadata (category, severity, confidence band, selected action, deny reason code, mutation-block flags) for acceptance evidence and deterministic auditability
- The extreme-edge acceptance pack is a hard gate with deterministic outcomes across six canonical scenarios: `minor_safety`, `self_harm`, `real_world_violence_targeted`, `hate_or_slur_severe`, `jailbreak_or_prompt_attack`, and `game_world_violence` control behavior

## Key seams

- Native engine runtime consumes K3H4 through the public module boundary
- API native adapters consume K3H4 through the same boundary

## Open questions

- None yet
