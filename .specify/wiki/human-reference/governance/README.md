# Governance

> [Home](../Home.md) › Governance

Contracts, policies, and autonomous systems governance.

## Pages In This Section

| Page | Description |
|------|-------------|
| [API Parity Governance](api-parity-governance.md) | How parity between ASP.NET and Fastify is enforced |
| [Autonomous Self-Training](autonomous-self-training.md) | How models retrain themselves and the neuroscience layer |

## Key Contracts

- API parity gate runs on every PR via `compose-ci.yml`.
- Self-training is opt-in; controlled by `--neuro-profile` flag.
