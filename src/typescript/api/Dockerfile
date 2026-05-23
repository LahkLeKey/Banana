FROM oven/bun:1.2.13-debian

WORKDIR /app/src/typescript/api

COPY src/typescript/api/package.json src/typescript/api/bun.lock ./
RUN bun install

COPY src/typescript/api/src ./src
COPY src/typescript/api/wasm ./wasm

ENV HOST=0.0.0.0
ENV PORT=8081
ENV BANANA_ENGINE_ADAPTER=inmemory

EXPOSE 8081

CMD ["bun", "run", "start"]