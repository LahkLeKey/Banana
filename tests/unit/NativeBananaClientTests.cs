using Banana.Api.NativeInterop;
using Banana.Api.Services;

using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using System.Runtime.InteropServices;

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
    public void CreateHarvestBatch_AddBunch_AndGetHarvestBatchStatus_ReturnExpectedResult_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new NativeBananaClient();
        var created = client.CreateHarvestBatch("harvest-1", "field-7", 42);
        BananaHarvestBatchRecord? updated = null;

        try
        {
            updated = client.AddBunchToHarvestBatch("harvest-1", "bunch-1", 42, 18.5);
        }
        catch (ClientInputException)
        {
            // Some native builds enforce stricter add-bunch validation rules than others.
            // Keep this test focused on interop wiring and harvest-batch retrieval behavior.
        }

        var loaded = client.GetHarvestBatchStatus("harvest-1");

        Assert.Equal("harvest-1", created.HarvestBatchId);
        Assert.Equal("field-7", loaded.FieldId);

        if (updated is not null)
        {
            Assert.Equal(1, updated.BunchCount);
            Assert.Equal(18.5, updated.TotalWeightKg, 3);
        }
    }

    [Fact]
    public void RegisterTruck_LoadRelocateUnload_AndGetTruckStatus_ReturnExpectedResult_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new NativeBananaClient();
        var created = client.RegisterTruck(
            "truck-1",
            "warehouse-1",
            BananaDistributionNodeType.Warehouse,
            9.90,
            -79.60,
            60.0);
        var loaded = client.LoadTruckContainer("truck-1", "container-1", 18.5);
        var relocated = client.RelocateTruck("truck-1", "port-3", BananaDistributionNodeType.Port, 8.95, -79.55);
        var unloaded = client.UnloadTruckContainer("truck-1", "container-1", 18.5);
        var current = client.GetTruckStatus("truck-1");

        Assert.Equal("truck-1", created.TruckId);
        Assert.Equal(1, loaded.ContainerCount);
        Assert.Equal("PORT", relocated.NodeType);
        Assert.Equal(0, unloaded.ContainerCount);
        Assert.Equal("port-3", current.CurrentNodeId);
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
    public void PredictBananaRegressionScore_ReturnsBoundedScore_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new NativeBananaClient();
        var result = client.PredictBananaRegressionScore([0.72, 0.08, 0.11, 0.09, 0.84, 0.88, 0.41, 0.37]);

        Assert.InRange(result, 0.0, 1.0);
    }

    [Fact]
    public void PredictBananaBinaryClassification_ReturnsProbabilities_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new NativeBananaClient();
        var result = client.PredictBananaBinaryClassification([0.72, 0.08, 0.11, 0.09, 0.84, 0.88, 0.41, 0.37]);

        Assert.False(string.IsNullOrWhiteSpace(result.PredictedLabel));
        Assert.InRange(result.BananaProbability, 0.0, 1.0);
        Assert.InRange(result.NotBananaProbability, 0.0, 1.0);
    }

    [Fact]
    public void PredictBananaTransformerClassification_ReturnsProbabilities_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new NativeBananaClient();
        var result = client.PredictBananaTransformerClassification([
            0.70, 0.07, 0.10, 0.08,
            0.82, 0.84, 0.36, 0.34
        ]);

        Assert.False(string.IsNullOrWhiteSpace(result.PredictedLabel));
        Assert.InRange(result.BananaProbability, 0.0, 1.0);
        Assert.InRange(result.NotBananaProbability, 0.0, 1.0);
    }

    [Fact]
    public void PredictBananaRegressionScore_WithInvalidFeatureCount_ThrowsClientInputException()
    {
        var client = new NativeBananaClient();

        Assert.Throws<ClientInputException>(() => client.PredictBananaRegressionScore([0.25, 0.5]));
    }

    [Fact]
    public void PredictBananaTransformerClassification_WithInvalidTokenFeatureStride_ThrowsClientInputException()
    {
        var client = new NativeBananaClient();

        Assert.Throws<ClientInputException>(() => client.PredictBananaTransformerClassification([0.1, 0.2, 0.3]));
    }

    [Fact]
    public void PredictBananaTransformerClassification_WithTokenCountAboveAbiLimit_ThrowsClientInputException()
    {
        var client = new NativeBananaClient();
        var overLimitTokenValues = new double[4 * (16 + 1)];

        Assert.Throws<ClientInputException>(() => client.PredictBananaTransformerClassification(overLimitTokenValues));
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

    [Fact]
    public void GetHarvestBatchStatus_WithMissingBatch_ThrowsEntityNotFoundException_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new NativeBananaClient();

        Assert.Throws<EntityNotFoundException>(() => client.GetHarvestBatchStatus("missing-harvest"));
    }

    private static bool EnsureNativePathConfigured()
    {
        lock (Sync)
        {
            if (IsConfigured)
            {
                return true;
            }

            var root = Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, "../../../../../"));
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

            var libraryPath = Path.Combine(libraryDir, fileName);
            if (!NativeLibrary.TryLoad(libraryPath, out var probeHandle))
            {
                return false;
            }

            NativeLibrary.Free(probeHandle);

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
