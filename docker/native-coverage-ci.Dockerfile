FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt-get update -qq \
  && apt-get install -y --no-install-recommends \
    bash \
    ca-certificates \
    cmake \
    ninja-build \
    gcc \
    g++ \
    libomp-dev \
    gcovr \
    lcov \
    python3 \
    git \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

CMD ["bash", "scripts/run-native-coverage.sh", "--build-dir", "out/native-coverage-ci-docker", "--output-dir", "artifacts/native/coverage-ci-docker"]