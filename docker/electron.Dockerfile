FROM node:20-bookworm-slim

ENV CXXFLAGS=-fpermissive \
    ELECTRON_DISABLE_SECURITY_WARNINGS=true

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        python3 \
        make \
        g++ \
        libffi-dev \
        libpq5 \
        xauth \
        ca-certificates \
        libasound2 \
        libatk-bridge2.0-0 \
        libatk1.0-0 \
        libcups2 \
        libdbus-1-3 \
        libdrm2 \
        libgbm1 \
        libgl1 \
        libgtk-3-0 \
        libnspr4 \
        libnss3 \
        libx11-6 \
        libx11-xcb1 \
        libxcb-dri3-0 \
        libxcomposite1 \
        libxdamage1 \
        libxext6 \
        libxfixes3 \
        libxkbcommon0 \
        libxrandr2 \
        libxshmfence1 \
        libxss1 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace/src/typescript/electron

COPY src/typescript/electron/package.json src/typescript/electron/package-lock.json ./
RUN npm ci --omit=dev

COPY src/typescript/electron ./

CMD ["npm", "run", "example"]
