# Test runner (spec 014). dotnet + bun + native artifacts.
FROM mcr.microsoft.com/dotnet/sdk:8.0
RUN apt-get update && apt-get install -y curl unzip libpq5 \
 && curl -fsSL https://bun.sh/install | bash \
 && rm -rf /var/lib/apt/lists/*
ENV PATH="/root/.bun/bin:${PATH}"
WORKDIR /workspace
COPY . .
ENV BANANA_NATIVE_PATH=/native
CMD ["bash", "scripts/run-tests-with-coverage.sh"]
