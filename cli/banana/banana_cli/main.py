from __future__ import annotations

import argparse
import ctypes
import json
import os
from pathlib import Path
import platform
import sys
from dataclasses import dataclass
from typing import Sequence


DEFAULT_API_URL = "http://localhost:3000"
EXPECTED_ABI_VERSION = 3
EXPECTED_PING_STATUS = 0
REQUIRED_PYTHON = (3, 10)
CANONICAL_SCHEMA_RELATIVE = Path("src/typescript/api/coverage-denominator.json")
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
    return _not_implemented("sample")


def _k3h4_run(args: argparse.Namespace) -> int:
    return _not_implemented("run")


def _k3h4_explain(args: argparse.Namespace) -> int:
    return _not_implemented("explain")


def _k3h4_export(args: argparse.Namespace) -> int:
    return _not_implemented("export")


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

    add_k3h4_subcommand("sample", "Generate deterministic sample dataset (stub)", _k3h4_sample)
    add_k3h4_subcommand("run", "Execute K3H4 pipeline (stub)", _k3h4_run)
    add_k3h4_subcommand("explain", "Explain K3H4 outputs (stub)", _k3h4_explain)
    add_k3h4_subcommand("export", "Export K3H4 artifacts (stub)", _k3h4_export)
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
