# Electron (spec 010). NPM (NOT bun) for lockfile reproducibility.
# CXXFLAGS=-fpermissive for native modules. Normalize CRLF in copied scripts.
FROM node:20-bookworm
RUN apt-get update && apt-get install -y \
    libgl1 libglib2.0-0 libnss3 libxss1 libasound2 libgtk-3-0 \
    libxshmfence1 dbus-x11 xvfb \
 && rm -rf /var/lib/apt/lists/*
ENV CXXFLAGS=-fpermissive
WORKDIR /workspace
COPY src/typescript/electron/package.json src/typescript/electron/package-lock.json* ./src/typescript/electron/
RUN cd src/typescript/electron && (npm ci --omit=dev || npm install --omit=dev)
COPY src/typescript/electron ./src/typescript/electron
RUN find ./src/typescript/electron -name '*.sh' -exec sed -i 's/\r$//' {} +
WORKDIR /workspace/src/typescript/electron
CMD ["npm", "run", "smoke"]
