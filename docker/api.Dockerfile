# ASP.NET API (spec 007). BANANA_NATIVE_PATH points at the mounted .so.
FROM mcr.microsoft.com/dotnet/sdk:8.0 AS build
WORKDIR /src
COPY Directory.Build.props ./
COPY src/c-sharp/asp.net ./src/c-sharp/asp.net
RUN dotnet publish src/c-sharp/asp.net/Banana.Api.csproj -c Release -o /app

FROM mcr.microsoft.com/dotnet/aspnet:8.0
RUN apt-get update && apt-get install -y libpq5 curl && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build /app .
ENV ASPNETCORE_URLS=http://+:8080
ENV BANANA_NATIVE_PATH=/native
EXPOSE 8080
ENTRYPOINT ["dotnet", "Banana.Api.dll"]
