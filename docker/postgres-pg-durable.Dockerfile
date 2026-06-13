FROM postgres:17-bookworm

ARG PG_DURABLE_VERSION=0.2.2
ARG TARGETARCH=amd64

RUN apt-get update \
    && apt-get install -y --no-install-recommends ca-certificates curl \
    && rm -rf /var/lib/apt/lists/*

RUN set -eux; \
    package="pg-durable-postgresql-17_${PG_DURABLE_VERSION}-1_${TARGETARCH}.deb"; \
    url="https://github.com/microsoft/pg_durable/releases/download/v${PG_DURABLE_VERSION}/${package}"; \
    curl -fsSL "$url" -o "/tmp/${package}"; \
        if ! dpkg -i "/tmp/${package}"; then \
            apt-get update; \
            apt-get install -y --no-install-recommends -f; \
        fi; \
    rm -f "/tmp/${package}"; \
    rm -rf /var/lib/apt/lists/*

COPY docker/pg-durable/initdb/010-create-extension.sh /docker-entrypoint-initdb.d/010-create-extension.sh

RUN chmod +x /docker-entrypoint-initdb.d/010-create-extension.sh

CMD ["postgres", "-c", "shared_preload_libraries=pg_durable", "-c", "pg_durable.worker_role=postgres"]