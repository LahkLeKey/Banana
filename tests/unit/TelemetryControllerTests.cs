using System.Net;
using System.Net.Http.Json;
using System.Text.Json;

using Microsoft.AspNetCore.Mvc.Testing;

using Xunit;

namespace Banana.Api.Tests.Unit;

public sealed class TelemetryControllerTests : IClassFixture<WebApplicationFactory<Program>>, IDisposable
{
    private readonly WebApplicationFactory<Program> _factory;
    private readonly string? _priorBananaPgConnection;

    public TelemetryControllerTests(WebApplicationFactory<Program> factory)
    {
        _priorBananaPgConnection = Environment.GetEnvironmentVariable("BANANA_PG_CONNECTION");
        _factory = factory.WithWebHostBuilder(_ =>
        {
            Environment.SetEnvironmentVariable("BANANA_PG_CONNECTION", null);
        });
    }

    public void Dispose()
    {
        Environment.SetEnvironmentVariable("BANANA_PG_CONNECTION", _priorBananaPgConnection);
    }

    [Fact]
    public async Task TelemetryConfig_ReturnsSampleRateContract()
    {
        var client = _factory.CreateClient();

        var response = await client.GetAsync("/operator/telemetry/config");

        Assert.Equal(HttpStatusCode.OK, response.StatusCode);
        var doc = JsonDocument.Parse(await response.Content.ReadAsStringAsync());
        Assert.True(doc.RootElement.TryGetProperty("sample_rate", out var sampleRate));
        Assert.True(sampleRate.GetInt32() >= 0);
        Assert.Equal("percent", doc.RootElement.GetProperty("unit").GetString());
    }

    [Fact]
    public async Task TelemetryEvents_IngestThenRead_ReturnsPersistedEvent()
    {
        var client = _factory.CreateClient();
        var now = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();

        var ingestPayload = new
        {
            source = "api",
            @event = "api.health.check.ok",
            timestamp = now,
            status = "ok",
            durationMs = 12.5,
            code = (int?)null,
            channel = "web",
            variant = (string?)null,
            layer = (string?)null,
            details = new Dictionary<string, object?>
            {
                ["status"] = "ok",
                ["probe"] = true,
            },
        };

        var postResponse = await client.PostAsJsonAsync("/operator/telemetry/events", ingestPayload);
        Assert.Equal(HttpStatusCode.Accepted, postResponse.StatusCode);

        var getResponse = await client.GetAsync("/operator/telemetry/events?limit=20&source=api");
        Assert.Equal(HttpStatusCode.OK, getResponse.StatusCode);

        var doc = JsonDocument.Parse(await getResponse.Content.ReadAsStringAsync());
        var root = doc.RootElement;
        Assert.True(root.GetProperty("count").GetInt32() >= 1);
        var backend = root.GetProperty("backend").GetString();
        Assert.True(backend is "memory" or "postgres");

        var events = root.GetProperty("events");
        Assert.True(events.GetArrayLength() >= 1);
        var first = events[0];
        Assert.Equal("api", first.GetProperty("source").GetString());
        Assert.Equal("api.health.check.ok", first.GetProperty("event").GetString());
        Assert.Equal("ok", first.GetProperty("status").GetString());
    }
}