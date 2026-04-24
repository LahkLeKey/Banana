#!/usr/bin/env python3
"""Validate Banana AI customization contracts under .github."""

from __future__ import annotations

import json
import re
import subprocess
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
WORKFLOW_ORCHESTRATE_TRIAGE_IDEA = ROOT / ".github" / "workflows" / "orchestrate-triage-idea-cloud.yml"
WORKFLOW_ORCHESTRATE_AUTONOMOUS = ROOT / ".github" / "workflows" / "orchestrate-autonomous-self-training-cycle.yml"
WORKFLOW_COPILOT_REVIEW_TRIAGE = ROOT / ".github" / "workflows" / "copilot-review-triage.yml"
WORKFLOW_REQUIRE_HUMAN_APPROVAL = ROOT / ".github" / "workflows" / "require-human-approval.yml"
SCRIPT_ORCHESTRATE_TRIAGED = ROOT / "scripts" / "workflow-orchestrate-triaged-item-pr.sh"
SCRIPT_ORCHESTRATE_SDLC = ROOT / "scripts" / "workflow-orchestrate-sdlc.sh"
SCRIPT_ORCHESTRATE_FEEDBACK = ROOT / "scripts" / "orchestrate-not-banana-feedback-loop.sh"
SCRIPT_ORCHESTRATE_TRIAGE_IDEA = ROOT / "scripts" / "workflow-triage-idea-cloud.sh"
SCRIPT_PERSIST_REGISTRY_HISTORY = ROOT / "scripts" / "workflow-persist-registry-history-pr.sh"
SCRIPT_ENSURE_SPECKIT = ROOT / "scripts" / "workflow-ensure-speckit.sh"
CANONICAL_WIKI_REMOTE_URL = "https://github.com/LahkLeKey/Banana.wiki.git"

AGENT_CONTRACT_FRAGMENT = "Feedback Loop And Incremental Branch Contract"
PROMPT_WIKI_CONTRACT_HEADER = "## Wiki Updater Contract"
SPECKIT_EXTENSION_PROMPT_PREFIX = "speckit.git."
SPECKIT_EXTENSION_AGENT_PREFIX = "speckit.git."
SKILL_NAME_RE = re.compile(r"^[a-z0-9-]{1,64}$")
ITERATE_BACKLOG_PROMPT = PROMPTS_DIR / "iterate-the-backlog.prompt.md"
ITERATE_BACKLOG_PROMPT_REQUIRED_FRAGMENTS = {
    "orchestrate-triage-idea-cloud.yml": "PROMPT missing cloud triage workflow contract",
    "workflow-triage-idea-cloud.sh": "PROMPT missing cloud triage orchestration script contract",
    "orchestrate-banana-sdlc.yml": "PROMPT missing SDLC workflow contract",
    "workflow-orchestrate-sdlc.sh": "PROMPT missing SDLC orchestration script contract",
    "orchestrate-not-banana-feedback-loop.yml": "PROMPT missing feedback-loop workflow contract",
    "copilot-review-triage.yml": "PROMPT missing copilot triage required-check contract",
    "require-human-approval.yml": "PROMPT missing human-approval required-check contract",
    "Stop when no eligible backlog items remain.": "PROMPT missing deterministic no-work stop condition",
    "Return concrete outputs for each iteration": "PROMPT missing iteration output contract",
}
FOCUS_OPEN_PRS_PROMPT = PROMPTS_DIR / "focus-on-open-pull-requests.prompt.md"
FOCUS_OPEN_PRS_PROMPT_REQUIRED_FRAGMENTS = {
    "open pull requests": "PROMPT missing open pull request focus scope",
    "copilot-review-triage.yml": "PROMPT missing copilot review triage workflow contract",
    "require-human-approval.yml": "PROMPT missing human approval workflow contract",
    "Do not force-merge or bypass branch protections unless explicitly requested by a human.": "PROMPT missing branch-protection safety contract",
    "Do not open duplicate replacement PRs": "PROMPT missing duplicate PR safeguard contract",
    "Return concrete outputs for each PR handled": "PROMPT missing per-PR output contract",
}
TERM_GUARD_TOKEN = "vi" + "be"
TERM_GUARD_TITLE_TOKEN = TERM_GUARD_TOKEN.capitalize()
TERM_GUARD_PATTERNS: tuple[tuple[re.Pattern[str], str], ...] = (
    (
        re.compile(rf"\bcopilot-bypass-{TERM_GUARD_TOKEN}-coded\b", re.IGNORECASE),
        "legacy-label-token",
    ),
    (re.compile(rf"\b{TERM_GUARD_TOKEN}[- ]coding\b", re.IGNORECASE), "legacy-coding-phrase"),
    (re.compile(rf"\b{TERM_GUARD_TOKEN}[- ]coded\b", re.IGNORECASE), "legacy-coded-phrase"),
    (re.compile(rf"\b{TERM_GUARD_TOKEN}bypass\b", re.IGNORECASE), "legacy-bypass-token"),
    (
        re.compile(rf"##\s+{TERM_GUARD_TITLE_TOKEN}\s+Code\s+Quickstart", re.IGNORECASE),
        "legacy-quickstart-heading",
    ),
    (
        re.compile(rf"##\s+{TERM_GUARD_TITLE_TOKEN}\s+Coding\s+Guardrails", re.IGNORECASE),
        "legacy-guardrails-heading",
    ),
)
TERM_GUARD_EXCLUDED_DIRECTORIES = {
    ".git",
    ".idea",
    ".venv",
    ".vs",
    "build",
    "node_modules",
    "__pycache__",
}
TERM_GUARD_EXCLUDED_SUFFIXES = {
    ".7z",
    ".a",
    ".bin",
    ".bmp",
    ".class",
    ".dll",
    ".dylib",
    ".eot",
    ".exe",
    ".gif",
    ".gz",
    ".ico",
    ".jar",
    ".jpeg",
    ".jpg",
    ".lib",
    ".o",
    ".obj",
    ".pdf",
    ".png",
    ".so",
    ".tar",
    ".tgz",
    ".ttf",
    ".webp",
    ".woff",
    ".woff2",
    ".zip",
}


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


def iter_fallback_guard_files() -> list[Path]:
    candidates: list[Path] = []
    for path in ROOT.rglob("*"):
        if not path.is_file():
            continue

        rel_path = path.relative_to(ROOT)
        if any(part in TERM_GUARD_EXCLUDED_DIRECTORIES for part in rel_path.parts):
            continue

        if path.suffix.lower() in TERM_GUARD_EXCLUDED_SUFFIXES:
            continue

        candidates.append(path)

    return sorted(candidates)


def iter_guard_target_files() -> list[Path]:
    try:
        result = subprocess.run(
            ["git", "-C", str(ROOT), "ls-files", "-z"],
            check=True,
            capture_output=True,
            text=False,
        )
    except (FileNotFoundError, subprocess.CalledProcessError):
        return iter_fallback_guard_files()

    candidates: list[Path] = []
    for raw_rel in result.stdout.split(b"\x00"):
        if not raw_rel:
            continue

        rel = raw_rel.decode("utf-8", errors="ignore")
        rel_path = Path(rel)

        if any(part in TERM_GUARD_EXCLUDED_DIRECTORIES for part in rel_path.parts):
            continue

        abs_path = ROOT / rel_path
        if not abs_path.is_file():
            continue

        if abs_path.suffix.lower() in TERM_GUARD_EXCLUDED_SUFFIXES:
            continue

        candidates.append(abs_path)

    return sorted(candidates)


def validate_no_legacy_terms(issues: list[str]) -> None:
    for file_path in iter_guard_target_files():
        rel = file_path.relative_to(ROOT).as_posix()

        try:
            text = file_path.read_text(encoding="utf-8")
        except (OSError, UnicodeDecodeError):
            continue

        for line_number, line in enumerate(text.splitlines(), start=1):
            for pattern, label in TERM_GUARD_PATTERNS:
                if pattern.search(line):
                    snippet = line.strip()
                    if len(snippet) > 140:
                        snippet = snippet[:137] + "..."
                    issues.append(
                        f"TERM_GUARD prohibited phrase '{label}' found: {rel}:{line_number} ({snippet})"
                    )
                    break


def main() -> int:
    issues: list[str] = []
    prompt_missing_wiki_contract: list[str] = []

    # Prompts
    for prompt_path in sorted(PROMPTS_DIR.glob("*.prompt.md")):
        frontmatter, body = parse_frontmatter(prompt_path)
        rel = prompt_path.relative_to(ROOT).as_posix()
        prompt_name = prompt_path.name.removesuffix(".prompt.md")
        is_speckit_extension_prompt = prompt_name.startswith(SPECKIT_EXTENSION_PROMPT_PREFIX)

        if frontmatter is None:
            issues.append(f"PROMPT missing/invalid frontmatter: {rel}")
            continue

        required_frontmatter = (
            ("agent",)
            if is_speckit_extension_prompt
            else ("name", "description", "argument-hint", "agent")
        )

        for required in required_frontmatter:
            if not frontmatter.get(required):
                issues.append(f"PROMPT missing {required}: {rel}")

        if not is_speckit_extension_prompt and PROMPT_WIKI_CONTRACT_HEADER not in body:
            prompt_missing_wiki_contract.append(rel)

    iterate_backlog_rel = ITERATE_BACKLOG_PROMPT.relative_to(ROOT).as_posix()
    if not ITERATE_BACKLOG_PROMPT.exists():
        issues.append(f"PROMPT missing backlog-iteration command contract: {iterate_backlog_rel}")
    else:
        iterate_frontmatter, iterate_body = parse_frontmatter(ITERATE_BACKLOG_PROMPT)
        if iterate_frontmatter is None:
            issues.append(f"PROMPT missing/invalid frontmatter: {iterate_backlog_rel}")
        else:
            for fragment, message in ITERATE_BACKLOG_PROMPT_REQUIRED_FRAGMENTS.items():
                if fragment not in iterate_body:
                    issues.append(f"{message}: {iterate_backlog_rel}")

    focus_open_prs_rel = FOCUS_OPEN_PRS_PROMPT.relative_to(ROOT).as_posix()
    if not FOCUS_OPEN_PRS_PROMPT.exists():
        issues.append(f"PROMPT missing open-pr-focus command contract: {focus_open_prs_rel}")
    else:
        focus_frontmatter, focus_body = parse_frontmatter(FOCUS_OPEN_PRS_PROMPT)
        if focus_frontmatter is None:
            issues.append(f"PROMPT missing/invalid frontmatter: {focus_open_prs_rel}")
        else:
            for fragment, message in FOCUS_OPEN_PRS_PROMPT_REQUIRED_FRAGMENTS.items():
                if fragment not in focus_body:
                    issues.append(f"{message}: {focus_open_prs_rel}")

    # Agents
    for agent_path in sorted(AGENTS_DIR.glob("*.agent.md")):
        frontmatter, body = parse_frontmatter(agent_path)
        rel = agent_path.relative_to(ROOT).as_posix()
        is_speckit_extension_agent = agent_path.name.startswith(SPECKIT_EXTENSION_AGENT_PREFIX)

        if frontmatter is None:
            issues.append(f"AGENT missing/invalid frontmatter: {rel}")
            continue

        if not frontmatter.get("description"):
            issues.append(f"AGENT missing description: {rel}")

        if not is_speckit_extension_agent and AGENT_CONTRACT_FRAGMENT not in body:
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

    if "find .github/workflows -maxdepth 1 -type f -name \"*.yml\"" not in workflow_sync_text:
        issues.append("WIKI_SYNC missing dynamic .github/workflows mapping block")

    if "Auto-GitHub-Workflows/${workflow_file}.md" not in workflow_sync_text:
        issues.append("WIKI_SYNC missing Auto-GitHub-Workflows target mapping")

    if "find .github/instructions -maxdepth 1 -type f -name \"*.md\"" not in workflow_sync_text:
        issues.append("WIKI_SYNC missing dynamic .github/instructions mapping block")

    if "Auto-GitHub-Instructions/${instruction_file}" not in workflow_sync_text:
        issues.append("WIKI_SYNC missing Auto-GitHub-Instructions target mapping")

    if "CANONICAL_WIKI_REMOTE_URL" not in workflow_sync_text:
        issues.append("WIKI_SYNC missing canonical wiki remote contract")

    if "BANANA_ENFORCE_CANONICAL_WIKI_REMOTE" not in workflow_sync_text:
        issues.append("WIKI_SYNC missing canonical wiki remote enforcement toggle")

    if not SCRIPT_ENSURE_SPECKIT.exists():
        issues.append("SCRIPT missing Spec Kit preflight helper: scripts/workflow-ensure-speckit.sh")
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

        if CANONICAL_WIKI_REMOTE_URL not in workflow_text:
            issues.append(f"WORKFLOW missing canonical wiki remote default: {workflow_rel}")

        if "speckit-driven" not in workflow_text:
            issues.append(f"WORKFLOW missing spec-kit provenance label default: {workflow_rel}")
        if workflow_path == WORKFLOW_ORCHESTRATE_FEEDBACK:
            if "github.event_name == 'schedule' && 'true'" not in workflow_text:
                issues.append(f"WORKFLOW missing schedule-forced wiki strict mode: {workflow_rel}")

    triage_idea_rel = WORKFLOW_ORCHESTRATE_TRIAGE_IDEA.relative_to(ROOT).as_posix()
    if not WORKFLOW_ORCHESTRATE_TRIAGE_IDEA.exists():
        issues.append(f"WORKFLOW missing cloud triage idea orchestration workflow: {triage_idea_rel}")
    else:
        triage_idea_text = WORKFLOW_ORCHESTRATE_TRIAGE_IDEA.read_text(encoding="utf-8")
        triage_idea_required_fragments = {
            "workflow_dispatch": "WORKFLOW missing workflow_dispatch trigger for cloud triage idea orchestration",
            "issues": "WORKFLOW missing issue-event trigger for cloud triage idea orchestration",
            "triage-idea": "WORKFLOW missing triage-idea issue label contract",
            "copilot-suggestion": "WORKFLOW missing copilot-suggestion issue label contract",
            "human-triage": "WORKFLOW missing human-triage issue label contract",
            "ai-generated": "WORKFLOW missing AI-origin triage label contract",
            "join(github.event.issue.labels.*.name, ',')": "WORKFLOW missing issue-event label preservation contract",
            "clear_backlog": "WORKFLOW missing backlog cleanup toggle",
            "backlog_issue_labels": "WORKFLOW missing backlog issue-label cleanup contract",
            "backlog_pr_labels": "WORKFLOW missing backlog PR-label cleanup contract",
            "backlog_pr_branch_prefixes": "WORKFLOW missing backlog PR-branch cleanup contract",
            "epic_decomposition": "WORKFLOW missing epic decomposition toggle",
            "epic_auto_dispatch_first_task": "WORKFLOW missing epic first-task auto-dispatch toggle",
            "BANANA_TRIAGE_ENABLE_EPIC_DECOMPOSITION": "WORKFLOW missing epic decomposition env wiring",
            "BANANA_TRIAGE_EPIC_AUTO_DISPATCH_FIRST_TASK": "WORKFLOW missing epic first-task dispatch env wiring",
            "workflow-triage-idea-cloud.sh": "WORKFLOW missing triage idea orchestration script execution",
            "workflow-sync-wiki.sh": "WORKFLOW missing wiki sync step",
            "BANANA_WIKI_REMOTE_URL": "WORKFLOW missing BANANA_WIKI_REMOTE_URL wiring",
            CANONICAL_WIKI_REMOTE_URL: "WORKFLOW missing canonical wiki remote default",
        }

        for fragment, message in triage_idea_required_fragments.items():
            if fragment not in triage_idea_text:
                issues.append(f"{message}: {triage_idea_rel}")

    sdlc_workflow_text = WORKFLOW_ORCHESTRATE_SDLC.read_text(encoding="utf-8")
    sdlc_workflow_rel = WORKFLOW_ORCHESTRATE_SDLC.relative_to(ROOT).as_posix()
    if "workflow-orchestrate-sdlc.sh" not in sdlc_workflow_text:
        issues.append(f"WORKFLOW missing SDLC orchestration step: {sdlc_workflow_rel}")

    if "BANANA_WIKI_REMOTE_URL" not in sdlc_workflow_text:
        issues.append(f"WORKFLOW missing BANANA_WIKI_REMOTE_URL wiring: {sdlc_workflow_rel}")

    if CANONICAL_WIKI_REMOTE_URL not in sdlc_workflow_text:
        issues.append(f"WORKFLOW missing canonical wiki remote default: {sdlc_workflow_rel}")

    if "speckit-driven" not in sdlc_workflow_text:
        issues.append(f"WORKFLOW missing spec-kit provenance label default: {sdlc_workflow_rel}")

    if "github.event_name == 'schedule' && 'true'" not in sdlc_workflow_text:
        issues.append(f"WORKFLOW missing schedule-forced wiki strict mode: {sdlc_workflow_rel}")

    if "allow_actions_actor" not in sdlc_workflow_text:
        issues.append(f"WORKFLOW missing allow_actions_actor dispatch contract: {sdlc_workflow_rel}")

    autonomous_rel = WORKFLOW_ORCHESTRATE_AUTONOMOUS.relative_to(ROOT).as_posix()
    if not WORKFLOW_ORCHESTRATE_AUTONOMOUS.exists():
        issues.append(f"WORKFLOW missing autonomous self-training cycle workflow: {autonomous_rel}")
    else:
        autonomous_text = WORKFLOW_ORCHESTRATE_AUTONOMOUS.read_text(encoding="utf-8")
        autonomous_required_fragments = {
            "pull_request_target": "WORKFLOW missing autonomous continuation trigger",
            "copilot-autonomous-cycle": "WORKFLOW missing copilot-autonomous-cycle label contract",
            "speckit-driven": "WORKFLOW missing spec-kit provenance label contract",
            "workflow-orchestrate-sdlc.sh": "WORKFLOW missing SDLC orchestrator execution",
            "training-profile ci --session-mode single --max-sessions 1": "WORKFLOW missing bounded minimal-resource training command",
            CANONICAL_WIKI_REMOTE_URL: "WORKFLOW missing canonical wiki remote default",
        }

        for fragment, message in autonomous_required_fragments.items():
            if fragment not in autonomous_text:
                issues.append(f"{message}: {autonomous_rel}")

    copilot_triage_rel = WORKFLOW_COPILOT_REVIEW_TRIAGE.relative_to(ROOT).as_posix()
    if not WORKFLOW_COPILOT_REVIEW_TRIAGE.exists():
        issues.append(f"WORKFLOW missing Copilot review triage workflow: {copilot_triage_rel}")
    else:
        copilot_triage_text = WORKFLOW_COPILOT_REVIEW_TRIAGE.read_text(encoding="utf-8")
        copilot_required_fragments = {
            "workflow_dispatch": "WORKFLOW missing workflow_dispatch trigger",
            "pull_number": "WORKFLOW missing workflow_dispatch pull_number input",
            "pull_request_review": "WORKFLOW missing pull_request_review trigger",
            "copilot-pull-request-reviewer": "WORKFLOW missing Copilot reviewer login detection",
            "copilot-triage-pending": "WORKFLOW missing copilot-triage-pending label contract",
            "copilot-triage-ready": "WORKFLOW missing copilot-triage-ready label contract",
            "copilot-auto-approve": "WORKFLOW missing copilot-auto-approve opt-in contract",
            "copilot-autonomous-cycle": "WORKFLOW missing autonomous cycle label contract",
            "speckit-driven": "WORKFLOW missing copilot bypass provenance label contract",
            "copilot-suggestion": "WORKFLOW missing copilot suggestion issue label contract",
            "syncSuggestionIssues": "WORKFLOW missing per-suggestion issue triage path",
            "createWorkflowDispatch": "WORKFLOW missing per-suggestion orchestration dispatch path",
            "requestReviewers": "WORKFLOW missing Copilot reviewer request path",
            'event: "APPROVE"': "WORKFLOW missing automated approval event",
            "core.setFailed": "WORKFLOW missing unresolved-triage failure path",
            "enablePullRequestAutoMerge": "WORKFLOW missing autonomous auto-merge path",
            "Awaiting Copilot review activity": "WORKFLOW missing no-activity Copilot wait state",
        }

        for fragment, message in copilot_required_fragments.items():
            if fragment not in copilot_triage_text:
                issues.append(f"{message}: {copilot_triage_rel}")

    require_human_rel = WORKFLOW_REQUIRE_HUMAN_APPROVAL.relative_to(ROOT).as_posix()
    require_human_text = WORKFLOW_REQUIRE_HUMAN_APPROVAL.read_text(encoding="utf-8")
    require_human_required_fragments = {
        "workflow_dispatch": "WORKFLOW missing workflow_dispatch trigger",
        "pull_number": "WORKFLOW missing workflow_dispatch pull_number input",
        "copilot-autonomous-cycle": "WORKFLOW missing autonomous-cycle bypass label support",
        "speckit-driven": "WORKFLOW missing spec-kit provenance label support",
        "copilot-triage-ready": "WORKFLOW missing copilot-triage-ready bypass guard",
        "github-actions[bot]": "WORKFLOW missing bot-approval continuity guard",
    }

    for fragment, message in require_human_required_fragments.items():
        if fragment not in require_human_text:
            issues.append(f"{message}: {require_human_rel}")
    triaged_script_text = SCRIPT_ORCHESTRATE_TRIAGED.read_text(encoding="utf-8")
    if "workflow-ensure-speckit.sh" not in triaged_script_text:
        issues.append("Triaged PR script missing Spec Kit preflight invocation")

    triaged_required_fragments = {
        "BANANA_AGENT_CONTRIBUTOR": "Triaged PR script missing automation contributor override input",
        "resolve_agent_contributor": "Triaged PR script missing automation contributor resolution",
        "contributor:${AGENT_CONTRIBUTOR_SLUG}": "Triaged PR script missing contributor label propagation",
        "git config user.name \"$AGENT_CONTRIBUTOR_NAME\"": "Triaged PR script missing contributor git author name wiring",
        "git config user.email \"$AGENT_CONTRIBUTOR_EMAIL\"": "Triaged PR script missing contributor git author email wiring",
        "Automation contributor:": "Triaged PR script missing contributor metadata in PR body",
    }

    for fragment, message in triaged_required_fragments.items():
        if fragment not in triaged_script_text:
            issues.append(message)

    feedback_script_text = SCRIPT_ORCHESTRATE_FEEDBACK.read_text(encoding="utf-8")
    if "workflow-ensure-speckit.sh" not in feedback_script_text:
        issues.append("Feedback-loop script missing Spec Kit preflight invocation")

    feedback_required_fragments = {
        "BANANA_AGENT_CONTRIBUTOR": "Feedback-loop script missing automation contributor override",
        "agent:banana-classifier-agent": "Feedback-loop script missing classifier agent label default",
    }

    for fragment, message in feedback_required_fragments.items():
        if fragment not in feedback_script_text:
            issues.append(message)

    sdlc_script_text = SCRIPT_ORCHESTRATE_SDLC.read_text(encoding="utf-8")
    if "WIKI_REMOTE_URL=" not in sdlc_script_text:
        issues.append("SDLC script missing WIKI_REMOTE_URL input wiring")

    if 'export BANANA_WIKI_REMOTE_URL="$WIKI_REMOTE_URL"' not in sdlc_script_text:
        issues.append("SDLC script missing BANANA_WIKI_REMOTE_URL export")

    if "speckit-driven" not in sdlc_script_text:
        issues.append("SDLC script missing spec-kit provenance label defaults")

    if "workflow-ensure-speckit.sh" not in sdlc_script_text:
        issues.append("SDLC script missing Spec Kit preflight invocation")

    persist_script_rel = SCRIPT_PERSIST_REGISTRY_HISTORY.relative_to(ROOT).as_posix()
    if not SCRIPT_PERSIST_REGISTRY_HISTORY.exists():
        issues.append(
            f"SCRIPT missing registry-history PR orchestration script: {persist_script_rel}"
        )
    else:
        persist_script_text = SCRIPT_PERSIST_REGISTRY_HISTORY.read_text(encoding="utf-8")
        persist_required_fragments = {
            "BANANA_AGENT_CONTRIBUTOR": "Registry-history PR script missing automation contributor override input",
            "contributor:${AGENT_CONTRIBUTOR_SLUG}": "Registry-history PR script missing contributor label propagation",
            "git config user.name \"$AGENT_CONTRIBUTOR_NAME\"": "Registry-history PR script missing contributor git author name wiring",
            "git config user.email \"$AGENT_CONTRIBUTOR_EMAIL\"": "Registry-history PR script missing contributor git author email wiring",
            "Automation contributor:": "Registry-history PR script missing contributor metadata in PR body",
        }

        for fragment, message in persist_required_fragments.items():
            if fragment not in persist_script_text:
                issues.append(f"{message}: {persist_script_rel}")

    triage_idea_script_rel = SCRIPT_ORCHESTRATE_TRIAGE_IDEA.relative_to(ROOT).as_posix()
    if not SCRIPT_ORCHESTRATE_TRIAGE_IDEA.exists():
        issues.append(f"SCRIPT missing cloud triage idea orchestration script: {triage_idea_script_rel}")
    else:
        triage_idea_script_text = SCRIPT_ORCHESTRATE_TRIAGE_IDEA.read_text(encoding="utf-8")
        triage_idea_script_required_fragments = {
            "triage-idea": "SCRIPT missing triage-idea label contract",
            "copilot-suggestion": "SCRIPT missing copilot-suggestion label contract",
            "speckit-driven": "SCRIPT missing spec-kit provenance label contract",
            "human-triage": "SCRIPT missing human triage label contract",
            "source-human-issue": "SCRIPT missing human-source issue label contract",
            "ai-generated": "SCRIPT missing AI-generated issue label contract",
            "source-ai-issue": "SCRIPT missing AI-source issue label contract",
            "banana-intake-source": "SCRIPT missing issue-source marker parsing contract",
            "banana-target-agent": "SCRIPT missing agent-target marker parsing contract",
            "agent:": "SCRIPT missing agent label propagation contract",
            "workflow-ensure-speckit.sh": "SCRIPT missing Spec Kit preflight invocation",
            "workflow-orchestrate-triaged-item-pr.sh": "SCRIPT missing triaged PR orchestration handoff",
            "docs/triage/intake": "SCRIPT missing triage intake artifact contract",
            "BANANA_TRIAGE_CLEAR_BACKLOG": "SCRIPT missing backlog cleanup toggle",
            "BANANA_TRIAGE_BACKLOG_ISSUE_LABELS": "SCRIPT missing backlog issue-label cleanup inputs",
            "BANANA_TRIAGE_BACKLOG_PR_LABELS": "SCRIPT missing backlog PR-label cleanup inputs",
            "BANANA_TRIAGE_BACKLOG_PR_BRANCH_PREFIXES": "SCRIPT missing backlog PR-branch cleanup inputs",
            "clear_triage_backlog_if_enabled": "SCRIPT missing backlog cleanup execution path",
            "BANANA_TRIAGE_DISPATCH_REQUIRED_CHECKS": "SCRIPT missing required-check dispatch toggle",
            "BANANA_TRIAGE_CHECK_WORKFLOWS": "SCRIPT missing required-check workflow list contract",
            "actions/workflows/${workflow_file}/dispatches": "SCRIPT missing required-check workflow dispatch path",
            "BANANA_TRIAGE_ENABLE_EPIC_DECOMPOSITION": "SCRIPT missing epic decomposition toggle",
            "BANANA_TRIAGE_EPIC_AUTO_DISPATCH_FIRST_TASK": "SCRIPT missing epic first-task auto-dispatch toggle",
            "scaffold_epic_breakdown_if_requested": "SCRIPT missing epic decomposition execution path",
            "dispatch_triage_issue_workflow": "SCRIPT missing first-task workflow dispatch helper",
            "epic-decomposed": "SCRIPT missing epic decomposition label contract",
            "user-story": "SCRIPT missing user-story label contract",
            "task": "SCRIPT missing task label contract",
        }

        for fragment, message in triage_idea_script_required_fragments.items():
            if fragment not in triage_idea_script_text:
                issues.append(f"{message}: {triage_idea_script_rel}")

    validate_no_legacy_terms(issues)

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
