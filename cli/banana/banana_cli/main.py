from __future__ import annotations

import argparse
import ctypes
import hashlib
import json
import os
from pathlib import Path
import platform
import random
import csv
import io
import sys
from dataclasses import dataclass
from typing import Sequence


DEFAULT_API_URL = "http://localhost:3000"
EXPECTED_ABI_VERSION = 3
EXPECTED_PING_STATUS = 0
REQUIRED_PYTHON = (3, 10)
CANONICAL_SCHEMA_RELATIVE = Path("src/typescript/api/coverage-denominator.json")
RUN_OUTPUT_SCHEMA_RELATIVE = Path("cli/banana/schema/k3h4-run-output.v1.json")
EXPORT_OUTPUT_SCHEMA_RELATIVE = Path("cli/banana/schema/k3h4-export-output.v1.json")
DIALOGUE_RUN_INPUT_SCHEMA_RELATIVE = Path("cli/banana/schema/k3h4-dialogue-run-input.v1.json")
DIALOGUE_RUN_OUTPUT_SCHEMA_RELATIVE = Path("cli/banana/schema/k3h4-dialogue-run-output.v1.json")
DIALOGUE_EXPORT_OUTPUT_SCHEMA_RELATIVE = Path("cli/banana/schema/k3h4-dialogue-export-output.v1.json")
SAMPLE_SCHEMA_VERSION = 1
EXPLAIN_SCHEMA_VERSION = 1
EXPORT_SCHEMA_VERSION = 1
DIALOGUE_FIXTURE_SCHEMA_VERSION = 1
DIALOGUE_EXPORT_SCHEMA_VERSION = 1
HARD_BLOCK_DENY_REASON_MIN = 9100
HARD_BLOCK_DENY_REASON_MAX = 9199
DEFAULT_SAMPLE_PRESET = "baseline"
RUN_FLOAT_PRECISION = 6
NATIVE_K3H4_CONTRACT_OK = 0
DIALOGUE_RUN_SYMBOL_CANDIDATES = (
    "banana_native_v3_k3h4_run_dialogue_turn",
    "banana_native_v3_k3h4_dialogue_run_turn",
    "banana_native_v3_netcode_run_k3h4_dialogue_turn",
)
DIALOGUE_FIXTURE_FIELDS = (
    "npc_id",
    "quest_state_id",
    "region_id",
    "intent_id",
    "policy_context",
    "prior_memory_delta",
)
DIALOGUE_SAMPLE_PRESETS = (
    "pilot-edda",
    "hard-block-self-harm",
)
DIALOGUE_EXPORT_FORMATS = (
    "json",
    "csv",
)
VECTOR_INPUT_FIELDS = (
    "call_density",
    "quest_percent",
    "player_level",
    "combo_streak",
    "branch_pressure",
    "dependency_pulse",
    "workflow_depth",
    "neural_relevance_score",
    "network_dimensions",
    "model_confidence",
    "policy_momentum",
    "assignment_family",
    "spectral_mode",
    "hardware_byte_order_tag",
    "hardware_dtype_tag",
    "hardware_alignment_bytes",
)
NATIVE_LIBRARY_FILENAMES = (
    "libbanana_native.so",
    "banana_native.so",
    "banana_native.dll",
    "libbanana_native.dylib",
    "banana_native.dylib",
)


@dataclass
class DoctorFinding:
    level: str
    check: str
    error_code: str
    message: str
    field_path: str | None = None


def _is_error(finding: DoctorFinding) -> bool:
    return finding.level == "error"


def _as_payload(finding: DoctorFinding) -> dict[str, str]:
    payload = {
        "error_code": finding.error_code,
        "message": finding.message,
    }
    if finding.field_path:
        payload["field_path"] = finding.field_path
    return payload


def _emit_error_envelope(finding: DoctorFinding) -> None:
    print(json.dumps(_as_payload(finding), sort_keys=True), file=sys.stderr)


def _resolve_repo_root(start: Path) -> Path:
    for candidate in [start, *start.parents]:
        if (candidate / ".git").exists():
            return candidate
    return start


def _iter_library_candidates(base: Path):
    for name in NATIVE_LIBRARY_FILENAMES:
        yield base / name
        yield base / "bin" / name
        yield base / "Debug" / name
        yield base / "Release" / name
        yield base / "RelWithDebInfo" / name
        yield base / "MinSizeRel" / name


def _resolve_env_library(env_value: str) -> Path | None:
    env_path = Path(env_value).expanduser()
    if env_path.is_file():
        return env_path
    if env_path.is_dir():
        for candidate in _iter_library_candidates(env_path):
            if candidate.exists() and candidate.is_file():
                return candidate
    return None


def _resolve_native_library(args: argparse.Namespace, repo_root: Path) -> tuple[Path | None, str, list[str]]:
    checked: list[str] = []

    if args.native_lib:
        explicit = Path(args.native_lib).expanduser()
        if not explicit.is_absolute():
            explicit = (Path.cwd() / explicit).resolve()
        checked.append(str(explicit))
        if explicit.exists() and explicit.is_file():
            return explicit, "--native-lib", checked
        return None, "--native-lib", checked

    env_value = os.environ.get("BANANA_NATIVE_PATH", "").strip()
    if env_value:
        checked.append(env_value)
        resolved_env = _resolve_env_library(env_value)
        if resolved_env is not None:
            checked.append(str(resolved_env))
            return resolved_env, "BANANA_NATIVE_PATH", checked

    for folder in ("out/v3-native", "out/native", "out"):
        base = repo_root / folder
        checked.append(str(base))
        if not base.exists() or not base.is_dir():
            continue
        for candidate in _iter_library_candidates(base):
            checked.append(str(candidate))
            if candidate.exists() and candidate.is_file():
                return candidate, "autodiscovery", checked

    return None, "autodiscovery", checked


def _resolve_schema_path(args: argparse.Namespace, repo_root: Path) -> Path:
    requested = Path(args.schema_path).expanduser()
    if requested.is_absolute():
        return requested
    return (repo_root / requested).resolve()


def _not_implemented(command: str) -> int:
    print(f"banana k3h4 {command}: not implemented yet (scaffold placeholder)", file=sys.stderr)
    return 2


def _k3h4_group_help(args: argparse.Namespace) -> int:
    parser = args._parser
    parser.print_help()
    return 0


def _k3h4_sample(args: argparse.Namespace) -> int:
    if args.count < 1:
        finding = DoctorFinding(
            level="error",
            check="sample_validation",
            error_code="BANANA_SAMPLE_COUNT_INVALID",
            message="count must be >= 1",
            field_path="count",
        )
        _emit_error_envelope(finding)
        return 1

    if args.dims < 1 or args.dims > 16:
        finding = DoctorFinding(
            level="error",
            check="sample_validation",
            error_code="BANANA_SAMPLE_DIMS_INVALID",
            message="dims must be in range 1..16",
            field_path="dims",
        )
        _emit_error_envelope(finding)
        return 1

    rng = random.Random(args.seed)
    samples = [_build_k3h4_sample_vector(rng=rng, dims=args.dims, preset=args.preset) for _ in range(args.count)]
    payload = {
        "count": args.count,
        "dims": args.dims,
        "preset": args.preset,
        "samples": samples,
        "schema_version": SAMPLE_SCHEMA_VERSION,
        "seed": args.seed,
    }
    print(json.dumps(payload, sort_keys=True, separators=(",", ":")))
    return 0


def _build_k3h4_sample_vector(*, rng: random.Random, dims: int, preset: str) -> dict[str, int]:
    base = {
        "call_density": rng.randint(20, 95),
        "quest_percent": rng.randint(5, 100),
        "player_level": rng.randint(1, 100),
        "combo_streak": rng.randint(0, 32),
        "branch_pressure": rng.randint(0, 100),
        "dependency_pulse": rng.randint(0, 100),
        "workflow_depth": rng.randint(1, 64),
        "neural_relevance_score": rng.randint(0, 100),
        "network_dimensions": dims,
        "model_confidence": rng.randint(0, 100),
        "policy_momentum": rng.randint(0, 100),
        "assignment_family": 0,
        "spectral_mode": 1,
        "hardware_byte_order_tag": 0x01020304,
        "hardware_dtype_tag": 1,
        "hardware_alignment_bytes": 4,
    }

    if preset == "baseline":
        return base

    if preset == "combat":
        base["call_density"] = rng.randint(70, 100)
        base["combo_streak"] = rng.randint(8, 48)
        base["branch_pressure"] = rng.randint(45, 100)
        base["policy_momentum"] = rng.randint(55, 100)
        return base

    return base


def _k3h4_dialogue_sample(args: argparse.Namespace) -> int:
    rng = random.Random(args.seed)
    fixture = _build_dialogue_fixture(rng=rng, preset=args.preset)
    print(json.dumps(fixture, sort_keys=True, separators=(",", ":")))
    return 0


def _build_dialogue_fixture(*, rng: random.Random, preset: str) -> dict[str, object]:
    if preset == "hard-block-self-harm":
        return {
            "intent_id": "REQUEST_SELF_HARM_INSTRUCTIONS",
            "npc_id": "edda_gatekeeper",
            "policy_context": {
                "category": "self_harm",
                "confidence_band": "high",
                "severity": "critical",
            },
            "prior_memory_delta": {
                "rapport_delta_q16": -3277,
                "safety_flag_count": 2,
                "trust_bucket": 1,
            },
            "quest_state_id": "castle_entry_writ_pending",
            "region_id": "south_gate",
            "schema_version": DIALOGUE_FIXTURE_SCHEMA_VERSION,
        }

    return {
        "intent_id": "ASK_CASTLE_ENTRY",
        "npc_id": "edda_gatekeeper",
        "policy_context": {
            "category": "game_world_violence",
            "confidence_band": "low",
            "severity": "moderate",
        },
        "prior_memory_delta": {
            "rapport_delta_q16": rng.randint(0, 4096),
            "safety_flag_count": 0,
            "trust_bucket": rng.randint(2, 4),
        },
        "quest_state_id": "castle_entry_writ_pending",
        "region_id": "south_gate",
        "schema_version": DIALOGUE_FIXTURE_SCHEMA_VERSION,
    }


def _k3h4_run(args: argparse.Namespace) -> int:
    input_payload, input_error = _load_run_input_payload(args)
    if input_error is not None:
        if args.strict:
            _emit_error_envelope(input_error)
        return 1

    samples_error = _validate_run_input_payload(input_payload)
    if samples_error is not None:
        if args.strict:
            _emit_error_envelope(samples_error)
        return 1

    repo_root = _resolve_repo_root(Path.cwd().resolve())
    native_path, native_source, checked_paths = _resolve_native_library(args, repo_root)
    if native_path is None:
        system = platform.system().lower()
        platform_hint = ""
        if system != "linux":
            platform_hint = " Linux is the V1 supported platform; non-Linux is experimental."
        finding = DoctorFinding(
            level="error",
            check="run_native_library_resolution",
            error_code="BANANA_RUN_NATIVE_LIB_NOT_FOUND",
            message=(
                "Unable to resolve Banana native library via --native-lib, BANANA_NATIVE_PATH, or autodiscovery."
                f" Checked paths: {checked_paths}.{platform_hint}"
            ),
            field_path="native.library",
        )
        if args.strict:
            _emit_error_envelope(finding)
        return 1

    try:
        native = ctypes.CDLL(str(native_path))
    except OSError as exc:
        system = platform.system().lower()
        platform_hint = ""
        if system != "linux":
            platform_hint = " Linux is the V1 supported platform; non-Linux is experimental."
        finding = DoctorFinding(
            level="error",
            check="run_native_load",
            error_code="BANANA_RUN_NATIVE_LOAD_FAILED",
            message=f"Failed to load native library at {native_path}: {exc}.{platform_hint}",
            field_path="native.library",
        )
        if args.strict:
            _emit_error_envelope(finding)
        return 1

    build_fn = getattr(native, "banana_native_v3_netcode_build_k3h4", None)
    if build_fn is None:
        finding = DoctorFinding(
            level="error",
            check="run_native_binding",
            error_code="BANANA_RUN_SYMBOL_MISSING",
            message="Missing symbol banana_native_v3_netcode_build_k3h4 in native library.",
            field_path="native.symbols.banana_native_v3_netcode_build_k3h4",
        )
        if args.strict:
            _emit_error_envelope(finding)
        return 1

    build_fn.argtypes = [ctypes.POINTER(_VectorInput), ctypes.POINTER(_K3h4Output)]
    build_fn.restype = ctypes.c_int

    results: list[dict[str, object]] = []
    for index, sample in enumerate(input_payload["samples"]):
        vector = _VectorInput(**sample)
        output = _K3h4Output()
        status = int(build_fn(ctypes.byref(vector), ctypes.byref(output)))
        if status != NATIVE_K3H4_CONTRACT_OK:
            finding = DoctorFinding(
                level="error",
                check="run_native_execute",
                error_code="BANANA_RUN_NATIVE_CONTRACT_FAILURE",
                message=(
                    "banana_native_v3_netcode_build_k3h4 returned non-zero status "
                    f"{status} for sample index {index}."
                ),
                field_path=f"samples[{index}]",
            )
            if args.strict:
                _emit_error_envelope(finding)
            return 1
        results.append(_serialize_k3h4_output(output))

    output_payload = {
        "input_sample_count": len(input_payload["samples"]),
        "native_library_path": str(native_path),
        "native_resolution_source": native_source,
        "results": results,
        "schema_version": SAMPLE_SCHEMA_VERSION,
    }

    schema_path = _resolve_run_output_schema_path(args, repo_root)
    schema_error = _validate_run_output_payload(output_payload, schema_path)
    if schema_error is not None:
        if args.strict:
            _emit_error_envelope(schema_error)
        return 1

    print(json.dumps(output_payload, sort_keys=True, separators=(",", ":")))
    return 0


def _is_int_value(value: object) -> bool:
    return isinstance(value, int) and not isinstance(value, bool)


def _load_run_input_payload(args: argparse.Namespace) -> tuple[dict[str, object] | None, DoctorFinding | None]:
    raw = ""
    if args.input_file:
        input_path = Path(args.input_file).expanduser()
        if not input_path.is_absolute():
            input_path = (Path.cwd() / input_path).resolve()
        if not input_path.exists() or not input_path.is_file():
            return None, DoctorFinding(
                level="error",
                check="run_input",
                error_code="BANANA_RUN_INPUT_FILE_NOT_FOUND",
                message=f"Input file not found: {input_path}",
                field_path="input_file",
            )
        try:
            raw = input_path.read_text(encoding="utf-8")
        except OSError as exc:
            return None, DoctorFinding(
                level="error",
                check="run_input",
                error_code="BANANA_RUN_INPUT_FILE_READ_FAILED",
                message=f"Failed to read input file: {exc}",
                field_path="input_file",
            )
    else:
        raw = sys.stdin.read()

    if raw.strip() == "":
        return None, DoctorFinding(
            level="error",
            check="run_input",
            error_code="BANANA_RUN_INPUT_EMPTY",
            message="No input JSON was provided via stdin or --input-file.",
            field_path="stdin",
        )

    try:
        payload = json.loads(raw)
    except json.JSONDecodeError as exc:
        return None, DoctorFinding(
            level="error",
            check="run_input",
            error_code="BANANA_RUN_INPUT_JSON_INVALID",
            message=f"Input is not valid JSON: {exc}",
            field_path="stdin",
        )

    if not isinstance(payload, dict):
        return None, DoctorFinding(
            level="error",
            check="run_input",
            error_code="BANANA_RUN_INPUT_SHAPE_INVALID",
            message="Input JSON must be an object.",
            field_path="stdin",
        )

    return payload, None


def _validate_run_input_payload(payload: dict[str, object]) -> DoctorFinding | None:
    schema_version = payload.get("schema_version")
    if schema_version != SAMPLE_SCHEMA_VERSION:
        return DoctorFinding(
            level="error",
            check="run_input",
            error_code="BANANA_RUN_SCHEMA_VERSION_MISMATCH",
            message=f"Expected schema_version={SAMPLE_SCHEMA_VERSION}, found {schema_version!r}.",
            field_path="schema_version",
        )

    samples = payload.get("samples")
    if not isinstance(samples, list) or len(samples) == 0:
        return DoctorFinding(
            level="error",
            check="run_input",
            error_code="BANANA_RUN_SAMPLES_MISSING",
            message="Input must include non-empty samples array.",
            field_path="samples",
        )

    expected = set(VECTOR_INPUT_FIELDS)
    for index, item in enumerate(samples):
        if not isinstance(item, dict):
            return DoctorFinding(
                level="error",
                check="run_input",
                error_code="BANANA_RUN_SAMPLE_SHAPE_INVALID",
                message="Each sample must be an object with exact native-aligned fields.",
                field_path=f"samples[{index}]",
            )

        keys = set(item.keys())
        missing = sorted(expected - keys)
        unknown = sorted(keys - expected)
        if missing:
            return DoctorFinding(
                level="error",
                check="run_input",
                error_code="BANANA_RUN_SAMPLE_FIELDS_MISSING",
                message=f"Missing required fields: {missing}",
                field_path=f"samples[{index}]",
            )
        if unknown:
            return DoctorFinding(
                level="error",
                check="run_input",
                error_code="BANANA_RUN_SAMPLE_FIELDS_UNKNOWN",
                message=f"Unknown fields are not allowed in V1: {unknown}",
                field_path=f"samples[{index}]",
            )

        for field_name in VECTOR_INPUT_FIELDS:
            if not _is_int_value(item[field_name]):
                return DoctorFinding(
                    level="error",
                    check="run_input",
                    error_code="BANANA_RUN_SAMPLE_FIELD_TYPE_INVALID",
                    message=f"Field {field_name} must be an integer.",
                    field_path=f"samples[{index}].{field_name}",
                )

    return None


def _load_dialogue_input_payload(args: argparse.Namespace) -> tuple[dict[str, object] | None, DoctorFinding | None]:
    raw = ""
    if args.input_file:
        input_path = Path(args.input_file).expanduser()
        if not input_path.is_absolute():
            input_path = (Path.cwd() / input_path).resolve()
        if not input_path.exists() or not input_path.is_file():
            return None, DoctorFinding(
                level="error",
                check="dialogue_run_input",
                error_code="BANANA_DIALOGUE_RUN_INPUT_FILE_NOT_FOUND",
                message=f"Input file not found: {input_path}",
                field_path="input_file",
            )
        try:
            raw = input_path.read_text(encoding="utf-8")
        except OSError as exc:
            return None, DoctorFinding(
                level="error",
                check="dialogue_run_input",
                error_code="BANANA_DIALOGUE_RUN_INPUT_FILE_READ_FAILED",
                message=f"Failed to read input file: {exc}",
                field_path="input_file",
            )
    else:
        raw = sys.stdin.read()

    if raw.strip() == "":
        return None, DoctorFinding(
            level="error",
            check="dialogue_run_input",
            error_code="BANANA_DIALOGUE_RUN_INPUT_EMPTY",
            message="No dialogue fixture JSON was provided via stdin or --input-file.",
            field_path="stdin",
        )

    try:
        payload = json.loads(raw)
    except json.JSONDecodeError as exc:
        return None, DoctorFinding(
            level="error",
            check="dialogue_run_input",
            error_code="BANANA_DIALOGUE_RUN_INPUT_JSON_INVALID",
            message=f"Input is not valid JSON: {exc}",
            field_path="stdin",
        )

    if not isinstance(payload, dict):
        return None, DoctorFinding(
            level="error",
            check="dialogue_run_input",
            error_code="BANANA_DIALOGUE_RUN_INPUT_SHAPE_INVALID",
            message="Input JSON must be an object.",
            field_path="stdin",
        )

    return payload, None


def _validate_dialogue_input_payload(payload: dict[str, object]) -> DoctorFinding | None:
    schema_version = payload.get("schema_version")
    if schema_version != DIALOGUE_FIXTURE_SCHEMA_VERSION:
        return DoctorFinding(
            level="error",
            check="dialogue_run_input",
            error_code="BANANA_DIALOGUE_RUN_SCHEMA_VERSION_MISMATCH",
            message=(
                f"Expected schema_version={DIALOGUE_FIXTURE_SCHEMA_VERSION}, found {schema_version!r}."
            ),
            field_path="schema_version",
        )

    expected = set(DIALOGUE_FIXTURE_FIELDS)
    keys = set(payload.keys())
    missing = sorted(expected - keys)
    unknown = sorted(keys - (expected | {"schema_version"}))
    if missing:
        return DoctorFinding(
            level="error",
            check="dialogue_run_input",
            error_code="BANANA_DIALOGUE_RUN_FIELDS_MISSING",
            message=f"Missing required fields: {missing}",
            field_path="fixture",
        )
    if unknown:
        return DoctorFinding(
            level="error",
            check="dialogue_run_input",
            error_code="BANANA_DIALOGUE_RUN_FIELDS_UNKNOWN",
            message=f"Unknown fields are not allowed in V1: {unknown}",
            field_path="fixture",
        )

    for id_field in ("npc_id", "quest_state_id", "region_id", "intent_id"):
        value = payload.get(id_field)
        if not isinstance(value, str) or value.strip() == "":
            return DoctorFinding(
                level="error",
                check="dialogue_run_input",
                error_code="BANANA_DIALOGUE_RUN_ID_FIELD_INVALID",
                message=f"Field {id_field} must be a non-empty string.",
                field_path=id_field,
            )

    policy_context = payload.get("policy_context")
    if not isinstance(policy_context, dict):
        return DoctorFinding(
            level="error",
            check="dialogue_run_input",
            error_code="BANANA_DIALOGUE_RUN_POLICY_CONTEXT_INVALID",
            message="Field policy_context must be a JSON object.",
            field_path="policy_context",
        )

    required_policy_keys = {"category", "confidence_band", "severity"}
    missing_policy = sorted(required_policy_keys - set(policy_context.keys()))
    if missing_policy:
        return DoctorFinding(
            level="error",
            check="dialogue_run_input",
            error_code="BANANA_DIALOGUE_RUN_POLICY_CONTEXT_FIELDS_MISSING",
            message=f"policy_context missing required fields: {missing_policy}",
            field_path="policy_context",
        )

    for key in required_policy_keys:
        value = policy_context.get(key)
        if not isinstance(value, str) or value.strip() == "":
            return DoctorFinding(
                level="error",
                check="dialogue_run_input",
                error_code="BANANA_DIALOGUE_RUN_POLICY_CONTEXT_FIELD_INVALID",
                message=f"policy_context.{key} must be a non-empty string.",
                field_path=f"policy_context.{key}",
            )

    prior_memory_delta = payload.get("prior_memory_delta")
    if not isinstance(prior_memory_delta, dict):
        return DoctorFinding(
            level="error",
            check="dialogue_run_input",
            error_code="BANANA_DIALOGUE_RUN_MEMORY_DELTA_INVALID",
            message="Field prior_memory_delta must be a JSON object.",
            field_path="prior_memory_delta",
        )

    return None


def _validate_dialogue_run_input_payload_against_schema(
    payload: dict[str, object], schema_path: Path
) -> DoctorFinding | None:
    if not schema_path.exists() or not schema_path.is_file():
        return DoctorFinding(
            level="error",
            check="dialogue_run_input_schema",
            error_code="BANANA_DIALOGUE_RUN_INPUT_SCHEMA_NOT_FOUND",
            message=f"Dialogue run input schema file not found: {schema_path}",
            field_path="schema.path",
        )

    try:
        schema_payload = json.loads(schema_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        return DoctorFinding(
            level="error",
            check="dialogue_run_input_schema",
            error_code="BANANA_DIALOGUE_RUN_INPUT_SCHEMA_UNREADABLE",
            message=f"Dialogue run input schema could not be parsed: {exc}",
            field_path="schema.path",
        )

    expected_version = schema_payload.get("schema_version")
    if expected_version != DIALOGUE_FIXTURE_SCHEMA_VERSION:
        return DoctorFinding(
            level="error",
            check="dialogue_run_input_schema",
            error_code="BANANA_DIALOGUE_RUN_INPUT_SCHEMA_VERSION_UNSUPPORTED",
            message=f"Expected dialogue run input schema version 1, found {expected_version!r}.",
            field_path="schema.schema_version",
        )

    required_top_level = schema_payload.get("required_top_level", [])
    if not isinstance(required_top_level, list):
        return DoctorFinding(
            level="error",
            check="dialogue_run_input_schema",
            error_code="BANANA_DIALOGUE_RUN_INPUT_SCHEMA_INVALID",
            message="required_top_level must be an array in dialogue run input schema.",
            field_path="schema.required_top_level",
        )

    for key in required_top_level:
        if key not in payload:
            return DoctorFinding(
                level="error",
                check="dialogue_run_input_schema",
                error_code="BANANA_DIALOGUE_RUN_INPUT_SCHEMA_VALIDATION_FAILED",
                message=f"Dialogue run input is missing required top-level key {key!r}.",
                field_path=f"input.{key}",
            )

    required_policy_context_keys = schema_payload.get("required_policy_context_keys", [])
    if not isinstance(required_policy_context_keys, list):
        return DoctorFinding(
            level="error",
            check="dialogue_run_input_schema",
            error_code="BANANA_DIALOGUE_RUN_INPUT_SCHEMA_INVALID",
            message="required_policy_context_keys must be an array in dialogue run input schema.",
            field_path="schema.required_policy_context_keys",
        )

    policy_context = payload.get("policy_context")
    if not isinstance(policy_context, dict):
        return DoctorFinding(
            level="error",
            check="dialogue_run_input_schema",
            error_code="BANANA_DIALOGUE_RUN_INPUT_SCHEMA_VALIDATION_FAILED",
            message="Dialogue run input policy_context must be an object.",
            field_path="input.policy_context",
        )
    for key in required_policy_context_keys:
        if key not in policy_context:
            return DoctorFinding(
                level="error",
                check="dialogue_run_input_schema",
                error_code="BANANA_DIALOGUE_RUN_INPUT_SCHEMA_VALIDATION_FAILED",
                message=f"policy_context is missing required key {key!r}.",
                field_path=f"input.policy_context.{key}",
            )

    required_prior_memory_delta_keys = schema_payload.get("required_prior_memory_delta_keys", [])
    if not isinstance(required_prior_memory_delta_keys, list):
        return DoctorFinding(
            level="error",
            check="dialogue_run_input_schema",
            error_code="BANANA_DIALOGUE_RUN_INPUT_SCHEMA_INVALID",
            message="required_prior_memory_delta_keys must be an array in dialogue run input schema.",
            field_path="schema.required_prior_memory_delta_keys",
        )

    prior_memory_delta = payload.get("prior_memory_delta")
    if not isinstance(prior_memory_delta, dict):
        return DoctorFinding(
            level="error",
            check="dialogue_run_input_schema",
            error_code="BANANA_DIALOGUE_RUN_INPUT_SCHEMA_VALIDATION_FAILED",
            message="Dialogue run input prior_memory_delta must be an object.",
            field_path="input.prior_memory_delta",
        )
    for key in required_prior_memory_delta_keys:
        if key not in prior_memory_delta:
            return DoctorFinding(
                level="error",
                check="dialogue_run_input_schema",
                error_code="BANANA_DIALOGUE_RUN_INPUT_SCHEMA_VALIDATION_FAILED",
                message=f"prior_memory_delta is missing required key {key!r}.",
                field_path=f"input.prior_memory_delta.{key}",
            )

    return None


def _validate_dialogue_run_output_payload(payload: dict[str, object], schema_path: Path) -> DoctorFinding | None:
    if not schema_path.exists() or not schema_path.is_file():
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_NOT_FOUND",
            message=f"Dialogue run output schema file not found: {schema_path}",
            field_path="schema.path",
        )

    try:
        schema_payload = json.loads(schema_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_UNREADABLE",
            message=f"Dialogue run output schema could not be parsed: {exc}",
            field_path="schema.path",
        )

    expected_version = schema_payload.get("schema_version")
    if expected_version != DIALOGUE_FIXTURE_SCHEMA_VERSION:
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_VERSION_UNSUPPORTED",
            message=f"Expected dialogue run output schema version 1, found {expected_version!r}.",
            field_path="schema.schema_version",
        )

    required_top_level = schema_payload.get("required_top_level", [])
    if not isinstance(required_top_level, list):
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_INVALID",
            message="required_top_level must be an array in dialogue run output schema.",
            field_path="schema.required_top_level",
        )

    for key in required_top_level:
        if key not in payload:
            return DoctorFinding(
                level="error",
                check="dialogue_run_output_schema",
                error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_VALIDATION_FAILED",
                message=f"Dialogue run output is missing required top-level key {key!r}.",
                field_path=f"output.{key}",
            )

    if payload.get("schema_version") != DIALOGUE_FIXTURE_SCHEMA_VERSION:
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_VALIDATION_FAILED",
            message=(
                f"Dialogue run output schema_version must be {DIALOGUE_FIXTURE_SCHEMA_VERSION}, "
                f"found {payload.get('schema_version')!r}."
            ),
            field_path="output.schema_version",
        )

    decision_metadata = payload.get("decision_metadata")
    if not isinstance(decision_metadata, dict):
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_VALIDATION_FAILED",
            message="decision_metadata must be an object in dialogue run output.",
            field_path="output.decision_metadata",
        )
    required_decision_metadata_keys = schema_payload.get("required_decision_metadata_keys", [])
    if not isinstance(required_decision_metadata_keys, list):
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_INVALID",
            message="required_decision_metadata_keys must be an array in dialogue run output schema.",
            field_path="schema.required_decision_metadata_keys",
        )
    for key in required_decision_metadata_keys:
        if key not in decision_metadata:
            return DoctorFinding(
                level="error",
                check="dialogue_run_output_schema",
                error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_VALIDATION_FAILED",
                message=f"decision_metadata is missing required key {key!r}.",
                field_path=f"output.decision_metadata.{key}",
            )

    mutation_flags = payload.get("mutation_flags")
    if not isinstance(mutation_flags, dict):
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_VALIDATION_FAILED",
            message="mutation_flags must be an object in dialogue run output.",
            field_path="output.mutation_flags",
        )
    required_mutation_flags_keys = schema_payload.get("required_mutation_flags_keys", [])
    if not isinstance(required_mutation_flags_keys, list):
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_INVALID",
            message="required_mutation_flags_keys must be an array in dialogue run output schema.",
            field_path="schema.required_mutation_flags_keys",
        )
    for key in required_mutation_flags_keys:
        if key not in mutation_flags:
            return DoctorFinding(
                level="error",
                check="dialogue_run_output_schema",
                error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_VALIDATION_FAILED",
                message=f"mutation_flags is missing required key {key!r}.",
                field_path=f"output.mutation_flags.{key}",
            )

    observability = payload.get("observability")
    if not isinstance(observability, dict):
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_VALIDATION_FAILED",
            message="observability must be an object in dialogue run output.",
            field_path="output.observability",
        )
    required_observability_keys = schema_payload.get("required_observability_keys", [])
    if not isinstance(required_observability_keys, list):
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_INVALID",
            message="required_observability_keys must be an array in dialogue run output schema.",
            field_path="schema.required_observability_keys",
        )
    for key in required_observability_keys:
        if key not in observability:
            return DoctorFinding(
                level="error",
                check="dialogue_run_output_schema",
                error_code="BANANA_DIALOGUE_RUN_OUTPUT_SCHEMA_VALIDATION_FAILED",
                message=f"observability is missing required key {key!r}.",
                field_path=f"output.observability.{key}",
            )

    response_policy_label = payload.get("decision_metadata", {}).get("response_policy")
    deny_reason_code = payload.get("decision_metadata", {}).get("deny_reason_code")
    mutation_flags = payload.get("mutation_flags", {})
    memory_delta_applied = payload.get("memory_delta_applied")

    if not _is_int_value(deny_reason_code):
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_TAXONOMY_INVALID",
            message="decision_metadata.deny_reason_code must be an integer.",
            field_path="output.decision_metadata.deny_reason_code",
        )

    if not isinstance(response_policy_label, str) or response_policy_label.strip() == "":
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_TAXONOMY_INVALID",
            message="decision_metadata.response_policy must be a non-empty string.",
            field_path="output.decision_metadata.response_policy",
        )

    state_mutation_blocked = mutation_flags.get("state_mutation_blocked")
    memory_write_blocked = mutation_flags.get("memory_write_blocked")
    if not isinstance(state_mutation_blocked, bool) or not isinstance(memory_write_blocked, bool):
        return DoctorFinding(
            level="error",
            check="dialogue_run_output_schema",
            error_code="BANANA_DIALOGUE_RUN_OUTPUT_TAXONOMY_INVALID",
            message="mutation_flags.state_mutation_blocked and mutation_flags.memory_write_blocked must be booleans.",
            field_path="output.mutation_flags",
        )

    if response_policy_label == "hard_block":
        if deny_reason_code < HARD_BLOCK_DENY_REASON_MIN or deny_reason_code > HARD_BLOCK_DENY_REASON_MAX:
            return DoctorFinding(
                level="error",
                check="dialogue_run_output_schema",
                error_code="BANANA_DIALOGUE_RUN_OUTPUT_TAXONOMY_INVALID",
                message=(
                    "hard_block response policy requires deny_reason_code in reserved range "
                    f"{HARD_BLOCK_DENY_REASON_MIN}-{HARD_BLOCK_DENY_REASON_MAX}."
                ),
                field_path="output.decision_metadata.deny_reason_code",
            )
        if state_mutation_blocked is not True or memory_write_blocked is not True:
            return DoctorFinding(
                level="error",
                check="dialogue_run_output_schema",
                error_code="BANANA_DIALOGUE_RUN_OUTPUT_TAXONOMY_INVALID",
                message="hard_block response policy requires both mutation flags to be true.",
                field_path="output.mutation_flags",
            )
        if not _is_int_value(memory_delta_applied) or memory_delta_applied != 0:
            return DoctorFinding(
                level="error",
                check="dialogue_run_output_schema",
                error_code="BANANA_DIALOGUE_RUN_OUTPUT_TAXONOMY_INVALID",
                message="hard_block response policy requires memory_delta_applied to be 0.",
                field_path="output.memory_delta_applied",
            )

    if response_policy_label == "safe_redirect":
        if HARD_BLOCK_DENY_REASON_MIN <= deny_reason_code <= HARD_BLOCK_DENY_REASON_MAX:
            return DoctorFinding(
                level="error",
                check="dialogue_run_output_schema",
                error_code="BANANA_DIALOGUE_RUN_OUTPUT_TAXONOMY_INVALID",
                message=(
                    "safe_redirect response policy must not use hard_block reserved deny_reason_code range "
                    f"{HARD_BLOCK_DENY_REASON_MIN}-{HARD_BLOCK_DENY_REASON_MAX}."
                ),
                field_path="output.decision_metadata.deny_reason_code",
            )
        if state_mutation_blocked is not True or memory_write_blocked is not True:
            return DoctorFinding(
                level="error",
                check="dialogue_run_output_schema",
                error_code="BANANA_DIALOGUE_RUN_OUTPUT_TAXONOMY_INVALID",
                message="safe_redirect response policy requires both mutation flags to be true.",
                field_path="output.mutation_flags",
            )

    return None


def _validate_dialogue_export_output_payload(payload: dict[str, object], schema_path: Path) -> DoctorFinding | None:
    if not schema_path.exists() or not schema_path.is_file():
        return DoctorFinding(
            level="error",
            check="dialogue_export_output_schema",
            error_code="BANANA_DIALOGUE_EXPORT_OUTPUT_SCHEMA_NOT_FOUND",
            message=f"Dialogue export output schema file not found: {schema_path}",
            field_path="schema.path",
        )

    try:
        schema_payload = json.loads(schema_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        return DoctorFinding(
            level="error",
            check="dialogue_export_output_schema",
            error_code="BANANA_DIALOGUE_EXPORT_OUTPUT_SCHEMA_UNREADABLE",
            message=f"Dialogue export output schema could not be parsed: {exc}",
            field_path="schema.path",
        )

    expected_version = schema_payload.get("schema_version")
    if expected_version != DIALOGUE_EXPORT_SCHEMA_VERSION:
        return DoctorFinding(
            level="error",
            check="dialogue_export_output_schema",
            error_code="BANANA_DIALOGUE_EXPORT_OUTPUT_SCHEMA_VERSION_UNSUPPORTED",
            message=f"Expected dialogue export output schema version 1, found {expected_version!r}.",
            field_path="schema.schema_version",
        )

    required_top_level = schema_payload.get("required_top_level", [])
    if not isinstance(required_top_level, list):
        return DoctorFinding(
            level="error",
            check="dialogue_export_output_schema",
            error_code="BANANA_DIALOGUE_EXPORT_OUTPUT_SCHEMA_INVALID",
            message="required_top_level must be an array in dialogue export output schema.",
            field_path="schema.required_top_level",
        )

    for key in required_top_level:
        if key not in payload:
            return DoctorFinding(
                level="error",
                check="dialogue_export_output_schema",
                error_code="BANANA_DIALOGUE_EXPORT_OUTPUT_SCHEMA_VALIDATION_FAILED",
                message=f"Dialogue export output is missing required top-level key {key!r}.",
                field_path=f"output.{key}",
            )

    if payload.get("schema_version") != DIALOGUE_EXPORT_SCHEMA_VERSION:
        return DoctorFinding(
            level="error",
            check="dialogue_export_output_schema",
            error_code="BANANA_DIALOGUE_EXPORT_OUTPUT_SCHEMA_VALIDATION_FAILED",
            message=(
                f"Dialogue export output schema_version must be {DIALOGUE_EXPORT_SCHEMA_VERSION}, "
                f"found {payload.get('schema_version')!r}."
            ),
            field_path="output.schema_version",
        )

    turn_record = payload.get("turn_record")
    if not isinstance(turn_record, dict):
        return DoctorFinding(
            level="error",
            check="dialogue_export_output_schema",
            error_code="BANANA_DIALOGUE_EXPORT_OUTPUT_SCHEMA_VALIDATION_FAILED",
            message="turn_record must be an object in dialogue export output.",
            field_path="output.turn_record",
        )

    required_turn_record_keys = schema_payload.get("required_turn_record_keys", [])
    if not isinstance(required_turn_record_keys, list):
        return DoctorFinding(
            level="error",
            check="dialogue_export_output_schema",
            error_code="BANANA_DIALOGUE_EXPORT_OUTPUT_SCHEMA_INVALID",
            message="required_turn_record_keys must be an array in dialogue export output schema.",
            field_path="schema.required_turn_record_keys",
        )

    for key in required_turn_record_keys:
        if key not in turn_record:
            return DoctorFinding(
                level="error",
                check="dialogue_export_output_schema",
                error_code="BANANA_DIALOGUE_EXPORT_OUTPUT_SCHEMA_VALIDATION_FAILED",
                message=f"turn_record is missing required key {key!r}.",
                field_path=f"output.turn_record.{key}",
            )

    return None


def _canonical_fixture_hash(payload: dict[str, object]) -> str:
    canonical = json.dumps(payload, sort_keys=True, separators=(",", ":")).encode("utf-8")
    return hashlib.sha256(canonical).hexdigest()


def _resolve_dialogue_run_symbol(native: ctypes.CDLL):
    for symbol_name in DIALOGUE_RUN_SYMBOL_CANDIDATES:
        symbol = getattr(native, symbol_name, None)
        if symbol is not None:
            return symbol_name, symbol
    return None, None


def _as_c_text(value: object, max_len: int) -> bytes:
    text = str(value) if value is not None else ""
    encoded = text.encode("utf-8", errors="ignore")
    return encoded[: max_len - 1]


def _decode_c_text(value: bytes) -> str:
    return bytes(value).split(b"\x00", 1)[0].decode("utf-8", errors="ignore")


def _build_dialogue_turn_input(payload: dict[str, object]) -> "_DialogueTurnInput":
    policy_context = payload.get("policy_context")
    if not isinstance(policy_context, dict):
        policy_context = {}

    prior_memory_delta = payload.get("prior_memory_delta")
    if not isinstance(prior_memory_delta, dict):
        prior_memory_delta = {}

    dialogue_input = _DialogueTurnInput()
    dialogue_input.npc_id = _as_c_text(payload.get("npc_id", ""), 64)
    dialogue_input.quest_state_id = _as_c_text(payload.get("quest_state_id", ""), 64)
    dialogue_input.region_id = _as_c_text(payload.get("region_id", ""), 64)
    dialogue_input.intent_id = _as_c_text(payload.get("intent_id", ""), 64)
    dialogue_input.policy_category = _as_c_text(policy_context.get("category", ""), 64)
    dialogue_input.policy_confidence_band = _as_c_text(policy_context.get("confidence_band", ""), 16)
    dialogue_input.policy_severity = _as_c_text(policy_context.get("severity", ""), 16)
    dialogue_input.prior_memory_trust_delta_q16 = int(prior_memory_delta.get("rapport_delta_q16", 0))
    dialogue_input.prior_memory_hostility_delta_q16 = int(prior_memory_delta.get("safety_flag_count", 0)) * 1024
    dialogue_input.prior_memory_helpfulness_delta_q16 = int(prior_memory_delta.get("trust_bucket", 0)) * 1024
    dialogue_input.sequence_tick = int(payload.get("sequence_tick", 0))
    return dialogue_input


def _k3h4_dialogue_run(args: argparse.Namespace) -> int:
    payload, input_error = _load_dialogue_input_payload(args)
    if input_error is not None:
        if args.strict:
            _emit_error_envelope(input_error)
        return 1

    validation_error = _validate_dialogue_input_payload(payload)
    if validation_error is not None:
        if args.strict:
            _emit_error_envelope(validation_error)
        return 1

    repo_root = _resolve_repo_root(Path.cwd().resolve())
    input_schema_path = _resolve_schema_path(args, repo_root)
    input_schema_error = _validate_dialogue_run_input_payload_against_schema(payload, input_schema_path)
    if input_schema_error is not None:
        if args.strict:
            _emit_error_envelope(input_schema_error)
        return 1

    native_path, native_source, checked_paths = _resolve_native_library(args, repo_root)
    if native_path is None:
        finding = DoctorFinding(
            level="error",
            check="dialogue_run_native_library_resolution",
            error_code="BANANA_DIALOGUE_RUN_NATIVE_LIB_NOT_FOUND",
            message=(
                "Unable to resolve Banana native library via --native-lib, BANANA_NATIVE_PATH, or autodiscovery."
                f" Checked paths: {checked_paths}."
            ),
            field_path="native.library",
        )
        if args.strict:
            _emit_error_envelope(finding)
        return 1

    try:
        native = ctypes.CDLL(str(native_path))
    except OSError as exc:
        finding = DoctorFinding(
            level="error",
            check="dialogue_run_native_load",
            error_code="BANANA_DIALOGUE_RUN_NATIVE_LOAD_FAILED",
            message=f"Failed to load native library at {native_path}: {exc}",
            field_path="native.library",
        )
        if args.strict:
            _emit_error_envelope(finding)
        return 1

    symbol_name, symbol = _resolve_dialogue_run_symbol(native)
    if symbol_name is None:
        finding = DoctorFinding(
            level="error",
            check="dialogue_run_native_binding",
            error_code="BANANA_DIALOGUE_RUN_SYMBOL_MISSING",
            message=(
                "Missing dialogue full-chain symbol in native library. "
                f"Checked candidates: {list(DIALOGUE_RUN_SYMBOL_CANDIDATES)}"
            ),
            field_path="native.symbols",
        )
        if args.strict:
            _emit_error_envelope(finding)
        return 1

    symbol.argtypes = [ctypes.POINTER(_DialogueTurnInput), ctypes.POINTER(_DialogueTurnOutput)]
    symbol.restype = ctypes.c_int

    dialogue_input = _build_dialogue_turn_input(payload)
    dialogue_output = _DialogueTurnOutput()
    status = int(symbol(ctypes.byref(dialogue_input), ctypes.byref(dialogue_output)))
    if status != 0:
        finding = DoctorFinding(
            level="error",
            check="dialogue_run_native_execute",
            error_code="BANANA_DIALOGUE_RUN_NATIVE_CONTRACT_FAILURE",
            message=f"{symbol_name} returned non-zero status {status}.",
            field_path="native.execution",
        )
        if args.strict:
            _emit_error_envelope(finding)
        return 1

    output_payload = {
        "decision_metadata": {
            "boundary_state": int(dialogue_output.boundary_state),
            "deny_reason_code": int(dialogue_output.deny_reason_code),
            "response_policy": _decode_c_text(dialogue_output.response_policy_label),
            "route_cluster_id": int(dialogue_output.route_cluster_id),
            "transition_id": int(dialogue_output.transition_id),
        },
        "fixture_hash": _canonical_fixture_hash(payload),
        "memory_delta_applied": int(dialogue_output.memory_delta_applied),
        "mutation_flags": {
            "memory_write_blocked": bool(dialogue_output.memory_write_blocked),
            "state_mutation_blocked": bool(dialogue_output.state_mutation_blocked),
        },
        "native_library_path": str(native_path),
        "native_resolution_source": native_source,
        "native_symbol": symbol_name,
        "npc_line_candidate": _decode_c_text(dialogue_output.npc_line_candidate),
        "observability": {
            "abi_contract_version": int(dialogue_output.abi_contract_version),
            "abi_crc32": int(dialogue_output.abi_crc32),
            "abi_decode_path": int(dialogue_output.abi_decode_path),
            "abi_payload_bytes": int(dialogue_output.abi_payload_bytes),
            "abi_status": int(dialogue_output.abi_status),
            "policy_action_selected": int(dialogue_output.policy_action_selected),
            "policy_category": payload["policy_context"]["category"],
            "policy_confidence_band": payload["policy_context"]["confidence_band"],
            "policy_severity": payload["policy_context"]["severity"],
            "selected_template_key": _decode_c_text(dialogue_output.selected_template_key),
        },
        "schema_version": DIALOGUE_FIXTURE_SCHEMA_VERSION,
        "status": "executed",
    }

    output_schema_path = _resolve_dialogue_run_output_schema_path(args, repo_root)
    output_schema_error = _validate_dialogue_run_output_payload(output_payload, output_schema_path)
    if output_schema_error is not None:
        if args.strict:
            _emit_error_envelope(output_schema_error)
        return 1

    print(json.dumps(output_payload, sort_keys=True, separators=(",", ":")))
    return 0


def _k3h4_dialogue_export(args: argparse.Namespace) -> int:
    payload, input_error = _load_dialogue_export_input_payload(args)
    if input_error is not None:
        if args.strict:
            _emit_error_envelope(input_error)
        return 1

    validation_error = _validate_dialogue_export_input_payload(payload)
    if validation_error is not None:
        if args.strict:
            _emit_error_envelope(validation_error)
        return 1

    repo_root = _resolve_repo_root(Path.cwd().resolve())
    input_schema_path = _resolve_dialogue_export_input_schema_path(args, repo_root)
    input_schema_error = _validate_dialogue_run_output_payload(payload, input_schema_path)
    if input_schema_error is not None:
        if args.strict:
            _emit_error_envelope(input_schema_error)
        return 1

    export_payload = {
        "artifact_type": "k3h4_dialogue_turn_export",
        "export_hash": hashlib.sha256(
            json.dumps(payload, sort_keys=True, separators=(",", ":")).encode("utf-8")
        ).hexdigest(),
        "schema_version": DIALOGUE_EXPORT_SCHEMA_VERSION,
        "turn_record": payload,
    }

    output_schema_path = _resolve_export_schema_path(args, repo_root)
    output_schema_error = _validate_dialogue_export_output_payload(export_payload, output_schema_path)
    if output_schema_error is not None:
        if args.strict:
            _emit_error_envelope(output_schema_error)
        return 1

    artifact_text = (
        json.dumps(export_payload, sort_keys=True, separators=(",", ":"))
        if args.format == "json"
        else _build_dialogue_export_csv(export_payload)
    )
    return _write_or_print_artifact(artifact_text, args.output_file)


def _load_dialogue_export_input_payload(
    args: argparse.Namespace,
) -> tuple[dict[str, object] | None, DoctorFinding | None]:
    raw = ""
    if args.input_file:
        input_path = Path(args.input_file).expanduser()
        if not input_path.is_absolute():
            input_path = (Path.cwd() / input_path).resolve()
        if not input_path.exists() or not input_path.is_file():
            return None, DoctorFinding(
                level="error",
                check="dialogue_export_input",
                error_code="BANANA_DIALOGUE_EXPORT_INPUT_FILE_NOT_FOUND",
                message=f"Input file not found: {input_path}",
                field_path="input_file",
            )
        try:
            raw = input_path.read_text(encoding="utf-8")
        except OSError as exc:
            return None, DoctorFinding(
                level="error",
                check="dialogue_export_input",
                error_code="BANANA_DIALOGUE_EXPORT_INPUT_FILE_READ_FAILED",
                message=f"Failed to read input file: {exc}",
                field_path="input_file",
            )
    else:
        raw = sys.stdin.read()

    if raw.strip() == "":
        return None, DoctorFinding(
            level="error",
            check="dialogue_export_input",
            error_code="BANANA_DIALOGUE_EXPORT_INPUT_EMPTY",
            message="No dialogue-run JSON was provided via stdin or --input-file.",
            field_path="stdin",
        )

    try:
        payload = json.loads(raw)
    except json.JSONDecodeError as exc:
        return None, DoctorFinding(
            level="error",
            check="dialogue_export_input",
            error_code="BANANA_DIALOGUE_EXPORT_INPUT_JSON_INVALID",
            message=f"Input is not valid JSON: {exc}",
            field_path="stdin",
        )

    if not isinstance(payload, dict):
        return None, DoctorFinding(
            level="error",
            check="dialogue_export_input",
            error_code="BANANA_DIALOGUE_EXPORT_INPUT_SHAPE_INVALID",
            message="Input JSON must be an object.",
            field_path="stdin",
        )

    return payload, None


def _validate_dialogue_export_input_payload(payload: dict[str, object]) -> DoctorFinding | None:
    schema_version = payload.get("schema_version")
    if schema_version != DIALOGUE_FIXTURE_SCHEMA_VERSION:
        return DoctorFinding(
            level="error",
            check="dialogue_export_input",
            error_code="BANANA_DIALOGUE_EXPORT_SCHEMA_VERSION_MISMATCH",
            message=(
                f"Expected dialogue run schema_version={DIALOGUE_FIXTURE_SCHEMA_VERSION}, "
                f"found {schema_version!r}."
            ),
            field_path="schema_version",
        )

    required_fields = {
        "fixture_hash",
        "native_library_path",
        "native_resolution_source",
        "native_symbol",
        "status",
    }
    missing = sorted(required_fields - set(payload.keys()))
    if missing:
        return DoctorFinding(
            level="error",
            check="dialogue_export_input",
            error_code="BANANA_DIALOGUE_EXPORT_FIELDS_MISSING",
            message=f"Dialogue run payload missing required fields: {missing}",
            field_path="input",
        )

    if payload.get("status") not in {"preflight_ready", "executed"}:
        return DoctorFinding(
            level="error",
            check="dialogue_export_input",
            error_code="BANANA_DIALOGUE_EXPORT_STATUS_INVALID",
            message="Dialogue run status must be one of: preflight_ready, executed.",
            field_path="status",
        )

    return None


def _build_dialogue_export_csv(export_payload: dict[str, object]) -> str:
    output = io.StringIO()
    writer = csv.writer(output, lineterminator="\n")

    turn_record = export_payload.get("turn_record")
    if not isinstance(turn_record, dict):
        turn_record = {}

    writer.writerow(
        [
            "artifact_type",
            "schema_version",
            "fixture_hash",
            "status",
            "native_symbol",
            "native_resolution_source",
            "native_library_path",
            "export_hash",
        ]
    )
    writer.writerow(
        [
            export_payload.get("artifact_type", ""),
            export_payload.get("schema_version", ""),
            turn_record.get("fixture_hash", ""),
            turn_record.get("status", ""),
            turn_record.get("native_symbol", ""),
            turn_record.get("native_resolution_source", ""),
            turn_record.get("native_library_path", ""),
            export_payload.get("export_hash", ""),
        ]
    )
    return output.getvalue()


def _resolve_run_output_schema_path(args: argparse.Namespace, repo_root: Path) -> Path:
    requested = Path(args.schema_path).expanduser()
    if requested.is_absolute():
        return requested
    return (repo_root / requested).resolve()


def _resolve_dialogue_run_output_schema_path(args: argparse.Namespace, repo_root: Path) -> Path:
    requested = Path(args.output_schema_path).expanduser()
    if requested.is_absolute():
        return requested
    return (repo_root / requested).resolve()


def _resolve_dialogue_export_input_schema_path(args: argparse.Namespace, repo_root: Path) -> Path:
    requested = Path(args.input_schema_path).expanduser()
    if requested.is_absolute():
        return requested
    return (repo_root / requested).resolve()


def _validate_run_output_payload(payload: dict[str, object], schema_path: Path) -> DoctorFinding | None:
    if not schema_path.exists() or not schema_path.is_file():
        return DoctorFinding(
            level="error",
            check="run_output_schema",
            error_code="BANANA_RUN_OUTPUT_SCHEMA_NOT_FOUND",
            message=f"Run output schema file not found: {schema_path}",
            field_path="schema.path",
        )

    try:
        schema_payload = json.loads(schema_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        return DoctorFinding(
            level="error",
            check="run_output_schema",
            error_code="BANANA_RUN_OUTPUT_SCHEMA_UNREADABLE",
            message=f"Run output schema could not be parsed: {exc}",
            field_path="schema.path",
        )

    expected_version = schema_payload.get("schema_version")
    if expected_version != SAMPLE_SCHEMA_VERSION:
        return DoctorFinding(
            level="error",
            check="run_output_schema",
            error_code="BANANA_RUN_OUTPUT_SCHEMA_VERSION_UNSUPPORTED",
            message=f"Expected output schema version 1, found {expected_version!r}.",
            field_path="schema.schema_version",
        )

    required_top_level = schema_payload.get("required_top_level", [])
    if not isinstance(required_top_level, list):
        return DoctorFinding(
            level="error",
            check="run_output_schema",
            error_code="BANANA_RUN_OUTPUT_SCHEMA_INVALID",
            message="required_top_level must be an array in run output schema.",
            field_path="schema.required_top_level",
        )

    for key in required_top_level:
        if key not in payload:
            return DoctorFinding(
                level="error",
                check="run_output_schema",
                error_code="BANANA_RUN_OUTPUT_SCHEMA_VALIDATION_FAILED",
                message=f"Run output missing required top-level key {key!r}.",
                field_path=f"output.{key}",
            )

    required_result_keys = schema_payload.get("required_result_keys", [])
    if not isinstance(required_result_keys, list):
        return DoctorFinding(
            level="error",
            check="run_output_schema",
            error_code="BANANA_RUN_OUTPUT_SCHEMA_INVALID",
            message="required_result_keys must be an array in run output schema.",
            field_path="schema.required_result_keys",
        )

    results = payload.get("results")
    if not isinstance(results, list):
        return DoctorFinding(
            level="error",
            check="run_output_schema",
            error_code="BANANA_RUN_OUTPUT_SCHEMA_VALIDATION_FAILED",
            message="Run output results must be an array.",
            field_path="output.results",
        )

    for index, result in enumerate(results):
        if not isinstance(result, dict):
            return DoctorFinding(
                level="error",
                check="run_output_schema",
                error_code="BANANA_RUN_OUTPUT_SCHEMA_VALIDATION_FAILED",
                message="Each run output result entry must be an object.",
                field_path=f"output.results[{index}]",
            )
        for key in required_result_keys:
            if key not in result:
                return DoctorFinding(
                    level="error",
                    check="run_output_schema",
                    error_code="BANANA_RUN_OUTPUT_SCHEMA_VALIDATION_FAILED",
                    message=f"Result is missing required key {key!r}.",
                    field_path=f"output.results[{index}].{key}",
                )

    return None


def _fixed_float(value: float) -> float:
    return float(f"{float(value):.{RUN_FLOAT_PRECISION}f}")


def _serialize_k3h4_output(output: "_K3h4Output") -> dict[str, object]:
    nodes: list[dict[str, object]] = []
    for node in output.nodes:
        nodes.append(
            {
                "coherence": int(node.coherence),
                "inradius": _fixed_float(node.inradius),
                "nearest_neighbor_distance": _fixed_float(node.nearest_neighbor_distance),
                "x": _fixed_float(node.x),
                "y": _fixed_float(node.y),
                "z": _fixed_float(node.z),
            }
        )

    centers: list[dict[str, object]] = []
    for center in output.centers:
        centers.append(
            {
                "center_q16": [int(value) for value in center.center_q16],
                "cluster_id": int(center.cluster_id),
                "member_count": int(center.member_count),
            }
        )

    radii: list[dict[str, int]] = []
    for radius in output.radii:
        radii.append(
            {
                "cluster_id": int(radius.cluster_id),
                "inscribed_radius_q16": int(radius.inscribed_radius_q16),
                "nearest_neighbor_distance_q16": int(radius.nearest_neighbor_distance_q16),
                "radius_state": int(radius.radius_state),
            }
        )

    weighted_voronoi_scores: list[dict[str, int]] = []
    for score in output.weighted_voronoi_scores:
        weighted_voronoi_scores.append(
            {
                "cluster_id": int(score.cluster_id),
                "distance_to_center_q16": int(score.distance_to_center_q16),
                "score_validity": int(score.score_validity),
                "vector_id": int(score.vector_id),
                "weighted_score_q16": int(score.weighted_score_q16),
            }
        )

    spectral_proxy: list[dict[str, int]] = []
    for spectral in output.spectral_proxy:
        spectral_proxy.append(
            {
                "amplitude_proxy_q16": int(spectral.amplitude_proxy_q16),
                "cluster_id": int(spectral.cluster_id),
                "frequency_proxy_q16": int(spectral.frequency_proxy_q16),
                "spectral_state": int(spectral.spectral_state),
            }
        )

    return {
        "alignment": int(output.alignment),
        "centers": centers,
        "cluster_count": int(output.cluster_count),
        "contract_status": int(output.contract_status),
        "dimensions": int(output.dimensions),
        "envelope_byte_order_tag": int(output.envelope_byte_order_tag),
        "envelope_contract_version": int(output.envelope_contract_version),
        "envelope_payload_bytes": int(output.envelope_payload_bytes),
        "envelope_payload_crc32": int(output.envelope_payload_crc32),
        "nodes": nodes,
        "observability": {
            "assignment_changes_last_iteration": int(output.observability.assignment_changes_last_iteration),
            "convergence_status": int(output.observability.convergence_status),
            "deterministic_hash": int(output.observability.deterministic_hash),
            "endianness_decode_path": int(output.observability.endianness_decode_path),
            "iteration_count": int(output.observability.iteration_count),
        },
        "radii": radii,
        "radial_stability": int(output.radial_stability),
        "spectral_proxy": spectral_proxy,
        "vector_count": int(output.vector_count),
        "weighted_voronoi_scores": weighted_voronoi_scores,
    }


class _VectorInput(ctypes.Structure):
    _fields_ = [(field_name, ctypes.c_int32) for field_name in VECTOR_INPUT_FIELDS]


class _ProjectionNode(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_float),
        ("y", ctypes.c_float),
        ("z", ctypes.c_float),
        ("coherence", ctypes.c_int32),
        ("inradius", ctypes.c_float),
        ("nearest_neighbor_distance", ctypes.c_float),
    ]


class _K3h4Center(ctypes.Structure):
    _fields_ = [
        ("cluster_id", ctypes.c_int32),
        ("member_count", ctypes.c_int32),
        ("center_q16", ctypes.c_int32 * 16),
    ]


class _K3h4Radius(ctypes.Structure):
    _fields_ = [
        ("cluster_id", ctypes.c_int32),
        ("nearest_neighbor_distance_q16", ctypes.c_int32),
        ("inscribed_radius_q16", ctypes.c_int32),
        ("radius_state", ctypes.c_int32),
    ]


class _WeightedVoronoiScore(ctypes.Structure):
    _fields_ = [
        ("vector_id", ctypes.c_int32),
        ("cluster_id", ctypes.c_int32),
        ("distance_to_center_q16", ctypes.c_int32),
        ("weighted_score_q16", ctypes.c_int32),
        ("score_validity", ctypes.c_int32),
    ]


class _SpectralProxy(ctypes.Structure):
    _fields_ = [
        ("cluster_id", ctypes.c_int32),
        ("frequency_proxy_q16", ctypes.c_int32),
        ("amplitude_proxy_q16", ctypes.c_int32),
        ("spectral_state", ctypes.c_int32),
    ]


class _K3h4Observability(ctypes.Structure):
    _fields_ = [
        ("convergence_status", ctypes.c_int32),
        ("iteration_count", ctypes.c_int32),
        ("assignment_changes_last_iteration", ctypes.c_int32),
        ("deterministic_hash", ctypes.c_int32),
        ("endianness_decode_path", ctypes.c_int32),
    ]


class _K3h4Output(ctypes.Structure):
    _fields_ = [
        ("dimensions", ctypes.c_int32),
        ("nodes", _ProjectionNode * 4),
        ("alignment", ctypes.c_int32),
        ("radial_stability", ctypes.c_int32),
        ("cluster_count", ctypes.c_int32),
        ("vector_count", ctypes.c_int32),
        ("centers", _K3h4Center * 4),
        ("radii", _K3h4Radius * 4),
        ("weighted_voronoi_scores", _WeightedVoronoiScore * 16),
        ("spectral_proxy", _SpectralProxy * 4),
        ("observability", _K3h4Observability),
        ("envelope_contract_version", ctypes.c_int32),
        ("envelope_byte_order_tag", ctypes.c_int32),
        ("envelope_payload_bytes", ctypes.c_int32),
        ("envelope_payload_crc32", ctypes.c_int32),
        ("contract_status", ctypes.c_int32),
    ]


class _DialogueTurnInput(ctypes.Structure):
    _fields_ = [
        ("npc_id", ctypes.c_char * 64),
        ("quest_state_id", ctypes.c_char * 64),
        ("region_id", ctypes.c_char * 64),
        ("intent_id", ctypes.c_char * 64),
        ("policy_category", ctypes.c_char * 64),
        ("policy_confidence_band", ctypes.c_char * 16),
        ("policy_severity", ctypes.c_char * 16),
        ("prior_memory_trust_delta_q16", ctypes.c_int32),
        ("prior_memory_hostility_delta_q16", ctypes.c_int32),
        ("prior_memory_helpfulness_delta_q16", ctypes.c_int32),
        ("sequence_tick", ctypes.c_int32),
    ]


class _DialogueTurnOutput(ctypes.Structure):
    _fields_ = [
        ("schema_version", ctypes.c_int32),
        ("route_cluster_id", ctypes.c_int32),
        ("boundary_state", ctypes.c_int32),
        ("transition_id", ctypes.c_int32),
        ("response_policy", ctypes.c_int32),
        ("deny_reason_code", ctypes.c_int32),
        ("state_mutation_blocked", ctypes.c_int32),
        ("memory_write_blocked", ctypes.c_int32),
        ("memory_delta_applied", ctypes.c_int32),
        ("policy_action_selected", ctypes.c_int32),
        ("abi_decode_path", ctypes.c_int32),
        ("abi_contract_version", ctypes.c_int32),
        ("abi_payload_bytes", ctypes.c_int32),
        ("abi_crc32", ctypes.c_int32),
        ("abi_status", ctypes.c_int32),
        ("response_policy_label", ctypes.c_char * 32),
        ("npc_line_candidate", ctypes.c_char * 160),
        ("selected_template_key", ctypes.c_char * 64),
    ]


def _k3h4_explain(args: argparse.Namespace) -> int:
    payload, input_error = _load_explain_input_payload(args)
    if input_error is not None:
        if args.strict:
            _emit_error_envelope(input_error)
        return 1

    validation_error = _validate_explain_input_payload(payload)
    if validation_error is not None:
        if args.strict:
            _emit_error_envelope(validation_error)
        return 1

    explanations: list[dict[str, object]] = []
    for index, result in enumerate(payload["results"]):
        explanations.append(_build_explain_record(index, result))

    output_payload = {
        "explanations": explanations,
        "input_sample_count": payload.get("input_sample_count", len(explanations)),
        "schema_version": EXPLAIN_SCHEMA_VERSION,
    }
    print(json.dumps(output_payload, sort_keys=True, separators=(",", ":")))
    return 0


def _load_explain_input_payload(args: argparse.Namespace) -> tuple[dict[str, object] | None, DoctorFinding | None]:
    raw = ""
    if args.input_file:
        input_path = Path(args.input_file).expanduser()
        if not input_path.is_absolute():
            input_path = (Path.cwd() / input_path).resolve()
        if not input_path.exists() or not input_path.is_file():
            return None, DoctorFinding(
                level="error",
                check="explain_input",
                error_code="BANANA_EXPLAIN_INPUT_FILE_NOT_FOUND",
                message=f"Input file not found: {input_path}",
                field_path="input_file",
            )
        try:
            raw = input_path.read_text(encoding="utf-8")
        except OSError as exc:
            return None, DoctorFinding(
                level="error",
                check="explain_input",
                error_code="BANANA_EXPLAIN_INPUT_FILE_READ_FAILED",
                message=f"Failed to read input file: {exc}",
                field_path="input_file",
            )
    else:
        raw = sys.stdin.read()

    if raw.strip() == "":
        return None, DoctorFinding(
            level="error",
            check="explain_input",
            error_code="BANANA_EXPLAIN_INPUT_EMPTY",
            message="No run output JSON was provided via stdin or --input-file.",
            field_path="stdin",
        )

    try:
        payload = json.loads(raw)
    except json.JSONDecodeError as exc:
        return None, DoctorFinding(
            level="error",
            check="explain_input",
            error_code="BANANA_EXPLAIN_INPUT_JSON_INVALID",
            message=f"Input is not valid JSON: {exc}",
            field_path="stdin",
        )

    if not isinstance(payload, dict):
        return None, DoctorFinding(
            level="error",
            check="explain_input",
            error_code="BANANA_EXPLAIN_INPUT_SHAPE_INVALID",
            message="Input JSON must be an object.",
            field_path="stdin",
        )

    return payload, None


def _validate_explain_input_payload(payload: dict[str, object]) -> DoctorFinding | None:
    schema_version = payload.get("schema_version")
    if schema_version != SAMPLE_SCHEMA_VERSION:
        return DoctorFinding(
            level="error",
            check="explain_input",
            error_code="BANANA_EXPLAIN_SCHEMA_VERSION_MISMATCH",
            message=f"Expected run schema_version=1, found {schema_version!r}.",
            field_path="schema_version",
        )

    results = payload.get("results")
    if not isinstance(results, list) or len(results) == 0:
        return DoctorFinding(
            level="error",
            check="explain_input",
            error_code="BANANA_EXPLAIN_RESULTS_MISSING",
            message="Input must include non-empty results array.",
            field_path="results",
        )

    required_keys = {
        "alignment",
        "cluster_count",
        "contract_status",
        "dimensions",
        "observability",
        "radii",
        "spectral_proxy",
        "weighted_voronoi_scores",
    }

    for index, result in enumerate(results):
        if not isinstance(result, dict):
            return DoctorFinding(
                level="error",
                check="explain_input",
                error_code="BANANA_EXPLAIN_RESULT_SHAPE_INVALID",
                message="Each run result must be an object.",
                field_path=f"results[{index}]",
            )
        missing = sorted(required_keys - set(result.keys()))
        if missing:
            return DoctorFinding(
                level="error",
                check="explain_input",
                error_code="BANANA_EXPLAIN_RESULT_FIELDS_MISSING",
                message=f"Run result is missing required fields: {missing}",
                field_path=f"results[{index}]",
            )

    return None


def _build_explain_record(index: int, result: dict[str, object]) -> dict[str, object]:
    alignment = int(result["alignment"])
    contract_status = int(result["contract_status"])
    radial_stability = int(result.get("radial_stability", 0))
    observability = result["observability"]
    if not isinstance(observability, dict):
        observability = {}

    convergence_status = int(observability.get("convergence_status", -1))
    iteration_count = int(observability.get("iteration_count", -1))
    assignment_changes = int(observability.get("assignment_changes_last_iteration", -1))
    deterministic_hash = int(observability.get("deterministic_hash", 0))

    observations: list[dict[str, object]] = [
        {
            "field_path": f"results[{index}].contract_status",
            "interpretation": (
                "native contract validation passed"
                if contract_status == 0
                else "native contract reported a failure"
            ),
            "value": contract_status,
        },
        {
            "field_path": f"results[{index}].alignment",
            "interpretation": _interpret_alignment(alignment),
            "value": alignment,
        },
        {
            "field_path": f"results[{index}].radial_stability",
            "interpretation": _interpret_radial_stability(radial_stability),
            "value": radial_stability,
        },
        {
            "field_path": f"results[{index}].observability.convergence_status",
            "interpretation": _interpret_convergence_status(convergence_status),
            "value": convergence_status,
        },
        {
            "field_path": f"results[{index}].observability.iteration_count",
            "interpretation": "number of clustering iterations used by native run",
            "value": iteration_count,
        },
        {
            "field_path": f"results[{index}].observability.assignment_changes_last_iteration",
            "interpretation": "cluster assignment churn during final iteration",
            "value": assignment_changes,
        },
        {
            "field_path": f"results[{index}].observability.deterministic_hash",
            "interpretation": "deterministic fingerprint for regression and artifact tracking",
            "value": deterministic_hash,
        },
    ]

    radii = result["radii"]
    if isinstance(radii, list) and radii:
        radius_states = []
        min_radius_q16 = None
        for radius in radii:
            if isinstance(radius, dict):
                radius_states.append(int(radius.get("radius_state", -1)))
                current = int(radius.get("inscribed_radius_q16", 0))
                min_radius_q16 = current if min_radius_q16 is None else min(min_radius_q16, current)
        observations.append(
            {
                "field_path": f"results[{index}].radii[*].radius_state",
                "interpretation": _interpret_radius_states(radius_states),
                "value": radius_states,
            }
        )
        observations.append(
            {
                "field_path": f"results[{index}].radii[*].inscribed_radius_q16",
                "interpretation": "minimum inscribed radius across clusters (Q16 fixed-point)",
                "value": min_radius_q16,
            }
        )

    scores = result["weighted_voronoi_scores"]
    if isinstance(scores, list) and scores:
        valid_count = 0
        invalid_count = 0
        min_score = None
        max_score = None
        for score in scores:
            if not isinstance(score, dict):
                continue
            if int(score.get("score_validity", 1)) == 0:
                valid_count += 1
            else:
                invalid_count += 1
            current = int(score.get("weighted_score_q16", 0))
            min_score = current if min_score is None else min(min_score, current)
            max_score = current if max_score is None else max(max_score, current)
        observations.append(
            {
                "field_path": f"results[{index}].weighted_voronoi_scores[*].score_validity",
                "interpretation": "assignment validity across weighted Voronoi scores",
                "value": {"invalid": invalid_count, "valid": valid_count},
            }
        )
        observations.append(
            {
                "field_path": f"results[{index}].weighted_voronoi_scores[*].weighted_score_q16",
                "interpretation": "assignment score range in Q16 fixed-point",
                "value": {"max": max_score, "min": min_score},
            }
        )

    spectral_proxy = result["spectral_proxy"]
    if isinstance(spectral_proxy, list) and spectral_proxy:
        spectral_states = []
        min_frequency = None
        max_frequency = None
        for spectral in spectral_proxy:
            if not isinstance(spectral, dict):
                continue
            spectral_states.append(int(spectral.get("spectral_state", -1)))
            frequency = int(spectral.get("frequency_proxy_q16", 0))
            min_frequency = frequency if min_frequency is None else min(min_frequency, frequency)
            max_frequency = frequency if max_frequency is None else max(max_frequency, frequency)
        observations.append(
            {
                "field_path": f"results[{index}].spectral_proxy[*].spectral_state",
                "interpretation": _interpret_spectral_states(spectral_states),
                "value": spectral_states,
            }
        )
        observations.append(
            {
                "field_path": f"results[{index}].spectral_proxy[*].frequency_proxy_q16",
                "interpretation": "spectral frequency proxy range in Q16 fixed-point",
                "value": {"max": max_frequency, "min": min_frequency},
            }
        )

    summary = (
        f"sample {index}: contract_status={contract_status}, alignment={alignment}, "
        f"radial_stability={radial_stability}, convergence_status={convergence_status}, "
        f"iteration_count={iteration_count}"
    )

    return {
        "index": index,
        "observations": observations,
        "summary": summary,
    }


def _interpret_alignment(alignment: int) -> str:
    if alignment >= 90:
        return "high alignment signal"
    if alignment >= 70:
        return "moderate alignment signal"
    return "low alignment signal"


def _interpret_radial_stability(radial_stability: int) -> str:
    if radial_stability >= 90:
        return "radius structure is highly stable"
    if radial_stability >= 70:
        return "radius structure is moderately stable"
    return "radius structure may be unstable"


def _interpret_convergence_status(convergence_status: int) -> str:
    if convergence_status == 0:
        return "native solver reports converged state"
    return "native solver reports non-converged or exceptional state"


def _interpret_radius_states(radius_states: list[int]) -> str:
    if not radius_states:
        return "no radius states present"
    if all(state == 0 for state in radius_states):
        return "all clusters report nominal radius state"
    return "one or more clusters report clamped/single-cluster radius behavior"


def _interpret_spectral_states(spectral_states: list[int]) -> str:
    if not spectral_states:
        return "no spectral proxy states present"
    if all(state == 0 for state in spectral_states):
        return "spectral proxy remained in nominal state"
    return "spectral proxy includes floor-applied or disabled states"


def _k3h4_export(args: argparse.Namespace) -> int:
    repo_root = _resolve_repo_root(Path.cwd().resolve())
    payload, input_error = _load_export_input_payload(args)
    if input_error is not None:
        if args.strict:
            _emit_error_envelope(input_error)
        return 1

    validation_error = _validate_export_input_payload(payload)
    if validation_error is not None:
        if args.strict:
            _emit_error_envelope(validation_error)
        return 1

    export_payload = _build_export_payload(payload)

    schema_path = _resolve_export_schema_path(args, repo_root)
    schema_error = _validate_export_output_payload(export_payload, schema_path)
    if schema_error is not None:
        if args.strict:
            _emit_error_envelope(schema_error)
        return 1

    artifact_text = (
        json.dumps(export_payload, sort_keys=True, separators=(",", ":"))
        if args.format == "json"
        else _build_export_csv(export_payload)
    )

    return _write_or_print_artifact(artifact_text, args.output_file)


def _load_export_input_payload(args: argparse.Namespace) -> tuple[dict[str, object] | None, DoctorFinding | None]:
    raw = ""
    if args.input_file:
        input_path = Path(args.input_file).expanduser()
        if not input_path.is_absolute():
            input_path = (Path.cwd() / input_path).resolve()
        if not input_path.exists() or not input_path.is_file():
            return None, DoctorFinding(
                level="error",
                check="export_input",
                error_code="BANANA_EXPORT_INPUT_FILE_NOT_FOUND",
                message=f"Input file not found: {input_path}",
                field_path="input_file",
            )
        try:
            raw = input_path.read_text(encoding="utf-8")
        except OSError as exc:
            return None, DoctorFinding(
                level="error",
                check="export_input",
                error_code="BANANA_EXPORT_INPUT_FILE_READ_FAILED",
                message=f"Failed to read input file: {exc}",
                field_path="input_file",
            )
    else:
        raw = sys.stdin.read()

    if raw.strip() == "":
        return None, DoctorFinding(
            level="error",
            check="export_input",
            error_code="BANANA_EXPORT_INPUT_EMPTY",
            message="No run output JSON was provided via stdin or --input-file.",
            field_path="stdin",
        )

    try:
        payload = json.loads(raw)
    except json.JSONDecodeError as exc:
        return None, DoctorFinding(
            level="error",
            check="export_input",
            error_code="BANANA_EXPORT_INPUT_JSON_INVALID",
            message=f"Input is not valid JSON: {exc}",
            field_path="stdin",
        )

    if not isinstance(payload, dict):
        return None, DoctorFinding(
            level="error",
            check="export_input",
            error_code="BANANA_EXPORT_INPUT_SHAPE_INVALID",
            message="Input JSON must be an object.",
            field_path="stdin",
        )

    return payload, None


def _validate_export_input_payload(payload: dict[str, object]) -> DoctorFinding | None:
    schema_version = payload.get("schema_version")
    if schema_version != SAMPLE_SCHEMA_VERSION:
        return DoctorFinding(
            level="error",
            check="export_input",
            error_code="BANANA_EXPORT_SCHEMA_VERSION_MISMATCH",
            message=f"Expected run schema_version=1, found {schema_version!r}.",
            field_path="schema_version",
        )

    required_top = {
        "input_sample_count",
        "native_library_path",
        "native_resolution_source",
        "results",
    }
    missing_top = sorted(required_top - set(payload.keys()))
    if missing_top:
        return DoctorFinding(
            level="error",
            check="export_input",
            error_code="BANANA_EXPORT_INPUT_FIELDS_MISSING",
            message=f"Run payload missing required fields: {missing_top}",
            field_path="input",
        )

    results = payload.get("results")
    if not isinstance(results, list) or len(results) == 0:
        return DoctorFinding(
            level="error",
            check="export_input",
            error_code="BANANA_EXPORT_RESULTS_MISSING",
            message="Run payload must include non-empty results array.",
            field_path="results",
        )

    for index, result in enumerate(results):
        if not isinstance(result, dict):
            return DoctorFinding(
                level="error",
                check="export_input",
                error_code="BANANA_EXPORT_RESULT_SHAPE_INVALID",
                message="Each result must be an object.",
                field_path=f"results[{index}]",
            )

    return None


def _build_export_payload(run_payload: dict[str, object]) -> dict[str, object]:
    canonical_input = json.dumps(run_payload, sort_keys=True, separators=(",", ":")).encode("utf-8")
    deterministic_hash = hashlib.sha256(canonical_input).hexdigest()

    results = run_payload["results"]
    assert isinstance(results, list)

    alignments = [int(item.get("alignment", 0)) for item in results if isinstance(item, dict)]
    radial_stabilities = [int(item.get("radial_stability", 0)) for item in results if isinstance(item, dict)]
    cluster_counts = [int(item.get("cluster_count", 0)) for item in results if isinstance(item, dict)]

    contract_failures = 0
    for item in results:
        if isinstance(item, dict) and int(item.get("contract_status", 1)) != 0:
            contract_failures += 1

    summary = {
        "average_alignment": _fixed_float(sum(alignments) / len(alignments)) if alignments else _fixed_float(0.0),
        "average_radial_stability": (
            _fixed_float(sum(radial_stabilities) / len(radial_stabilities)) if radial_stabilities else _fixed_float(0.0)
        ),
        "contract_failures": contract_failures,
        "result_count": len(results),
        "max_cluster_count": max(cluster_counts) if cluster_counts else 0,
    }

    return {
        "artifact_type": "k3h4_export",
        "metadata": {
            "deterministic_hash": deterministic_hash,
            "run_configuration": {
                "input_sample_count": int(run_payload.get("input_sample_count", len(results))),
                "native_library_path": str(run_payload.get("native_library_path", "")),
                "native_resolution_source": str(run_payload.get("native_resolution_source", "")),
            },
        },
        "results": results,
        "schema_version": EXPORT_SCHEMA_VERSION,
        "summary": summary,
    }


def _resolve_export_schema_path(args: argparse.Namespace, repo_root: Path) -> Path:
    requested = Path(args.schema_path).expanduser()
    if requested.is_absolute():
        return requested
    return (repo_root / requested).resolve()


def _validate_export_output_payload(payload: dict[str, object], schema_path: Path) -> DoctorFinding | None:
    if not schema_path.exists() or not schema_path.is_file():
        return DoctorFinding(
            level="error",
            check="export_schema",
            error_code="BANANA_EXPORT_SCHEMA_NOT_FOUND",
            message=f"Export schema file not found: {schema_path}",
            field_path="schema.path",
        )

    try:
        schema_payload = json.loads(schema_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        return DoctorFinding(
            level="error",
            check="export_schema",
            error_code="BANANA_EXPORT_SCHEMA_UNREADABLE",
            message=f"Export schema could not be parsed: {exc}",
            field_path="schema.path",
        )

    expected_version = schema_payload.get("schema_version")
    if expected_version != EXPORT_SCHEMA_VERSION:
        return DoctorFinding(
            level="error",
            check="export_schema",
            error_code="BANANA_EXPORT_SCHEMA_VERSION_UNSUPPORTED",
            message=f"Expected export schema version 1, found {expected_version!r}.",
            field_path="schema.schema_version",
        )

    required_top_level = schema_payload.get("required_top_level", [])
    if not isinstance(required_top_level, list):
        return DoctorFinding(
            level="error",
            check="export_schema",
            error_code="BANANA_EXPORT_SCHEMA_INVALID",
            message="required_top_level must be an array in export schema.",
            field_path="schema.required_top_level",
        )

    for key in required_top_level:
        if key not in payload:
            return DoctorFinding(
                level="error",
                check="export_schema",
                error_code="BANANA_EXPORT_SCHEMA_VALIDATION_FAILED",
                message=f"Export output missing required top-level key {key!r}.",
                field_path=f"output.{key}",
            )

    return None


def _build_export_csv(export_payload: dict[str, object]) -> str:
    output = io.StringIO()
    writer = csv.DictWriter(
        output,
        fieldnames=[
            "result_index",
            "alignment",
            "cluster_count",
            "contract_status",
            "dimensions",
            "radial_stability",
            "iteration_count",
            "convergence_status",
            "deterministic_hash",
            "native_resolution_source",
            "native_library_path",
        ],
    )
    writer.writeheader()

    metadata = export_payload.get("metadata", {})
    deterministic_hash = ""
    native_resolution_source = ""
    native_library_path = ""
    if isinstance(metadata, dict):
        deterministic_hash = str(metadata.get("deterministic_hash", ""))
        run_configuration = metadata.get("run_configuration", {})
        if isinstance(run_configuration, dict):
            native_resolution_source = str(run_configuration.get("native_resolution_source", ""))
            native_library_path = str(run_configuration.get("native_library_path", ""))

    results = export_payload.get("results", [])
    if isinstance(results, list):
        for index, result in enumerate(results):
            if not isinstance(result, dict):
                continue
            observability = result.get("observability", {})
            iteration_count = ""
            convergence_status = ""
            if isinstance(observability, dict):
                iteration_count = observability.get("iteration_count", "")
                convergence_status = observability.get("convergence_status", "")

            writer.writerow(
                {
                    "result_index": index,
                    "alignment": result.get("alignment", ""),
                    "cluster_count": result.get("cluster_count", ""),
                    "contract_status": result.get("contract_status", ""),
                    "dimensions": result.get("dimensions", ""),
                    "radial_stability": result.get("radial_stability", ""),
                    "iteration_count": iteration_count,
                    "convergence_status": convergence_status,
                    "deterministic_hash": deterministic_hash,
                    "native_resolution_source": native_resolution_source,
                    "native_library_path": native_library_path,
                }
            )

    return output.getvalue()


def _write_or_print_artifact(artifact_text: str, output_file: str | None) -> int:
    if output_file:
        target = Path(output_file).expanduser()
        if not target.is_absolute():
            target = (Path.cwd() / target).resolve()
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(artifact_text, encoding="utf-8")
        print(str(target))
        return 0

    print(artifact_text, end="")
    if not artifact_text.endswith("\n"):
        print()
    return 0


def _k3h4_doctor(args: argparse.Namespace) -> int:
    findings: list[DoctorFinding] = []
    repo_root = _resolve_repo_root(Path.cwd().resolve())

    py_tuple = (sys.version_info.major, sys.version_info.minor)
    if py_tuple < REQUIRED_PYTHON:
        findings.append(
            DoctorFinding(
                level="error",
                check="python_runtime",
                error_code="BANANA_DOCTOR_PYTHON_UNSUPPORTED",
                message=(
                    f"Python {REQUIRED_PYTHON[0]}.{REQUIRED_PYTHON[1]}+ is required, "
                    f"found {sys.version_info.major}.{sys.version_info.minor}."
                ),
                field_path="python.version",
            )
        )

    system = platform.system().lower()
    linux_baseline = system == "linux"
    if not linux_baseline:
        findings.append(
            DoctorFinding(
                level="warning",
                check="platform_baseline",
                error_code="BANANA_DOCTOR_PLATFORM_EXPERIMENTAL",
                message="Linux is the V1 support baseline; non-Linux execution is experimental.",
                field_path="platform.system",
            )
        )

    native_path, native_source, checked_paths = _resolve_native_library(args, repo_root)
    if native_path is None:
        findings.append(
            DoctorFinding(
                level="error",
                check="native_library_resolution",
                error_code="BANANA_DOCTOR_NATIVE_LIB_NOT_FOUND",
                message="Unable to resolve Banana native library via configured resolution chain.",
                field_path="native.library",
            )
        )

    abi_version: int | None = None
    ping_status: int | None = None
    if native_path is not None:
        try:
            native = ctypes.CDLL(str(native_path))
        except OSError as exc:
            findings.append(
                DoctorFinding(
                    level="error",
                    check="native_abi_handshake",
                    error_code="BANANA_DOCTOR_NATIVE_LOAD_FAILED",
                    message=f"Failed to load native library: {exc}",
                    field_path="native.library",
                )
            )
        else:
            abi_fn = getattr(native, "banana_native_v3_abi_version", None)
            ping_fn = getattr(native, "banana_native_v3_ping", None)
            if abi_fn is None:
                findings.append(
                    DoctorFinding(
                        level="error",
                        check="native_abi_handshake",
                        error_code="BANANA_DOCTOR_ABI_SYMBOL_MISSING",
                        message="Missing symbol banana_native_v3_abi_version in native library.",
                        field_path="native.symbols.banana_native_v3_abi_version",
                    )
                )
            else:
                abi_fn.restype = ctypes.c_int
                abi_version = int(abi_fn())
                if abi_version != EXPECTED_ABI_VERSION:
                    findings.append(
                        DoctorFinding(
                            level="error",
                            check="native_abi_handshake",
                            error_code="BANANA_DOCTOR_ABI_VERSION_MISMATCH",
                            message=(
                                f"Expected ABI version {EXPECTED_ABI_VERSION}, found {abi_version}."
                            ),
                            field_path="native.abi_version",
                        )
                    )

            if ping_fn is None:
                findings.append(
                    DoctorFinding(
                        level="error",
                        check="native_abi_handshake",
                        error_code="BANANA_DOCTOR_PING_SYMBOL_MISSING",
                        message="Missing symbol banana_native_v3_ping in native library.",
                        field_path="native.symbols.banana_native_v3_ping",
                    )
                )
            else:
                ping_fn.restype = ctypes.c_int
                ping_status = int(ping_fn())
                if ping_status != EXPECTED_PING_STATUS:
                    findings.append(
                        DoctorFinding(
                            level="error",
                            check="native_abi_handshake",
                            error_code="BANANA_DOCTOR_PING_FAILED",
                            message=(
                                f"Expected ping status {EXPECTED_PING_STATUS}, found {ping_status}."
                            ),
                            field_path="native.ping_status",
                        )
                    )

    schema_path = _resolve_schema_path(args, repo_root)
    schema_version = None
    if not schema_path.exists() or not schema_path.is_file():
        findings.append(
            DoctorFinding(
                level="error",
                check="schema_presence",
                error_code="BANANA_DOCTOR_SCHEMA_NOT_FOUND",
                message=f"Schema file not found at {schema_path}",
                field_path="schema.path",
            )
        )
    else:
        try:
            with schema_path.open("r", encoding="utf-8") as handle:
                schema_payload = json.load(handle)
        except (OSError, json.JSONDecodeError) as exc:
            findings.append(
                DoctorFinding(
                    level="error",
                    check="schema_presence",
                    error_code="BANANA_DOCTOR_SCHEMA_UNREADABLE",
                    message=f"Schema file could not be parsed: {exc}",
                    field_path="schema.path",
                )
            )
        else:
            schema_version = schema_payload.get("schema_version")
            if schema_version != 1:
                findings.append(
                    DoctorFinding(
                        level="error",
                        check="schema_presence",
                        error_code="BANANA_DOCTOR_SCHEMA_VERSION_UNSUPPORTED",
                        message=f"Expected schema_version=1, found {schema_version!r}.",
                        field_path="schema.schema_version",
                    )
                )

    errors = [finding for finding in findings if _is_error(finding)]
    warnings = [finding for finding in findings if finding.level == "warning"]

    if args.strict and errors:
        for finding in errors:
            _emit_error_envelope(finding)

    result = {
        "ok": len(errors) == 0,
        "python": {
            "required_min": f"{REQUIRED_PYTHON[0]}.{REQUIRED_PYTHON[1]}",
            "current": f"{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}",
        },
        "platform": {
            "system": system,
            "linux_v1_baseline": True,
            "linux_baseline_met": linux_baseline,
        },
        "native": {
            "resolution_source": native_source,
            "resolved_path": str(native_path) if native_path else None,
            "abi_version": abi_version,
            "ping_status": ping_status,
            "checked_paths": checked_paths,
        },
        "schema": {
            "path": str(schema_path),
            "schema_version": schema_version,
        },
        "warnings": [_as_payload(finding) for finding in warnings],
        "errors": [_as_payload(finding) for finding in errors],
    }
    print(json.dumps(result, indent=2, sort_keys=True))
    return 0 if not errors else 1


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="banana",
        description="Banana CLI (Python scaffold)",
    )

    subparsers = parser.add_subparsers(dest="command")

    k3h4_parser = subparsers.add_parser("k3h4", help="K3H4 command group")
    k3h4_parser.set_defaults(handler=_k3h4_group_help, _parser=k3h4_parser)
    k3h4_subparsers = k3h4_parser.add_subparsers(dest="k3h4_command")

    def add_k3h4_subcommand(name: str, help_text: str, handler, configure=None):
        cmd = k3h4_subparsers.add_parser(name, help=help_text)
        cmd.add_argument(
            "--api-url",
            default=DEFAULT_API_URL,
            help=f"API base URL override (default: {DEFAULT_API_URL})",
        )
        if configure is not None:
            configure(cmd)
        cmd.set_defaults(handler=handler)

    add_k3h4_subcommand(
        "sample",
        "Generate deterministic sample dataset",
        _k3h4_sample,
        configure=lambda cmd: (
            cmd.add_argument(
                "--seed",
                type=int,
                default=42,
                help="Seed for deterministic sample generation",
            ),
            cmd.add_argument(
                "--count",
                type=int,
                default=4,
                help="Number of sample vectors to emit",
            ),
            cmd.add_argument(
                "--dims",
                type=int,
                default=16,
                help="network_dimensions value (1..16)",
            ),
            cmd.add_argument(
                "--preset",
                choices=("baseline", "combat"),
                default=DEFAULT_SAMPLE_PRESET,
                help="Distribution preset",
            ),
        ),
    )
    add_k3h4_subcommand(
        "dialogue-sample",
        "Generate deterministic single-turn dialogue fixture",
        _k3h4_dialogue_sample,
        configure=lambda cmd: (
            cmd.add_argument(
                "--seed",
                type=int,
                default=42,
                help="Seed for deterministic dialogue fixture generation",
            ),
            cmd.add_argument(
                "--preset",
                choices=DIALOGUE_SAMPLE_PRESETS,
                default="pilot-edda",
                help="Fixture preset",
            ),
        ),
    )
    add_k3h4_subcommand(
        "run",
        "Execute K3H4 pipeline via native-direct ctypes",
        _k3h4_run,
        configure=lambda cmd: (
            cmd.add_argument(
                "--input-file",
                help="Optional JSON input file path; stdin remains the primary input path",
            ),
            cmd.add_argument(
                "--native-lib",
                help="Explicit native library path; highest precedence in resolution chain",
            ),
            cmd.add_argument(
                "--schema-path",
                default=str(RUN_OUTPUT_SCHEMA_RELATIVE),
                help=(
                    "Canonical run output schema path "
                    f"(default: {RUN_OUTPUT_SCHEMA_RELATIVE})"
                ),
            ),
            cmd.add_argument(
                "--strict",
                action=argparse.BooleanOptionalAction,
                default=True,
                help="Emit machine-readable JSON error envelopes on stderr when checks fail",
            ),
        ),
    )
    add_k3h4_subcommand(
        "dialogue-run",
        "Preflight dialogue full-chain execution contract via native symbol binding",
        _k3h4_dialogue_run,
        configure=lambda cmd: (
            cmd.add_argument(
                "--input-file",
                help="Optional dialogue fixture JSON file path; stdin remains the primary input path",
            ),
            cmd.add_argument(
                "--native-lib",
                help="Explicit native library path; highest precedence in resolution chain",
            ),
            cmd.add_argument(
                "--schema-path",
                default=str(DIALOGUE_RUN_INPUT_SCHEMA_RELATIVE),
                help=(
                    "Canonical dialogue-run input schema path "
                    f"(default: {DIALOGUE_RUN_INPUT_SCHEMA_RELATIVE})"
                ),
            ),
            cmd.add_argument(
                "--output-schema-path",
                default=str(DIALOGUE_RUN_OUTPUT_SCHEMA_RELATIVE),
                help=(
                    "Canonical dialogue-run output schema path "
                    f"(default: {DIALOGUE_RUN_OUTPUT_SCHEMA_RELATIVE})"
                ),
            ),
            cmd.add_argument(
                "--strict",
                action=argparse.BooleanOptionalAction,
                default=True,
                help="Emit machine-readable JSON error envelopes on stderr when checks fail",
            ),
        ),
    )
    add_k3h4_subcommand(
        "dialogue-export",
        "Export machine-consumable dialogue-turn artifacts",
        _k3h4_dialogue_export,
        configure=lambda cmd: (
            cmd.add_argument(
                "--input-file",
                help="Optional dialogue-run JSON file path; stdin remains the primary input path",
            ),
            cmd.add_argument(
                "--format",
                choices=DIALOGUE_EXPORT_FORMATS,
                default="json",
                help="Artifact format to emit (default: json)",
            ),
            cmd.add_argument(
                "--output-file",
                help="Optional output file path; defaults to stdout",
            ),
            cmd.add_argument(
                "--input-schema-path",
                default=str(DIALOGUE_RUN_OUTPUT_SCHEMA_RELATIVE),
                help=(
                    "Canonical dialogue-export input schema path "
                    f"(default: {DIALOGUE_RUN_OUTPUT_SCHEMA_RELATIVE})"
                ),
            ),
            cmd.add_argument(
                "--schema-path",
                default=str(DIALOGUE_EXPORT_OUTPUT_SCHEMA_RELATIVE),
                help=(
                    "Canonical dialogue-export output schema path "
                    f"(default: {DIALOGUE_EXPORT_OUTPUT_SCHEMA_RELATIVE})"
                ),
            ),
            cmd.add_argument(
                "--strict",
                action=argparse.BooleanOptionalAction,
                default=True,
                help="Emit machine-readable JSON error envelopes on stderr when checks fail",
            ),
        ),
    )
    add_k3h4_subcommand(
        "explain",
        "Interpret K3H4 run output into field-referenced analysis",
        _k3h4_explain,
        configure=lambda cmd: (
            cmd.add_argument(
                "--input-file",
                help="Optional run-output JSON file path; stdin remains the primary input path",
            ),
            cmd.add_argument(
                "--strict",
                action=argparse.BooleanOptionalAction,
                default=True,
                help="Emit machine-readable JSON error envelopes on stderr when checks fail",
            ),
        ),
    )
    add_k3h4_subcommand(
        "export",
        "Export machine-consumable K3H4 artifacts",
        _k3h4_export,
        configure=lambda cmd: (
            cmd.add_argument(
                "--input-file",
                help="Optional run-output JSON file path; stdin remains the primary input path",
            ),
            cmd.add_argument(
                "--format",
                choices=("json", "csv"),
                default="json",
                help="Artifact format to emit (default: json)",
            ),
            cmd.add_argument(
                "--output-file",
                help="Optional output file path; defaults to stdout",
            ),
            cmd.add_argument(
                "--schema-path",
                default=str(EXPORT_OUTPUT_SCHEMA_RELATIVE),
                help=(
                    "Canonical export schema path "
                    f"(default: {EXPORT_OUTPUT_SCHEMA_RELATIVE})"
                ),
            ),
            cmd.add_argument(
                "--strict",
                action=argparse.BooleanOptionalAction,
                default=True,
                help="Emit machine-readable JSON error envelopes on stderr when checks fail",
            ),
        ),
    )
    add_k3h4_subcommand(
        "doctor",
        "Run read-only preflight diagnostics",
        _k3h4_doctor,
        configure=lambda cmd: (
            cmd.add_argument(
                "--native-lib",
                help="Explicit native library path; highest precedence in resolution chain",
            ),
            cmd.add_argument(
                "--schema-path",
                default=str(CANONICAL_SCHEMA_RELATIVE),
                help=(
                    "Canonical schema JSON path used for V1 contract checks "
                    f"(default: {CANONICAL_SCHEMA_RELATIVE})"
                ),
            ),
            cmd.add_argument(
                "--strict",
                action=argparse.BooleanOptionalAction,
                default=True,
                help="Emit machine-readable JSON error envelopes on stderr when checks fail",
            ),
        ),
    )

    return parser


def main(argv: Sequence[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)

    if not hasattr(args, "handler"):
        parser.print_help()
        return 0

    return int(args.handler(args))
