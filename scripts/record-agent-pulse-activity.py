#!/usr/bin/env python3
"""Record one management-facing autonomous activity snapshot for an agent."""

from __future__ import annotations

import argparse
import datetime as dt
import json
import os
import pathlib
import re


def slugify(value: str) -> str:
    normalized = value.strip().lower()
    if normalized.startswith("agent:"):
        normalized = normalized.split(":", 1)[1]
    normalized = re.sub(r"[^a-z0-9._-]+", "-", normalized).strip("-._")
    return normalized or "workflow-agent"


def utc_now_iso() -> str:
    return dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def to_display_name(agent_slug: str) -> str:
    return " ".join(part.capitalize() for part in agent_slug.split("-"))


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Create an agent pulse snapshot markdown file for one SDLC increment."
    )
    parser.add_argument("--agent", required=True, help="Agent label or slug, for example agent:workflow-agent")
    parser.add_argument("--increment-id", required=True, help="Increment identifier")
    parser.add_argument("--intent", required=True, help="Human-readable intent summary")
    parser.add_argument("--scope", default="autonomous-self-training", help="Orchestration scope")
    parser.add_argument(
        "--output-root",
        default="docs/automation/agent-pulse",
        help="Root folder for management pulse documentation",
    )
    parser.add_argument(
        "--identity-registry",
        default="docs/automation/agent-pulse/agent-identities.json",
        help="Identity registry JSON file used to resolve icon/name/follow metadata",
    )
    return parser.parse_args()


def build_run_url() -> str:
    server = os.environ.get("GITHUB_SERVER_URL", "").strip()
    repository = os.environ.get("GITHUB_REPOSITORY", "").strip()
    run_id = os.environ.get("GITHUB_RUN_ID", "local-run").strip()
    if server and repository and run_id != "local-run":
        return f"{server}/{repository}/actions/runs/{run_id}"
    return "local-run"


def load_agent_identity(registry_path: pathlib.Path, agent_slug: str) -> dict[str, str]:
    fallback = {
        "display_name": to_display_name(agent_slug),
        "icon": "",
        "follow_url": f"docs/automation/agent-pulse/agents/{agent_slug}/",
        "followable": "false",
    }

    if not registry_path.exists():
        return fallback

    try:
        payload = json.loads(registry_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return fallback

    if not isinstance(payload, dict):
        return fallback

    agents = payload.get("agents", [])
    if not isinstance(agents, list):
        return fallback

    for entry in agents:
        if not isinstance(entry, dict):
            continue
        if slugify(str(entry.get("slug", ""))) != agent_slug:
            continue

        resolved = {
            "display_name": str(entry.get("display_name", "")).strip() or fallback["display_name"],
            "icon": str(entry.get("icon", "")).strip(),
            "follow_url": str(entry.get("follow_url", "")).strip() or fallback["follow_url"],
            "followable": str(entry.get("followable", "false")).strip().lower(),
        }
        if resolved["followable"] not in {"true", "false"}:
            resolved["followable"] = "false"
        return resolved

    return fallback


def ensure_agent_readme(path: pathlib.Path, agent_slug: str, identity: dict[str, str]) -> None:
    if path.exists():
        return

    path.write_text(
        "\n".join(
            [
                f"# Agent Pulse: {agent_slug}",
                "",
                "This folder stores autonomous run snapshots for this agent.",
                "",
                "## Identity",
                "",
                f"- Display Name: {identity.get('display_name', to_display_name(agent_slug))}",
                f"- Icon: {identity.get('icon', '') or 'n/a'}",
                f"- Follow URL: {identity.get('follow_url', f'docs/automation/agent-pulse/agents/{agent_slug}/')}",
                f"- Followable on GitHub: {identity.get('followable', 'false')}",
                "",
                "## Snapshot Convention",
                "",
                "- Each autonomous increment writes one markdown snapshot under `runs/`.",
                "- File names include workflow run ID, attempt, and increment ID.",
            ]
        )
        + "\n",
        encoding="utf-8",
    )


def main() -> int:
    args = parse_args()

    agent_slug = slugify(args.agent)
    increment_slug = slugify(args.increment_id)

    run_id = os.environ.get("GITHUB_RUN_ID", "local-run").strip() or "local-run"
    run_attempt = os.environ.get("GITHUB_RUN_ATTEMPT", "1").strip() or "1"
    workflow_name = os.environ.get("GITHUB_WORKFLOW", "local-workflow").strip() or "local-workflow"
    event_name = os.environ.get("GITHUB_EVENT_NAME", "local").strip() or "local"
    actor = os.environ.get("GITHUB_ACTOR", "local-user").strip() or "local-user"
    git_ref = os.environ.get("GITHUB_REF_NAME", "local-ref").strip() or "local-ref"
    sha = os.environ.get("GITHUB_SHA", "local-sha").strip() or "local-sha"
    run_url = build_run_url()
    recorded_at = utc_now_iso()

    output_root = pathlib.Path(args.output_root)
    identity_registry_path = pathlib.Path(args.identity_registry)
    agent_root = output_root / "agents" / agent_slug
    run_dir = agent_root / "runs"
    run_dir.mkdir(parents=True, exist_ok=True)

    identity = load_agent_identity(identity_registry_path, agent_slug)
    ensure_agent_readme(agent_root / "README.md", agent_slug, identity)

    snapshot_path = run_dir / f"run-{run_id}-attempt-{run_attempt}-{increment_slug}.md"
    snapshot_path.write_text(
        "\n".join(
            [
                "# Agent Run Snapshot",
                "",
                f"- Agent: {agent_slug}",
                f"- Agent Display Name: {identity.get('display_name', to_display_name(agent_slug))}",
                f"- Agent Icon: {identity.get('icon', '') or 'n/a'}",
                f"- Agent Follow URL: {identity.get('follow_url', f'docs/automation/agent-pulse/agents/{agent_slug}/')}",
                f"- Agent Followable on GitHub: {identity.get('followable', 'false')}",
                f"- Increment: {args.increment_id}",
                f"- Intent: {args.intent}",
                f"- Scope: {args.scope}",
                f"- Recorded at (UTC): {recorded_at}",
                f"- Workflow: {workflow_name}",
                f"- Event: {event_name}",
                f"- Run ID: {run_id}",
                f"- Run Attempt: {run_attempt}",
                f"- Run URL: {run_url}",
                f"- Actor: {actor}",
                f"- Ref: {git_ref}",
                f"- SHA: {sha}",
            ]
        )
        + "\n",
        encoding="utf-8",
    )

    print(str(snapshot_path))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
