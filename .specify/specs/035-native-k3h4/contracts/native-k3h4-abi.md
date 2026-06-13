# Contract: Native K3H4 ABI Envelope

## Scope
Versioned K3H4 payload and envelope crossing native C runtime, TypeScript FFI service, and API orchestration.

## Canonical Encoding
- Byte order: little-endian canonical.
- Numeric encoding:
   - Fixed-point metrics use signed `Q16.16` as `int32`.
   - Counts/ids and scalar metadata are `int32`.
- Envelope transport fields are stored in the trailing envelope section of `RuntimeNetcodeK3h4Output`.

## Hardware Preflight (Required)
Before any K3H4 clustering entrypoint executes, caller must provide declarations in `RuntimeK3h4VectorSignalInput`:
- `hardware_byte_order_tag == RUNTIME_K3H4_BYTE_ORDER_TAG (0x01020304)`
- `hardware_dtype_tag == RUNTIME_K3H4_DTYPE_TAG_F32_Q16_MIXED (1)`
- `hardware_alignment_bytes == RUNTIME_K3H4_ALIGNMENT_BYTES_4 (4)`

If any preflight declaration mismatches, build returns deterministic contract status:
- `RUNTIME_K3H4_CONTRACT_INVALID_PAYLOAD`

## Explicit Runtime Controls
`RuntimeK3h4VectorSignalInput` includes explicit controls:
- `assignment_family`:
   - `RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE`
   - `RUNTIME_NETCODE_K3H4_ASSIGNMENT_POWER`
- `spectral_mode`:
   - `RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED`
   - `RUNTIME_NETCODE_K3H4_SPECTRAL_AFFINITY_GRAPH`

These are propagated into pipeline execution through config-aware K3H4 build APIs.

## Envelope Fields
Envelope metadata written to `RuntimeNetcodeK3h4Output.envelope`:
1. `contract_version`
2. `byte_order_tag`
3. `payload_bytes`
4. `payload_crc32`
5. `contract_status`

Validation path:
- `runtime_netcode_abi_encode_k3h4_envelope`
- `runtime_netcode_abi_validate_k3h4_envelope`

## Error Contract
- `RUNTIME_K3H4_CONTRACT_OK`
- `RUNTIME_K3H4_CONTRACT_UNSUPPORTED_VERSION`
- `RUNTIME_K3H4_CONTRACT_INVALID_PAYLOAD`
- `RUNTIME_K3H4_CONTRACT_NONFINITE_VALUE`
- `RUNTIME_K3H4_CONTRACT_CRC_MISMATCH`

## Determinism Rules
- Assignment family behavior is deterministic within a selected mode.
- Mode-specific score paths are explicit:
   - Multiplicative score path
   - Power score path
- Spectral output is gated:
   - Spectral disabled mode emits spectral-disabled state.
   - Affinity-graph mode emits active spectral states.
- Deterministic hash is built from output fields including mode-governed score/spectral sections.
