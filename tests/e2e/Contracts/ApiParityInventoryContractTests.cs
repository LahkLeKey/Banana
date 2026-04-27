using System.Text.Json;

using Banana.Api.Controllers;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.E2eTests.Contracts;

public sealed class ApiParityInventoryContractTests
{
    [Fact]
    public void RipenessControllerContractStillExposesPredictEndpoint()
    {
        var route = typeof(RipenessController)
            .GetCustomAttributes(typeof(RouteAttribute), inherit: false)
            .Cast<RouteAttribute>()
            .Single();

        var predict = typeof(RipenessController)
            .GetMethod(nameof(RipenessController.Predict))!
            .GetCustomAttributes(typeof(HttpPostAttribute), inherit: false)
            .Cast<HttpPostAttribute>()
            .Single();

        Assert.Equal("ripeness", route.Template);
        Assert.Equal("predict", predict.Template);
    }

    [Fact]
    public void ParityInventoryArtifactHasRequiredContractFields()
    {
        var repoRoot = FindRepoRoot();
        var inventoryPath = Path.Combine(
            repoRoot,
            ".specify",
            "specs",
            "047-api-parity-governance",
            "artifacts",
            "overlapping-routes.inventory.json");

        Assert.True(File.Exists(inventoryPath), $"Missing inventory artifact: {inventoryPath}");

        using var document = JsonDocument.Parse(File.ReadAllText(inventoryPath));
        var root = document.RootElement;

        Assert.True(root.TryGetProperty("generated_at_utc", out var generatedAt));
        Assert.False(string.IsNullOrWhiteSpace(generatedAt.GetString()));

        Assert.True(root.TryGetProperty("routes", out var routes));
        Assert.Equal(JsonValueKind.Array, routes.ValueKind);
        Assert.NotEmpty(routes.EnumerateArray());

        foreach (var route in routes.EnumerateArray())
        {
            Assert.True(route.TryGetProperty("route_key", out _));
            Assert.True(route.TryGetProperty("method", out _));
            Assert.True(route.TryGetProperty("path", out _));
            Assert.True(route.TryGetProperty("expected_statuses", out _));
            Assert.True(route.TryGetProperty("required_response_fields", out _));
        }
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
