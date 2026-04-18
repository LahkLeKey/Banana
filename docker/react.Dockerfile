FROM oven/bun:1.3.9

WORKDIR /workspace/src/typescript/react

COPY src/typescript/react/package.json src/typescript/react/bun.lock ./
RUN bun install

COPY src/typescript/react/ ./

EXPOSE 5173

CMD ["bun", "run", "dev", "--host", "0.0.0.0", "--port", "5173"]
