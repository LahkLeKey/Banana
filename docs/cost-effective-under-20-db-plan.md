# Banana under $20/month deployment plan

## Goal

Keep total monthly cost under $20 while running:
- API on Fly
- Self-hosted database endpoint for API persistence

This plan uses plain PostgreSQL for now, not full Neon platform components. It is the cheapest reliable path and is fully compatible with current Banana API env aliases.

## Monthly budget target

- VM (2 vCPU / 4 GB class): about $6 to $10
- Block storage + snapshots/backups: about $2 to $6
- DNS/domain amortized: about $1 to $2
- Total target: about $9 to $18

## Provider options (pick one)

- Hetzner CX22/CAX11-class + volume
- OVH VPS entry tier
- Lightsail smallest Linux with attached storage

Pick the region closest to Fly iad latency-wise (US East if possible).

## Architecture

- Fly app: banana-api
- DB VM: db.banana.engineer
- PostgreSQL listens on 5432 (or tunneled via private networking if available)
- API gets one authoritative URL copied into all aliases:
  - NEON_DATABASE_URL
  - DATABASE_URL
  - BANANA_PG_CONNECTION

## VM bootstrap (Ubuntu 22.04/24.04)

Run on the DB VM as root or sudo user:

apt update
apt install -y postgresql postgresql-contrib ufw

# Basic firewall
ufw allow OpenSSH
ufw allow 5432/tcp
ufw --force enable

# Create DB + user
sudo -u postgres psql -c "CREATE USER banana_app WITH PASSWORD 'CHANGE_ME_STRONG_PASSWORD';"
sudo -u postgres psql -c "CREATE DATABASE banana OWNER banana_app;"
sudo -u postgres psql -d banana -c "CREATE EXTENSION IF NOT EXISTS pgcrypto;"

# Tune auth and bind address
# Edit /etc/postgresql/*/main/postgresql.conf
#   listen_addresses = '*'
# Edit /etc/postgresql/*/main/pg_hba.conf
#   host    banana    banana_app    0.0.0.0/0    scram-sha-256

systemctl restart postgresql
systemctl enable postgresql

## TLS and network hardening (minimum)

- Restrict inbound 5432 to Fly egress ranges or your own trusted ranges where possible.
- Prefer WireGuard/private networking if your provider supports it.
- If exposing publicly, enforce strong password and rotate quarterly.

## Connection string

Build this URL (use your actual host/password):

postgresql://banana_app:CHANGE_ME_STRONG_PASSWORD@db.banana.engineer:5432/banana?sslmode=require

If your VM setup does not terminate TLS yet, use sslmode=disable temporarily and prioritize adding TLS soon.

## Set Fly secrets

From repo root:

fly secrets set -a banana-api \
  NEON_DATABASE_URL="postgresql://banana_app:CHANGE_ME_STRONG_PASSWORD@db.banana.engineer:5432/banana?sslmode=require" \
  DATABASE_URL="postgresql://banana_app:CHANGE_ME_STRONG_PASSWORD@db.banana.engineer:5432/banana?sslmode=require" \
  BANANA_PG_CONNECTION="postgresql://banana_app:CHANGE_ME_STRONG_PASSWORD@db.banana.engineer:5432/banana?sslmode=require"

## Deploy API from repository root

fly deploy -a banana-api --config src/typescript/api/fly.toml

## Smoke tests

curl -i https://banana-api.fly.dev/health
curl -i -X POST https://banana-api.fly.dev/v1/gameplay/sessions/admissions \
  -H "content-type: application/json" \
  -H "x-player-id: smoke-player" \
  -H "x-idempotency-key: smoke-1" \
  -d '{"requestedRole":"dps"}'
curl -i https://banana-api.fly.dev/v1/player/insights -H "x-player-id: smoke-player"

## Backups under budget

- Daily pg_dump compressed to object storage bucket
- Keep 7 daily + 4 weekly backups
- Test restore once per month

Example cron job:

0 3 * * * pg_dump -Fc -h 127.0.0.1 -U banana_app banana > /var/backups/banana_$(date +\%F).dump

## When to upgrade beyond this plan

Move to full Neon self-hosted platform components when one of these is true:
- Budget is above $50 to $100 monthly
- You require branching/time-travel workflows from Neon platform semantics
- You need higher durability/availability than single-VM Postgres can provide

## Banana-specific note

Current API startup is strict about database aliases when BANANA_NEON_STRICT=true.
If Fly secrets are unset or mismatched, the app will fail at boot.
