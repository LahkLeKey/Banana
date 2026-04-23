FROM python:3.12-bookworm@sha256:49de7aa80568e7a112035322a6a961cd55774862b35198886f7508223abf6ca1

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        bash \
        ca-certificates \
        curl \
        gh \
        git \
        jq \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

CMD ["bash", "-lc", "python --version && git --version && gh --version"]
