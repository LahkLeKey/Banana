#!/bin/sh
set -eu

if [ "${1:-}" != "postgres" ]; then
  exec "$@"
fi

: "${POSTGRES_USER:=postgres}"
: "${POSTGRES_DB:=postgres}"
: "${POSTGRES_PASSWORD:?POSTGRES_PASSWORD required}"
: "${PGDATA:=/var/lib/postgresql/data/pgdata}"

mkdir -p "$PGDATA"
chown -R postgres:postgres /var/lib/postgresql
chmod 700 "$PGDATA" || true

if [ ! -s "$PGDATA/PG_VERSION" ]; then
  tmp_pw="$(mktemp)"
  trap 'rm -f "$tmp_pw"' EXIT
  printf '%s' "$POSTGRES_PASSWORD" > "$tmp_pw"
  chown postgres:postgres "$tmp_pw"
  chmod 600 "$tmp_pw"

  gosu postgres initdb -D "$PGDATA" --username="$POSTGRES_USER" --pwfile="$tmp_pw"
  {
    echo "host all all all scram-sha-256"
    echo "host all all all md5"
  } >> "$PGDATA/pg_hba.conf"

  gosu postgres pg_ctl -D "$PGDATA" -o "-c listen_addresses=localhost -c shared_preload_libraries=pg_durable -c pg_durable.worker_role=postgres" -w start

  if ! gosu postgres psql --username "$POSTGRES_USER" --dbname postgres -tc "SELECT 1 FROM pg_database WHERE datname='$POSTGRES_DB'" | grep -q 1; then
    gosu postgres psql --username "$POSTGRES_USER" --dbname postgres -c "CREATE DATABASE \"$POSTGRES_DB\";"
  fi

  gosu postgres psql --username "$POSTGRES_USER" --dbname "$POSTGRES_DB" -c "CREATE EXTENSION IF NOT EXISTS pg_durable;"
  gosu postgres pg_ctl -D "$PGDATA" -m fast -w stop
fi

exec gosu postgres postgres -D "$PGDATA" -c shared_preload_libraries=pg_durable -c pg_durable.worker_role=postgres -c listen_addresses='*'