# Plan: SSL Certificate Management (155)

## Design Phase

Vercel auto-provisions and renews SSL certificates for all domains.

Certificate lifecycle:
- **Provisioning**: happens automatically on first custom domain set.
- **Renewal**: automatic before expiration (typically every 90 days).
- **Revocation**: automatic on domain removal.

## Success Criteria

- Certificate provisioned for banana.engineer.
- HTTPS connections work without errors.
- Expiration alerts configured.
