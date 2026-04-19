FROM node:20-bookworm-slim

WORKDIR /workspace/src/typescript/react-native

COPY src/typescript/react-native/package.json src/typescript/react-native/package-lock.json ./
COPY src/typescript/shared/ui/package.json /workspace/src/typescript/shared/ui/package.json
COPY src/typescript/shared/ui/src /workspace/src/typescript/shared/ui/src
RUN npm ci --no-audit --no-fund

COPY src/typescript/react-native/ ./

EXPOSE 8081

CMD ["npx", "expo", "start", "--web", "--host", "lan", "--port", "8081"]
