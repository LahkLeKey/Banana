# TypeScript API runtime image with native FFI path contract.
FROM oven/bun:1.3

RUN apt-get update && apt-get install -y libpq5 curl && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY src/typescript/api/package.json ./src/typescript/api/
COPY src/typescript/api/bun.lock ./src/typescript/api/
COPY src/typescript/api/prisma.config.ts ./src/typescript/api/
RUN cd src/typescript/api && bun install --frozen-lockfile --production || bun install --no-frozen-lockfile --production
COPY src/typescript/api ./src/typescript/api

ENV NODE_ENV=production
ENV PORT=8080
ENV BANANA_NATIVE_PATH=/native

WORKDIR /workspace/src/typescript/api
EXPOSE 8080
CMD ["bun", "run", "src/index.ts"]
