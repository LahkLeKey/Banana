#!/usr/bin/env python3
"""Validate Banana AI customization contracts under .github."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
GITHUB_DIR = ROOT / ".github"
PROMPTS_DIR = GITHUB_DIR / "prompts"
AGENTS_DIR = GITHUB_DIR / "agents"
INSTRUCTIONS_DIR = GITHUB_DIR / "instructions"
SKILLS_DIR = GITHUB_DIR / "skills"
WORKFLOW_SYNC_WIKI = ROOT / "scripts" / "workflow-sync-wiki.sh"
WORKFLOW_ORCHESTRATE_SDLC = ROOT / ".github" / "workflows" / "orchestrate-banana-sdlc.yml"
WORKFLOW_ORCHESTRATE_TRIAGED = ROOT / ".github" / "workflows" / "orchestrate-triaged-item-pr.yml"
WORKFLOW_ORCHESTRATE_FEEDBACK = ROOT / ".github" / "workflows" / "orchestrate-not-banana-feedback-loop.yml"
WORKFLOW_COPILOT_REVIEW_TRIAGE = ROOT / ".github" / "workflows" / "copilot-review-triage.yml"
SCRIPT_ORCHESTRATE_SDLC = ROOT / "scripts" / "workflow-orchestrate-sdlc.sh"

AGENT_CONTRACT_FRAGMENT = "Feedback Loop And Incremental Branch Contract"
PROMPT_WIKI_CONTRACT_HEADER = "## Wiki Updater Contract"
SKILL_NAME_RE = re.compile(r"^[a-z0-9-]{1,64}$")


def parse_frontmatter(path: Path) -> tuple[dict[str, str] | None, str]:
    text = path.read_text(encoding="utf-8")
    if not text.startswith("---\n"):
        return None, text

    end = text.find("\n---\n", 4)
    if end == -1:
        return None, text

    frontmatter = text[4:end]
    body = text[end + 5 :]

    data: dict[str, str] = {}
    for raw_line in frontmatter.splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue
        if ":" not in line:
            continue
        key, value = line.split(":", 1)
        data[key.strip()] = value.strip().strip('"\'')

    return data, body


def gather_contract_mappings() -> set[str]:
    """Expected prompt pages written by scripts/workflow-sync-wiki.sh default mapping."""
    prompt_targets: set[str] = set()

    for prompt_path in sorted(PROMPTS_DIR.glob("*.prompt.md")):
        prompt_name = prompt_path.name.removesuffix(".prompt.md")
        prompt_targets.add(f"Auto-Prompts/{prompt_name}.md")

    return prompt_targets


def main() -> int:
    issues: list[str] = []
    prompt_missing_wiki_contract: list[str] = []

    # Prompts
    for prompt_path in sorted(PROMPTS_DIR.glob("*.prompt.md")):
        frontmatter, body = parse_frontmatter(prompt_path)
        rel = prompt_path.relative_to(ROOT).as_posix()

        if frontmatter is None:
            issues.append(f"PROMPT missing/invalid frontmatter: {rel}")
            continue

        for required in ("name", "description", "argument-hint", "agent"):
            if not frontmatter.get(required):
                issues.append(f"PROMPT missing {required}: {rel}")

        if PROMPT_WIKI_CONTRACT_HEADER not in body:
            prompt_missing_wiki_contract.append(rel)

    # Agents
    for agent_path in sorted(AGENTS_DIR.glob("*.agent.md")):
        frontmatter, body = parse_frontmatter(agent_path)
        rel = agent_path.relative_to(ROOT).as_posix()

        if frontmatter is None:
            issues.append(f"AGENT missing/invalid frontmatter: {rel}")
            continue

        if not frontmatter.get("description"):
            issues.append(f"AGENT missing description: {rel}")

        if AGENT_CONTRACT_FRAGMENT not in body:
            issues.append(f"AGENT missing feedback-loop contract link: {rel}")

    # Instructions
    for instruction_path in sorted(INSTRUCTIONS_DIR.glob("*.instructions.md")):
        frontmatter, _ = parse_frontmatter(instruction_path)
        rel = instruction_path.relative_to(ROOT).as_posix()

        if frontmatter is None:
            issues.append(f"INSTRUCTION missing/invalid frontmatter: {rel}")
            continue

        if not frontmatter.get("description"):
            issues.append(f"INSTRUCTION missing description: {rel}")

    # Skills
    for skill_path in sorted(SKILLS_DIR.glob("*/SKILL.md")):
        frontmatter, _ = parse_frontmatter(skill_path)
        rel = skill_path.relative_to(ROOT).as_posix()

        if frontmatter is None:
            issues.append(f"SKILL missing/invalid frontmatter: {rel}")
            continue

        name = frontmatter.get("name", "")
        folder_name = skill_path.parent.name

        if not name:
            issues.append(f"SKILL missing name: {rel}")
        elif name != folder_name:
            issues.append(f"SKILL name mismatch ({name} != {folder_name}): {rel}")
        elif not SKILL_NAME_RE.match(name):
            issues.append(f"SKILL invalid name format: {rel}")

        if not frontmatter.get("description"):
            issues.append(f"SKILL missing description: {rel}")

    # Ensure prompt auto-sync mapping logic exists
    workflow_sync_text = WORKFLOW_SYNC_WIKI.read_text(encoding="utf-8")
    expected_targets = gather_contract_mappings()

    if "find .github/prompts -maxdepth 1 -type f -name \"*.prompt.md\"" not in workflow_sync_text:
        issues.append("WIKI_SYNC missing dynamic .github/prompts mapping block")

    if "Auto-Prompts/${prompt_name}.md" not in workflow_sync_text:
        issues.append("WIKI_SYNC missing Auto-Prompts target mapping")

    workflow_contract_targets = [
        WORKFLOW_ORCHESTRATE_TRIAGED,
        WORKFLOW_ORCHESTRATE_FEEDBACK,
    ]
    for workflow_path in workflow_contract_targets:
        workflow_text = workflow_path.read_text(encoding="utf-8")
        workflow_rel = workflow_path.relative_to(ROOT).as_posix()

        if "workflow-sync-wiki.sh" not in workflow_text:
            issues.append(f"WORKFLOW missing wiki sync step: {workflow_rel}")

        if "BANANA_WIKI_REMOTE_URL" not in workflow_text:
            issues.append(f"WORKFLOW missing BANANA_WIKI_REMOTE_URL wiring: {workflow_rel}")

        if workflow_path == WORKFLOW_ORCHESTRATE_FEEDBACK:
            if "github.event_name == 'schedule' && 'true'" not in workflow_text:
                issues.append(f"WORKFLOW missing schedule-forced wiki strict mode: {workflow_rel}")

    sdlc_workflow_text = WORKFLOW_ORCHESTRATE_SDLC.read_text(encoding="utf-8")
    sdlc_workflow_rel = WORKFLOW_ORCHESTRATE_SDLC.relative_to(ROOT).as_posix()
    if "workflow-orchestrate-sdlc.sh" not in sdlc_workflow_text:
        issues.append(f"WORKFLOW missing SDLC orchestration step: {sdlc_workflow_rel}")

    if "BANANA_WIKI_REMOTE_URL" not in sdlc_workflow_text:
        issues.append(f"WORKFLOW missing BANANA_WIKI_REMOTE_URL wiring: {sdlc_workflow_rel}")

    if "github.event_name == 'schedule' && 'true'" not in sdlc_workflow_text:
        issues.append(f"WORKFLOW missing schedule-forced wiki strict mode: {sdlc_workflow_rel}")

    copilot_triage_rel = WORKFLOW_COPILOT_REVIEW_TRIAGE.relative_to(ROOT).as_posix()
    if not WORKFLOW_COPILOT_REVIEW_TRIAGE.exists():
        issues.append(f"WORKFLOW missing Copilot review triage workflow: {copilot_triage_rel}")
    else:
        copilot_triage_text = WORKFLOW_COPILOT_REVIEW_TRIAGE.read_text(encoding="utf-8")
        copilot_required_fragments = {
            "pull_request_review_thread": "WORKFLOW missing pull_request_review_thread trigger",
            "copilot-pull-request-reviewer": "WORKFLOW missing Copilot reviewer login detection",
            "copilot-triage-pending": "WORKFLOW missing copilot-triage-pending label contract",
            "copilot-triage-ready": "WORKFLOW missing copilot-triage-ready label contract",
            "copilot-auto-approve": "WORKFLOW missing copilot-auto-approve opt-in contract",
            'event: "APPROVE"': "WORKFLOW missing automated approval event",
            "core.setFailed": "WORKFLOW missing unresolved-triage failure path",
        }

        for fragment, message in copilot_required_fragments.items():
            if fragment not in copilot_triage_text:
                issues.append(f"{message}: {copilot_triage_rel}")

    sdlc_script_text = SCRIPT_ORCHESTRATE_SDLC.read_text(encoding="utf-8")
    if "WIKI_REMOTE_URL=" not in sdlc_script_text:
        issues.append("SDLC script missing WIKI_REMOTE_URL input wiring")

    if 'export BANANA_WIKI_REMOTE_URL="$WIKI_REMOTE_URL"' not in sdlc_script_text:
        issues.append("SDLC script missing BANANA_WIKI_REMOTE_URL export")

    payload: dict[str, Any] = {
        "issues": issues,
        "prompt_missing_wiki_contract": prompt_missing_wiki_contract,
        "prompt_count": len(list(PROMPTS_DIR.glob("*.prompt.md"))),
        "expected_prompt_wiki_targets": sorted(expected_targets),
    }

    print(json.dumps(payload, indent=2))

    if issues or prompt_missing_wiki_contract:
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
