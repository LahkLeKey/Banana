FROM ubuntu:24.04

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        libpq-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

COPY CMakeLists.txt ./
COPY src/native ./src/native

RUN cmake -S . -B /workspace/out/native -DCMAKE_BUILD_TYPE=Release -DCINTEROP_ENABLE_POSTGRES=ON -DCINTEROP_ENABLE_NATIVE_TESTS=OFF \
    && cmake --build /workspace/out/native --config Release

CMD ["bash", "-lc", "ls -l /workspace/out/native/bin"]
