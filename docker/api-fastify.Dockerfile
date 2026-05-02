# Fastify TypeScript API (spec 008). Bun + Prisma 7 (prisma.config.ts).
FROM oven/bun:1.1
WORKDIR /workspace
COPY src/typescript/api/package.json src/typescript/api/bun.lock* ./src/typescript/api/
COPY src/typescript/api/prisma ./src/typescript/api/prisma
COPY src/typescript/api/prisma.config.ts ./src/typescript/api/
RUN cd src/typescript/api && bun install --frozen-lockfile --production || bun install --production
COPY src/typescript/api ./src/typescript/api
ENV NODE_ENV=production
EXPOSE 8081
WORKDIR /workspace/src/typescript/api
CMD ["bun", "run", "src/index.ts"]
