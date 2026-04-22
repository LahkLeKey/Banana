FROM oven/bun:1.3.9

WORKDIR /workspace/src/typescript/api

COPY src/typescript/api/package.json ./package.json
COPY src/typescript/api/bun.lock ./bun.lock
COPY src/typescript/api/tsconfig.json ./tsconfig.json
COPY src/typescript/api/prisma.config.ts ./prisma.config.ts
COPY src/typescript/api/prisma ./prisma
COPY src/typescript/api/src ./src
COPY src/typescript/api/tests ./tests

RUN bun install --frozen-lockfile
RUN bun run check

EXPOSE 8180

CMD ["bun", "run", "start:gateway"]
