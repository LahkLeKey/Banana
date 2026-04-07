FROM mcr.microsoft.com/dotnet/sdk:8.0

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

COPY . ./

RUN cmake -S . -B /workspace/build/native -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /workspace/build/native --config Release

ENV CINTEROP_NATIVE_PATH=/workspace/build/native/bin

CMD ["bash", "-lc", "dotnet test tests/unit/CInteropSharp.UnitTests.csproj -c Release && dotnet test tests/integration/CInteropSharp.IntegrationTests.csproj -c Release"]
