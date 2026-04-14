FROM mcr.microsoft.com/dotnet/sdk:8.0

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        gcovr \
        libpq-dev \
        postgresql-client \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

COPY . ./

RUN cmake -S . -B /workspace/build/native -DCMAKE_BUILD_TYPE=Debug -DBANANA_ENABLE_POSTGRES=ON -DBANANA_ENABLE_NATIVE_TESTS=ON -DBANANA_ENABLE_COVERAGE=ON \
    && cmake --build /workspace/build/native --config Release

ENV BANANA_NATIVE_PATH=/workspace/build/native/bin

COPY scripts/run-tests-with-coverage.sh /workspace/scripts/run-tests-with-coverage.sh
RUN chmod +x /workspace/scripts/run-tests-with-coverage.sh

CMD ["bash", "-lc", "/workspace/scripts/run-tests-with-coverage.sh"]
