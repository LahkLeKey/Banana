# Local-workflow harness — mirrors GH Actions environment for local runs.
FROM ubuntu:24.04
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    bash curl git python3 python3-pip jq ca-certificates \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /workspace
COPY . .
RUN find scripts -name '*.sh' -exec sed -i 's/\r$//' {} +
CMD ["bash"]
