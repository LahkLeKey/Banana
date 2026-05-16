# Test runner (spec 014). Bun + native artifacts.
FROM oven/bun:1.3
RUN apt-get update && apt-get install -y curl unzip libpq5 python3 \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /workspace
COPY . .
ENV BANANA_NATIVE_PATH=/native
CMD ["bash", "scripts/run-tests-with-coverage.sh"]
