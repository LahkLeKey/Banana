FROM node:18-bookworm-slim

ENV ELECTRON_SKIP_BINARY_DOWNLOAD=1

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        python3 \
        make \
        g++ \
        libffi-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace/src/typescript/electron

COPY src/typescript/electron/package.json src/typescript/electron/package-lock.json ./
RUN npm ci --omit=dev

COPY src/typescript/electron ./

CMD ["npm", "run", "example"]
