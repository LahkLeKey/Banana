# React Native / Expo web preview (spec 011).
FROM oven/bun:1.1
WORKDIR /workspace
COPY src/typescript/shared/ui/package.json ./src/typescript/shared/ui/
RUN bun install --cwd /workspace/src/typescript/shared/ui || true
COPY src/typescript/shared ./src/typescript/shared
COPY src/typescript/tsconfig.base.json ./src/typescript/tsconfig.base.json
COPY src/typescript/react-native/package.json ./src/typescript/react-native/
RUN cd src/typescript/react-native && bun install
COPY src/typescript/react-native ./src/typescript/react-native
EXPOSE 8081 19006
WORKDIR /workspace/src/typescript/react-native
CMD ["bun", "run", "web"]
