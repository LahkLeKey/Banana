# Vault Integration

> Owner: ASP.NET API domain  
> Status: Implemented (spec #082)

This document describes how the Banana ASP.NET API reads secrets from HashiCorp Vault and how to migrate from environment-variable-based secrets.

---

## Architecture

Banana uses [VaultSharp](https://github.com/rajanadar/VaultSharp) to pull secrets from a Vault KV v2 store at startup. This replaces direct environment-variable injection for sensitive values (`BANANA_PG_CONNECTION`, `BANANA_JWT_SECRET`).

```
Kubernetes Pod / Docker Container
  └─ ASP.NET API (startup)
       └─ VaultSharp client → Vault KV v2
            ├─ secret/banana/postgres  (key: connection)
            └─ secret/banana/jwt       (key: secret)
```

---

## NuGet Dependency

Add to `src/c-sharp/asp.net/Banana.Api.csproj`:

```xml
<PackageReference Include="VaultSharp" Version="1.16.*" />
```

---

## Configuration

`appsettings.json` / environment:

```json
{
  "Vault": {
    "Address": "http://vault:8200",
    "Token": "",
    "MountPath": "secret",
    "PostgresPath": "banana/postgres",
    "JwtPath": "banana/jwt"
  }
}
```

Override `Vault__Token` via environment or Kubernetes Secret for non-dev environments.

---

## Env-var → Vault Migration Path

| Old env var | Vault path | Key |
|---|---|---|
| `BANANA_PG_CONNECTION` | `secret/banana/postgres` | `connection` |
| `BANANA_JWT_SECRET` | `secret/banana/jwt` | `secret` |

1. Bootstrap Vault: `bash scripts/vault-init.sh` (local dev uses `banana-dev-token`).
2. Set `Vault__Address` and `Vault__Token` in the runtime environment.
3. Remove direct env-var injection of the above secrets from compose / Helm values.

---

## Local Development

Start the Vault sidecar via the `secrets-management` compose profile:

```bash
docker compose --profile secrets-management up vault
bash scripts/vault-init.sh --dev
```

Then start the API normally; it will read from Vault at `http://localhost:8200`.

---

## CI

> **Warning:** CI pipelines continue to use **GitHub Actions secrets** (`BANANA_PG_CONNECTION`, `BANANA_JWT_SECRET`) injected as environment variables. Vault integration is not active in CI. Do not remove the GitHub Actions secrets until Vault is deployed to a shared environment and CI has been migrated.

---

## References

- [vault-init.sh](../../scripts/vault-init.sh)
- [Vault Smoke workflow](../../.github/workflows/vault-smoke.yml)
- [VaultSharp docs](https://github.com/rajanadar/VaultSharp)
