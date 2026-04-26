# Prompts Inventory (v1 snapshot)

Source: `.github/prompts/*.prompt.md`. 25 prompts. The validator
(`scripts/validate-ai-contracts.py`) enforces 1:1 linkage with
`Auto-Prompts/<name>.md` wiki pages.

| Prompt | Wiki contract | Primary consumer |
| --- | --- | --- |
| `debug-banana-ci.prompt.md` | `Auto-Prompts/debug-banana-ci.md` | banana-sdlc / infrastructure-agent |
| `debug-banana-mobile-runtime.prompt.md` | `Auto-Prompts/debug-banana-mobile-runtime.md` | mobile-runtime-agent |
| `debug-banana-test-failure.prompt.md` | `Auto-Prompts/debug-banana-test-failure.md` | test-triage-agent |
| `focus-on-open-pull-requests.prompt.md` | `Auto-Prompts/focus-on-open-pull-requests.md` | banana-sdlc |
| `implement-banana-api-interop-change.prompt.md` | `Auto-Prompts/implement-banana-api-interop-change.md` | api-interop-agent |
| `implement-banana-api-pipeline-change.prompt.md` | `Auto-Prompts/implement-banana-api-pipeline-change.md` | api-pipeline-agent |
| `implement-banana-change.prompt.md` | `Auto-Prompts/implement-banana-change.md` | banana-sdlc (top-level) |
| `implement-banana-classifier-slice.prompt.md` | `Auto-Prompts/implement-banana-classifier-slice.md` | banana-classifier-agent |
| `implement-banana-compose-change.prompt.md` | `Auto-Prompts/implement-banana-compose-change.md` | compose-runtime-agent |
| `implement-banana-mobile-runtime-change.prompt.md` | `Auto-Prompts/implement-banana-mobile-runtime-change.md` | mobile-runtime-agent |
| `implement-banana-native-core-change.prompt.md` | `Auto-Prompts/implement-banana-native-core-change.md` | native-core-agent |
| `implement-banana-native-wrapper-change.prompt.md` | `Auto-Prompts/implement-banana-native-wrapper-change.md` | native-wrapper-agent |
| `implement-banana-react-ui-change.prompt.md` | `Auto-Prompts/implement-banana-react-ui-change.md` | react-ui-agent |
| `implement-banana-workflow-change.prompt.md` | `Auto-Prompts/implement-banana-workflow-change.md` | workflow-agent |
| `iterate-the-backlog.prompt.md` | `Auto-Prompts/iterate-the-backlog.md` | value-risk-prioritization-agent |
| `plan-banana-change.prompt.md` | `Auto-Prompts/plan-banana-change.md` | banana-planner |
| `review-banana-change.prompt.md` | `Auto-Prompts/review-banana-change.md` | banana-reviewer |
| `specify.git.commit.prompt.md` | `Auto-Prompts/specify.git.commit.md` | specify.git.commit |
| `specify.git.feature.prompt.md` | `Auto-Prompts/specify.git.feature.md` | specify.git.feature |
| `specify.git.initialize.prompt.md` | `Auto-Prompts/specify.git.initialize.md` | specify.git.initialize |
| `specify.git.remote.prompt.md` | `Auto-Prompts/specify.git.remote.md` | specify.git.remote |
| `specify.git.validate.prompt.md` | `Auto-Prompts/specify.git.validate.md` | specify.git.validate |
| `sprint.prompt.md` | `Auto-Prompts/sprint.md` | banana-sdlc |
| `triage.prompt.md` | `Auto-Prompts/triage.md` | banana-sdlc |
| `validate-banana-change.prompt.md` | `Auto-Prompts/validate-banana-change.md` | integration-agent |

Verbatim prompt content is preserved at
`../github-ai-surface/prompts/`.
