FROM oven/bun:1.2

# Preinstall native toolchain so API startup does not stall on runtime apt installs.
RUN apt-get update \
  && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    cmake \
    build-essential \
    pkg-config \
    libomp-dev \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace/src/typescript/api
