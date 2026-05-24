# Banana under $20 runbook: Hetzner Postgres + Fly API

## Outcome

By the end of this runbook you will have:
- A low-cost PostgreSQL VM on Hetzner
- A Fly-deployed API using that DB URL through all Banana DB aliases
- Live smoke-test verification

## Budget target

- Hetzner VM (CX22): about $6-7/month
- Snapshot/backup allowance: about $2-4/month
- DNS/domain amortized: about $1-2/month
- Expected total: about $9-13/month

## 1) Provision the DB VM (Hetzner)

Create one Ubuntu 24.04 server:
- Type: CX22
- Region: US East nearest practical region (or closest to Fly iad equivalent)
- Volume: optional 20-40 GB if you prefer data on attached volume

Record:
- Public IP
- SSH key access

## 2) Point DNS

Create an A record:
- db.banana.engineer -> <VM_PUBLIC_IP>

Wait until:
- nslookup db.banana.engineer resolves to your VM IP

## 3) Bootstrap PostgreSQL on VM

SSH into VM and run:

sudo apt update
sudo apt install -y postgresql postgresql-contrib ufw

# Firewall
sudo ufw allow OpenSSH
sudo ufw allow 5432/tcp
sudo ufw --force enable

# Create user + DB
sudo -u postgres psql -c "CREATE USER banana_app WITH PASSWORD 'REPLACE_WITH_STRONG_PASSWORD';"
sudo -u postgres psql -c "CREATE DATABASE banana OWNER banana_app;"
sudo -u postgres psql -d banana -c "CREATE EXTENSION IF NOT EXISTS pgcrypto;"

## 4) Allow remote app connections

Edit PostgreSQL config files:

- /etc/postgresql/16/main/postgresql.conf (version may vary)
  - set: listen_addresses = '*'

- /etc/postgresql/16/main/pg_hba.conf
  - add:
    host    banana    banana_app    0.0.0.0/0    scram-sha-256

Restart and enable:

sudo systemctl restart postgresql
sudo systemctl enable postgresql

## 5) Verify DB connectivity from local machine

Run from your workstation:

psql "postgresql://banana_app:REPLACE_WITH_STRONG_PASSWORD@db.banana.engineer:5432/banana?sslmode=disable" -c "select 1;"

If this fails, fix network/firewall first.

## 6) Set Fly secrets for Banana API

From repo root on your workstation:

fly secrets set -a banana-api \
  NEON_DATABASE_URL="postgresql://banana_app:REPLACE_WITH_STRONG_PASSWORD@db.banana.engineer:5432/banana?sslmode=disable" \
  DATABASE_URL="postgresql://banana_app:REPLACE_WITH_STRONG_PASSWORD@db.banana.engineer:5432/banana?sslmode=disable" \
  BANANA_PG_CONNECTION="postgresql://banana_app:REPLACE_WITH_STRONG_PASSWORD@db.banana.engineer:5432/banana?sslmode=disable"

Notes:
- The current Banana runtime enforces alias consistency in strict mode.
- All three values must match.

## 7) Deploy API to Fly

From repo root:

fly deploy -a banana-api --config src/typescript/api/fly.toml

## 8) Smoke test live API

Run:

curl -i https://banana-api.fly.dev/health

curl -i -X POST https://banana-api.fly.dev/v1/gameplay/sessions/admissions \
  -H "content-type: application/json" \
  -H "x-player-id: smoke-player" \
  -H "x-idempotency-key: smoke-1" \
  -d '{"requestedRole":"dps"}'

curl -i https://banana-api.fly.dev/v1/player/insights -H "x-player-id: smoke-player"

Expected:
- /health returns HTTP 200
- v1 endpoints return non-timeout responses

## 9) If deploy still fails startup

Check Fly status/logs:

fly status -a banana-api
fly logs -a banana-api --no-tail | tail -n 120

Look for DB alias or connection errors and correct secrets.

## 10) Minimal backup setup (cost-effective)

Option A (fast): daily dump to local disk + weekly manual offsite copy

Create script /usr/local/bin/banana-db-backup.sh:

#!/usr/bin/env bash
set -euo pipefail
export PGPASSWORD='REPLACE_WITH_STRONG_PASSWORD'
STAMP=$(date +%F)
pg_dump -h 127.0.0.1 -U banana_app -d banana -Fc -f "/var/backups/banana_${STAMP}.dump"
find /var/backups -name 'banana_*.dump' -mtime +7 -delete

Then:

sudo chmod +x /usr/local/bin/banana-db-backup.sh
sudo crontab -e
# Add:
# 0 3 * * * /usr/local/bin/banana-db-backup.sh

## 11) Hardening follow-ups (recommended)

- Restrict 5432 inbound to Fly egress/IP ranges instead of 0.0.0.0/0
- Migrate to TLS-enabled Postgres endpoint and switch sslmode=require
- Add fail2ban and OS unattended security upgrades

## 12) Future migration path

When budget increases, migrate from single-VM Postgres to a more durable stack (managed Neon or full Neon self-hosted platform components). Keep Banana app alias contract unchanged to simplify cutover.
