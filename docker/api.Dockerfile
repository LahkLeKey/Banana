FROM mcr.microsoft.com/dotnet/sdk:8.0 AS build
WORKDIR /src

COPY Directory.Build.props ./
COPY src/api/CInteropSharp.Api.csproj src/api/

RUN dotnet restore src/api/CInteropSharp.Api.csproj

COPY src/api src/api

RUN dotnet publish src/api/CInteropSharp.Api.csproj -c Release -o /app/publish --no-restore

FROM mcr.microsoft.com/dotnet/aspnet:8.0
WORKDIR /app

ENV ASPNETCORE_URLS=http://+:8080

COPY --from=build /app/publish ./

EXPOSE 8080

ENTRYPOINT ["dotnet", "CInteropSharp.Api.dll"]
