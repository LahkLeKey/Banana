FROM postgres:18-bookworm

ARG PG_DURABLE_VERSION=0.2.2
ARG TARGETARCH=amd64

RUN apt-get update \
    && apt-get install -y --no-install-recommends ca-certificates curl \
    && rm -rf /var/lib/apt/lists/*

RUN set -eux; \
    package="pg-durable-postgresql-18_${PG_DURABLE_VERSION}-1_${TARGETARCH}.deb"; \
    url="https://github.com/microsoft/pg_durable/releases/download/v${PG_DURABLE_VERSION}/${package}"; \
    curl -fsSL "$url" -o "/tmp/${package}"; \
        if ! dpkg -i "/tmp/${package}"; then \
            apt-get update; \
            apt-get install -y --no-install-recommends -f; \
        fi; \
    rm -f "/tmp/${package}"; \
    rm -rf /var/lib/apt/lists/*

# Preload pg_durable on every start and create the extension on first init
COPY docker/pg-durable/initdb/010-create-extension.sh /docker-entrypoint-initdb.d/010-create-extension.sh
RUN chmod +x /docker-entrypoint-initdb.d/010-create-extension.sh
COPY docker/pg-durable/banana-entrypoint.sh /usr/local/bin/banana-db-v2-entrypoint.sh
RUN chmod +x /usr/local/bin/banana-db-v2-entrypoint.sh

ENV PGDATA=/var/lib/postgresql/data/pgdata

# The upstream image defaults to a non-root user. Running as root here allows
# docker-entrypoint.sh to prepare/chown mounted Fly volumes before dropping privileges.
USER root

ENTRYPOINT ["bash", "/usr/local/bin/banana-db-v2-entrypoint.sh"]
CMD ["postgres", "-c", "shared_preload_libraries=pg_durable", "-c", "pg_durable.worker_role=postgres"]