FROM mcr.microsoft.com/dotnet/sdk:8.0 AS build
WORKDIR /src

COPY Directory.Build.props ./
COPY src/c-sharp/asp.net/Banana.Api.csproj src/c-sharp/asp.net/

RUN dotnet restore src/c-sharp/asp.net/Banana.Api.csproj

COPY src/c-sharp/asp.net src/c-sharp/asp.net

RUN dotnet publish src/c-sharp/asp.net/Banana.Api.csproj -c Release -o /app/publish --no-restore

FROM mcr.microsoft.com/dotnet/aspnet:8.0
WORKDIR /app

RUN apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
		curl \
		libpq5 \
	&& rm -rf /var/lib/apt/lists/*

ENV ASPNETCORE_URLS=http://+:8080

COPY --from=build /app/publish ./

EXPOSE 8080

ENTRYPOINT ["dotnet", "Banana.Api.dll"]
