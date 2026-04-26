# Native builder (spec 012) — produces libbanana_native.so + headers.
FROM ubuntu:24.04 AS build
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential cmake libpq-dev ca-certificates \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /workspace
COPY CMakeLists.txt ./
COPY src/native ./src/native
RUN cmake -S . -B build/native -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build/native --config Release \
 && mkdir -p out/native/bin \
 && cp build/native/libbanana_native.so out/native/bin/

FROM ubuntu:24.04
COPY --from=build /workspace/out/native/bin/libbanana_native.so /workspace/out/native/bin/libbanana_native.so
