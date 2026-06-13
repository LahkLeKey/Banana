#!/usr/bin/env bash
# pg_durable must be loaded via shared_preload_libraries before this runs.
# docker-entrypoint-initdb.d scripts run as the postgres superuser on first init.
set -euo pipefail

psql -v ON_ERROR_STOP=1 --username "$POSTGRES_USER" --dbname postgres <<'SQL'
CREATE EXTENSION IF NOT EXISTS pg_durable;
SQL
