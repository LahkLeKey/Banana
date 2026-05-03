using System.Net;
using System.Text.Json;

using Microsoft.AspNetCore.Mvc.Testing;

using Xunit;

namespace Banana.Api.Tests.Unit;

public sealed class SwaggerIntegrationTests(WebApplicationFactory<Program> factory) : IClassFixture<WebApplicationFactory<Program>>
{
    private readonly HttpClient _client = factory.CreateClient();

    [Fact]
    public async Task OpenApiJsonEndpoint_Returns200_WithValidDocumentShape()
    {
        using var response = await _client.GetAsync("/swagger/v1/swagger.json");

        Assert.Equal(HttpStatusCode.OK, response.StatusCode);
        Assert.Contains("json", response.Content.Headers.ContentType?.MediaType ?? string.Empty, StringComparison.OrdinalIgnoreCase);

        await using var stream = await response.Content.ReadAsStreamAsync();
        using var doc = await JsonDocument.ParseAsync(stream);
        var root = doc.RootElement;

        Assert.True(root.TryGetProperty("openapi", out var openapiVersion));
        Assert.StartsWith("3.", openapiVersion.GetString() ?? string.Empty, StringComparison.Ordinal);

        Assert.True(root.TryGetProperty("info", out var info));
        Assert.Equal("Banana API", info.GetProperty("title").GetString());
    }

    [Fact]
    public async Task OpenApiJsonEndpoint_ReturnsCorsHeader_ForDocsOrigin()
    {
        const string origin = "https://banana.engineer";
        using var request = new HttpRequestMessage(HttpMethod.Get, "/swagger/v1/swagger.json");
        request.Headers.Add("Origin", origin);

        using var response = await _client.SendAsync(request);

        Assert.Equal(HttpStatusCode.OK, response.StatusCode);
        Assert.True(response.Headers.TryGetValues("Access-Control-Allow-Origin", out var values));
        Assert.Equal(origin, Assert.Single(values));
    }
}