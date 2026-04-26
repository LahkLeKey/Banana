# Legacy v1 Inventories

These files capture concrete v1 facts so the v1 source tree can be deleted
without losing data needed to regenerate v2 from `.specify/`. They are
**snapshots**, not specs. Specs live in `.specify/specs/006..014/`.

## Files

| File | What it captures |
| --- | --- |
| [native-abi-symbols.md](native-abi-symbols.md) | All 21 BANANA_API exports + status codes |
| [http-routes.md](http-routes.md) | ASP.NET controllers + route attributes |
| [env-vars.md](env-vars.md) | Runtime + automation env contract surface |
| [scripts-inventory.md](scripts-inventory.md) | All shell/python scripts with role |
| [workflows-inventory.md](workflows-inventory.md) | All GitHub Actions workflows |
| [dockerfiles-inventory.md](dockerfiles-inventory.md) | Dockerfiles + compose services |
| [prompts-inventory.md](prompts-inventory.md) | 25 prompts + wiki linkage |
| [agents-inventory.md](agents-inventory.md) | 28 agents + helper relationships |
| [skills-inventory.md](skills-inventory.md) | 16 skills + scope |
| [instructions-inventory.md](instructions-inventory.md) | 6 `applyTo` instruction files |
| [prisma-schema-snapshot.md](prisma-schema-snapshot.md) | Prisma 7 models |

The verbatim source content for `.github/{prompts,agents,skills,instructions}/`
and `.github/copilot-instructions.md` is preserved under
`../github-ai-surface/`.
