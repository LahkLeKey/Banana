# Feature Specification: Vercel Build Config (121)

**Feature Branch**: `feature/121-vercel-build-config`
**Created**: 2026-05-02
**Status**: Draft (pending spike 116 go verdict)
**Input**: Scaffolded from spike `116-vercel-react-deploy-spike`

## In Scope

- Harden `vercel.json` based on spike 116 findings: confirmed `buildCommand`, `outputDirectory`, `rewrites`, and security `headers`.
- Add `VITE_BANANA_API_BASE_URL` as a required environment variable comment in `vercel.json` or a companion `.env.example`.
- Confirm `bun run build` exits 0 in a clean Vercel-like environment (no pre-installed `node_modules`).

## Out of Scope

- Domain DNS changes (spec 123).
- Environment variable values (spec 124).
- API hosting (spec 125+).

## Success Criteria

```bash
python3 -c "import json; json.load(open('vercel.json')); print('OK')"
cd src/typescript/react && bun run build
python scripts/validate-spec-tasks-parity.py .specify/specs/121-vercel-build-config
```
