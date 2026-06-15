FROM oven/bun:1.2.13-debian

WORKDIR /app/src/typescript/api

RUN apt-get update && apt-get install -y --no-install-recommends \
	cmake \
	ninja-build \
	gcc \
	g++ \
	make \
	&& rm -rf /var/lib/apt/lists/*

COPY src/typescript/api/package.json src/typescript/api/bun.lock ./
RUN bun install

COPY src/typescript/api/src ./src
COPY notebooks /app/notebooks
COPY artifacts/native /app/artifacts/native
COPY src/native /app/src/native
COPY tests/native /app/tests/native

RUN cmake -S /app/src/native -B /app/out/native -G Ninja -DCMAKE_BUILD_TYPE=Release \
	&& cmake --build /app/out/native --target banana_native --parallel \
	&& cp /app/out/native/libbanana_native.so /app/src/typescript/api/libbanana_native.so

ENV HOST=0.0.0.0
ENV PORT=8081
ENV BANANA_ENGINE_ADAPTER=inmemory

EXPOSE 8081

CMD ["bun", "run", "start"]