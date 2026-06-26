from __future__ import annotations

import argparse
import sys
from typing import Sequence


DEFAULT_API_URL = "http://localhost:3000"


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
    return _not_implemented("doctor")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="banana",
        description="Banana CLI (Python scaffold)",
    )

    subparsers = parser.add_subparsers(dest="command")

    k3h4_parser = subparsers.add_parser("k3h4", help="K3H4 command group")
    k3h4_parser.set_defaults(handler=_k3h4_group_help, _parser=k3h4_parser)
    k3h4_subparsers = k3h4_parser.add_subparsers(dest="k3h4_command")

    def add_k3h4_subcommand(name: str, help_text: str, handler):
        cmd = k3h4_subparsers.add_parser(name, help=help_text)
        cmd.add_argument(
            "--api-url",
            default=DEFAULT_API_URL,
            help=f"API base URL override (default: {DEFAULT_API_URL})",
        )
        cmd.set_defaults(handler=handler)

    add_k3h4_subcommand("sample", "Generate deterministic sample dataset (stub)", _k3h4_sample)
    add_k3h4_subcommand("run", "Execute K3H4 pipeline (stub)", _k3h4_run)
    add_k3h4_subcommand("explain", "Explain K3H4 outputs (stub)", _k3h4_explain)
    add_k3h4_subcommand("export", "Export K3H4 artifacts (stub)", _k3h4_export)
    add_k3h4_subcommand("doctor", "Run read-only preflight diagnostics (stub)", _k3h4_doctor)

    return parser


def main(argv: Sequence[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)

    if not hasattr(args, "handler"):
        parser.print_help()
        return 0

    return int(args.handler(args))
