---
name: banana-discovery
description: Discover Banana architecture, touched domains, runtime contracts, and validation entry points before planning or implementation.
argument-hint: Describe the request, suspected area, or files you want mapped.
---

# Banana Discovery

Use this skill when you need to map a request onto Banana's actual architecture before planning or coding.

## What To Do

1. Start from the repo-wide guidance in [../../copilot-instructions.md](../../copilot-instructions.md).
2. Read the architecture summary in [../../../docs/developer-onboarding.md](../../../docs/developer-onboarding.md).
3. Identify which of Banana's four domains are touched: native C, ASP.NET, React/Electron, or delivery/runtime.
4. Map the touched files to the narrowest helper agents before naming a broader coordinating agent.
5. Locate the real entry points for build, run, test, and validation before proposing changes.
6. Call out cross-layer contracts up front, especially `BANANA_PG_CONNECTION`, `BANANA_NATIVE_PATH`, and `VITE_BANANA_API_BASE_URL`.

## Output

- Touched domains
- Recommended helper or coordinating agents
- Relevant files and folders
- Existing entry points to reuse
- Cross-layer contracts to preserve
- Recommended implementation and validation path

## Resources

- Architecture map: [architecture-map.md](./architecture-map.md)
- Helper routing: [../banana-agent-decomposition/helper-matrix.md](../banana-agent-decomposition/helper-matrix.md)
