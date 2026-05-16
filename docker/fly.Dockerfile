# Fly.io production image — builds native .so then TypeScript API in one image.

# Stage 1: build native C library
FROM ubuntu:24.04 AS native-build
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

# Stage 2: install API runtime dependencies
FROM oven/bun:1.3 AS api-build
WORKDIR /workspace
COPY src/typescript/api/package.json ./src/typescript/api/
COPY src/typescript/api/bun.lock ./src/typescript/api/
COPY src/typescript/api/prisma.config.ts ./src/typescript/api/
RUN cd src/typescript/api && bun install --frozen-lockfile --production || bun install --no-frozen-lockfile --production
COPY src/typescript/api ./src/typescript/api

# Stage 3: runtime
FROM oven/bun:1.3
RUN apt-get update && apt-get install -y libpq5 curl && rm -rf /var/lib/apt/lists/*
WORKDIR /workspace/src/typescript/api
COPY --from=api-build /workspace/src/typescript/api ./
COPY --from=native-build /workspace/out/native/bin/libbanana_native.so /native/libbanana_native.so
ENV NODE_ENV=production
ENV PORT=8080
ENV BANANA_NATIVE_PATH=/native
EXPOSE 8080
CMD ["bun", "run", "src/index.ts"]
