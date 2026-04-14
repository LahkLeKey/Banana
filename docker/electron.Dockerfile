FROM node:14-bullseye

ENV ELECTRON_SKIP_BINARY_DOWNLOAD=1

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        python3 \
        make \
        g++ \
        libffi-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace/src/typescript/electron

COPY src/typescript/electron/package.json ./
RUN npm install

COPY src/typescript/electron ./

CMD ["npm", "run", "example"]
