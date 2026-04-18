using Banana.Api.NativeInterop;
using Banana.Api.Services;

using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Xunit;

namespace Banana.UnitTests;

public sealed class NativeBananaClientTests
{
    private static readonly object Sync = new();
    private static bool IsConfigured;

    [Fact]
    public void Calculate_ReturnsExpectedResult_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new NativeBananaClient();

        var result = client.Calculate(10, 2);

        Assert.Equal(10, result.Purchases);
        Assert.Equal(2, result.Multiplier);
        Assert.Equal(150, result.Banana);
        Assert.Contains("banana_profile", result.Message, StringComparison.Ordinal);
        Assert.Contains("banana=150", result.Message, StringComparison.Ordinal);
    }

    [Fact]
    public void PredictRipeness_ReturnsExpectedResult_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new NativeBananaClient();

        var result = client.PredictRipeness([12.5, 13.0, 14.2], 5, 2.5, 0.1, 13.2);

        Assert.False(string.IsNullOrWhiteSpace(result.PredictedStage));
        Assert.True(result.ShelfLifeHours > 0);
        Assert.True(result.RipeningIndex > 0.0);
    }

    [Fact]
    public void CreateBatch_AndGetBatchStatus_ReturnExpectedResult_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new NativeBananaClient();
        var created = client.CreateBatch("batch-1", "farm-1", 13.2, 2.5, 3);
        var loaded = client.GetBatchStatus("batch-1");

        Assert.Equal("batch-1", created.BatchId);
        Assert.Equal("farm-1", loaded.OriginFarm);
    }

    [Fact]
    public void PredictBatchRipeness_ReturnsExpectedResult_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new NativeBananaClient();
        client.CreateBatch("batch-1", "farm-1", 13.2, 2.5, 3);

        var result = client.PredictBatchRipeness("batch-1", [12.5, 13.0, 14.2], 5, 0.1);

        Assert.True(result.ShelfLifeHours > 0);
        Assert.True(result.RipeningIndex > 0.0);
    }

    [Fact]
    public void Calculate_WithNegativeValues_ThrowsClientInputException_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new NativeBananaClient();

        Assert.Throws<ClientInputException>(() => client.Calculate(-1, 2));
    }

    [Fact]
    public void Calculate_WithOverflowValues_ThrowsClientInputException_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new NativeBananaClient();

        Assert.Throws<ClientInputException>(() => client.Calculate(214748365, 1));
    }

    private static bool EnsureNativePathConfigured()
    {
        lock (Sync)
        {
            if (IsConfigured)
            {
                return true;
            }

            var root = Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, "../../../../../../"));
            var fileName = NativeLibraryResolver.GetPlatformLibraryName();
            var candidates = new[]
            {
                Path.Combine(root, "build", "native", "bin", "Release"),
                Path.Combine(root, "build", "native", "bin")
            };

            var libraryDir = candidates.FirstOrDefault(path => File.Exists(Path.Combine(path, fileName)));
            if (libraryDir is null)
            {
                return false;
            }

            Environment.SetEnvironmentVariable("BANANA_NATIVE_PATH", libraryDir);

            var configuration = new ConfigurationBuilder().Build();
            using var loggerFactory = LoggerFactory.Create(_ => { });
            var logger = loggerFactory.CreateLogger("NativeBananaClientTests");

            try
            {
                NativeLibraryResolver.EnsureConfigured(configuration, logger);
            }
            catch (InvalidOperationException ex) when (ex.Message.Contains("resolver is already set", StringComparison.OrdinalIgnoreCase))
            {
            }

            IsConfigured = true;
            return true;
        }
    }
}
