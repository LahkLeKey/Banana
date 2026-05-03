using System.Net;
using System.Text.Json;

using Xunit;

namespace Banana.E2eTests.Contracts;

public sealed class ApiDocsFrontendSmokeContractTests
{
    [Fact]
    public async Task ApiDocsRouteAndSwaggerSpecAreReachable()
    {
        var frontendBase = Environment.GetEnvironmentVariable("BANANA_E2E_FRONTEND_BASE_URL") ?? "https://banana.engineer";
        var apiBase = Environment.GetEnvironmentVariable("BANANA_E2E_SWAGGER_API_BASE_URL") ?? "https://api.banana.engineer";

        using var client = new HttpClient { Timeout = TimeSpan.FromSeconds(20) };

        using var docsResponse = await client.GetAsync($"{frontendBase.TrimEnd('/')}/api-docs");
        Assert.Equal(HttpStatusCode.OK, docsResponse.StatusCode);

        using var specResponse = await client.GetAsync($"{apiBase.TrimEnd('/')}/swagger/v1/swagger.json");
        Assert.Equal(HttpStatusCode.OK, specResponse.StatusCode);

        await using var body = await specResponse.Content.ReadAsStreamAsync();
        using var doc = await JsonDocument.ParseAsync(body);
        Assert.Equal("Banana API", doc.RootElement.GetProperty("info").GetProperty("title").GetString());
    }

    [Fact]
    public void ApiDocsPageUsesSwaggerUiWithDefaultSpecContract()
    {
        var source = File.ReadAllText(Path.Combine(FindRepoRoot(), "src", "typescript", "react", "src", "pages", "ApiDocsPage.tsx"));

        Assert.Contains("FALLBACK_SPEC_URL = \"https://api.banana.engineer/swagger/v1/swagger.json\"", source, StringComparison.Ordinal);
        Assert.Contains("SwaggerUI url={specUrl}", source, StringComparison.Ordinal);
        Assert.Contains("Use API Base Default", source, StringComparison.Ordinal);
    }

    private static string FindRepoRoot()
    {
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        while (dir != null)
        {
            if (File.Exists(Path.Combine(dir.FullName, "Banana.sln")))
            {
                return dir.FullName;
            }

            dir = dir.Parent;
        }

        throw new InvalidOperationException("Could not locate repository root from test base directory.");
    }
}