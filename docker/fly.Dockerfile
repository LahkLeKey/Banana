# Fly.io production image — builds native .so then ASP.NET API in one image.
# Stage 1: build native C library
FROM ubuntu:24.04 AS native-build
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential cmake libpq-dev ca-certificates \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /workspace
COPY CMakeLists.txt ./
COPY src/native ./src/native
RUN cmake -S . -B build/native -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build/native --config Release \
 && mkdir -p out/native/bin \
 && cp build/native/libbanana_native.so out/native/bin/

# Stage 2: build ASP.NET API
FROM mcr.microsoft.com/dotnet/sdk:8.0 AS api-build
WORKDIR /src
COPY Directory.Build.props ./
COPY src/c-sharp/asp.net ./src/c-sharp/asp.net
RUN dotnet publish src/c-sharp/asp.net/Banana.Api.csproj -c Release -o /app

# Stage 3: runtime
FROM mcr.microsoft.com/dotnet/aspnet:8.0
RUN apt-get update && apt-get install -y libpq5 curl && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=api-build /app .
COPY --from=native-build /workspace/out/native/bin/libbanana_native.so /native/libbanana_native.so
ENV ASPNETCORE_URLS=http://+:8080
ENV BANANA_NATIVE_PATH=/native
EXPOSE 8080
ENTRYPOINT ["dotnet", "Banana.Api.dll"]
