# React app (spec 009). Bun. CRITICAL: install shared/ui first, never copy
# host node_modules (covered by .dockerignore).
FROM oven/bun:1.1
WORKDIR /workspace
# Shared UI must install BEFORE the app so workspace resolution works.
COPY src/typescript/shared/ui/package.json ./src/typescript/shared/ui/
RUN bun install --cwd /workspace/src/typescript/shared/ui || true
COPY src/typescript/shared ./src/typescript/shared
COPY src/typescript/react/package.json ./src/typescript/react/
RUN cd src/typescript/react && bun install
COPY src/typescript/react ./src/typescript/react
ENV VITE_BANANA_API_BASE_URL=http://api:8080
EXPOSE 5173
WORKDIR /workspace/src/typescript/react
CMD ["bun", "run", "dev", "--host", "0.0.0.0"]
