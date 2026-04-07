FROM node:14-bullseye

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        python3 \
        make \
        g++ \
        libffi-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace/src/electron

COPY src/electron/package.json ./
RUN npm install

COPY src/electron ./

CMD ["npm", "run", "example"]
