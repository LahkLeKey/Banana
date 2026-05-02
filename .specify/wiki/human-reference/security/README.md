# Security

> [Home](../Home.md) › Security

Security posture, threat model, and header policies.

## Pages In This Section

| Page | Description |
|------|-------------|
| [Threat Model](threat-model.md) | STRIDE analysis, data flow, secret inventory, open risks |
| [CSP and Security Headers](csp-and-headers.md) | Response headers applied by both API layers |

## Key Contacts

- Open risks are tracked in the [Threat Model](threat-model.md#open-risks-tracked).
- JWT auth is documented in `src/typescript/api/src/middleware/auth.ts` and `Program.cs`.
