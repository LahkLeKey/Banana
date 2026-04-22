using System.Text.Json;

using Banana.Api.Models;
using Banana.Api.NativeInterop;
using Banana.Api.Services;

using Xunit;

namespace Banana.UnitTests;

public sealed class NotBananaServiceTests
{
    [Fact]
    public void Classify_WithNullRequest_ThrowsClientInputException()
    {
        var service = new NotBananaService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() => service.Classify(null!));
    }

    [Fact]
    public void Classify_WithEmptyPayload_ThrowsClientInputException()
    {
        var service = new NotBananaService(new FakeNativeBananaClient());

        var ex = Assert.Throws<ClientInputException>(() => service.Classify(new NotBananaJunkRequest()));
        Assert.Contains("payload must include", ex.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Classify_WithNonObjectActor_ThrowsClientInputException()
    {
        var service = new NotBananaService(new FakeNativeBananaClient());

        var request = new NotBananaJunkRequest
        {
            Actors = [Element("7")]
        };

        var ex = Assert.Throws<ClientInputException>(() => service.Classify(request));
        Assert.Contains("actor entries must be JSON objects", ex.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Classify_WithActorMissingDiscriminator_ThrowsClientInputException()
    {
        var service = new NotBananaService(new FakeNativeBananaClient());

        var request = new NotBananaJunkRequest
        {
            Actors = [Element("{\"actorId\":\"A-1\"}")]
        };

        var ex = Assert.Throws<ClientInputException>(() => service.Classify(request));
        Assert.Contains("actor entries must include", ex.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Classify_WithNonObjectEntity_ThrowsClientInputException()
    {
        var service = new NotBananaService(new FakeNativeBananaClient());

        var request = new NotBananaJunkRequest
        {
            Entities = [Element("true")]
        };

        var ex = Assert.Throws<ClientInputException>(() => service.Classify(request));
        Assert.Contains("entity entries must be JSON objects", ex.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Classify_WithEntityMissingDiscriminator_ThrowsClientInputException()
    {
        var service = new NotBananaService(new FakeNativeBananaClient());

        var request = new NotBananaJunkRequest
        {
            Entities = [Element("{\"entityId\":\"E-1\"}")]
        };

        var ex = Assert.Throws<ClientInputException>(() => service.Classify(request));
        Assert.Contains("entity entries must include", ex.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Classify_NormalizesAndTokenizesPayload_AndReturnsMaybeBanana()
    {
        var fake = new FakeNativeBananaClient
        {
            NextClassification = new BananaNotBananaClassification(
                "BANANA",
                2,
                2,
                9,
                14,
                0.123456,
                0.876544,
                0.333351)
        };
        var service = new NotBananaService(fake);

        var request = new NotBananaJunkRequest
        {
            Actors =
            [
                Element("{\"actorType\":\"drone\",\"actorId\":\"A-1\",\"attrs\":{\"model\":\"A17\"}}")
            ],
            Actor = Element("{\"type\":\"picker\",\"id\":\"P-9\"}"),
            Entities =
            [
                Element("{\"entityType\":\"crate\",\"entityId\":\"C-2\"}")
            ],
            Entity = Element("{\"kind\":\"pallet\",\"entityKey\":\"PAL-7\"}"),
            Junk = Element("{\"freeText\":\"Banana??\",\"flag\":true,\"count\":5,\"items\":[\"x\",false] }"),
            Metadata = Element("{\"source\":\"mobile\",\"nested\":{\"key\":\"v\"}}")
        };

        var result = service.Classify(request);

        Assert.Equal("MAYBE_BANANA", result.Classification);
        Assert.Equal(0.1235, result.BananaProbability);
        Assert.Equal(0.8765, result.NotBananaProbability);
        Assert.Equal(0.3334, result.JunkConfidence);
        Assert.Equal(2, result.ActorCount);
        Assert.Equal(2, result.EntityCount);
        Assert.Equal(2, result.NormalizedActors.Count);
        Assert.Equal(2, result.NormalizedEntities.Count);
        Assert.Contains("manual review", result.Message, StringComparison.OrdinalIgnoreCase);

        Assert.Equal(2, fake.CapturedActorCount);
        Assert.Equal(2, fake.CapturedEntityCount);
        Assert.NotNull(fake.CapturedTokens);
        Assert.Contains("drone", fake.CapturedTokens!, StringComparer.Ordinal);
        Assert.Contains("banana", fake.CapturedTokens!, StringComparer.Ordinal);
        Assert.Contains("true", fake.CapturedTokens!, StringComparer.Ordinal);
        Assert.Contains("mobile", fake.CapturedTokens!, StringComparer.Ordinal);
    }

    [Fact]
    public void Classify_MapsIndeterminateToNotBanana()
    {
        var fake = new FakeNativeBananaClient
        {
            NextClassification = new BananaNotBananaClassification(
                "INDETERMINATE",
                1,
                0,
                2,
                3,
                0.41,
                0.59,
                0.88)
        };
        var service = new NotBananaService(fake);

        var request = new NotBananaJunkRequest
        {
            Actor = Element("{\"type\":\"bot\",\"id\":\"b-1\"}")
        };

        var result = service.Classify(request);

        Assert.Equal("NOT_BANANA", result.Classification);
        Assert.Contains("non-banana junk", result.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Classify_MapsUnknownNativeLabelToNotBanana()
    {
        var fake = new FakeNativeBananaClient
        {
            NextClassification = new BananaNotBananaClassification(
                "SOMETHING_ELSE",
                0,
                1,
                1,
                1,
                0.12,
                0.88,
                0.93)
        };
        var service = new NotBananaService(fake);

        var request = new NotBananaJunkRequest
        {
            Entity = Element("{\"kind\":\"crate\",\"entityKey\":\"c-1\"}")
        };

        var result = service.Classify(request);

        Assert.Equal("NOT_BANANA", result.Classification);
    }

    [Fact]
    public void Classify_StopsCollectingTokens_WhenDepthOrChildLimitsAreReached()
    {
        var fake = new FakeNativeBananaClient
        {
            NextClassification = new BananaNotBananaClassification("NOT_BANANA", 1, 0, 0, 0, 0.0, 1.0, 0.99)
        };
        var service = new NotBananaService(fake);

        var metadata = new Dictionary<string, object?>
        {
            ["deep"] = new Dictionary<string, object?>
            {
                ["a"] = new Dictionary<string, object?>
                {
                    ["b"] = new Dictionary<string, object?>
                    {
                        ["c"] = new Dictionary<string, object?>
                        {
                            ["d"] = new Dictionary<string, object?>
                            {
                                ["e"] = new Dictionary<string, object?>
                                {
                                    ["f"] = "too-deep-token"
                                }
                            }
                        }
                    }
                }
            }
        };

        for (var index = 0; index < 130; index++)
        {
            metadata[$"k{index}"] = index;
        }

        var arrayValues = Enumerable.Range(0, 130).Select(index => $"arr{index}").ToArray();

        var request = new NotBananaJunkRequest
        {
            Actors = [Element("{\"actorType\":\"scanner\"}")],
            Junk = Element(JsonSerializer.Serialize(arrayValues)),
            Metadata = Element(JsonSerializer.Serialize(metadata))
        };

        _ = service.Classify(request);

        Assert.NotNull(fake.CapturedTokens);
        Assert.Contains("arr127", fake.CapturedTokens!, StringComparer.Ordinal);
        Assert.DoesNotContain("arr128", fake.CapturedTokens!, StringComparer.Ordinal);
        Assert.Contains("k126", fake.CapturedTokens!, StringComparer.Ordinal);
        Assert.DoesNotContain("k127", fake.CapturedTokens!, StringComparer.Ordinal);
        Assert.DoesNotContain("too", fake.CapturedTokens!, StringComparer.Ordinal);
    }

    [Fact]
    public void Classify_StopsAddingStringTokens_WhenMaxTokenLimitIsReached()
    {
        var fake = new FakeNativeBananaClient
        {
            NextClassification = new BananaNotBananaClassification("BANANA", 1, 0, 0, 0, 0.99, 0.01, 0.02)
        };
        var service = new NotBananaService(fake);

        var hugeActorType = string.Join(' ', Enumerable.Range(0, 5000).Select(index => $"tok{index}"));

        var request = new NotBananaJunkRequest
        {
            Actors = [Element($"{{\"actorType\":\"{hugeActorType}\",\"actorId\":\"   \"}}")]
        };

        _ = service.Classify(request);

        Assert.NotNull(fake.CapturedTokens);
        Assert.Equal(4096, fake.CapturedTokens!.Count);
        Assert.Contains("tok0", fake.CapturedTokens!, StringComparer.Ordinal);
        Assert.DoesNotContain("tok4999", fake.CapturedTokens!, StringComparer.Ordinal);
    }

    private static JsonElement Element(string json)
    {
        using var doc = JsonDocument.Parse(json);
        return doc.RootElement.Clone();
    }

    private sealed class FakeNativeBananaClient : INativeBananaClient
    {
        public BananaNotBananaClassification NextClassification { get; set; } = new(
            "BANANA",
            0,
            0,
            0,
            0,
            0.9,
            0.1,
            0.05);

        public IReadOnlyList<string>? CapturedTokens { get; private set; }

        public int CapturedActorCount { get; private set; }

        public int CapturedEntityCount { get; private set; }

        public BananaNotBananaClassification ClassifyNotBananaJunk(IReadOnlyList<string> tokens, int actorCount, int entityCount)
        {
            CapturedTokens = tokens.ToArray();
            CapturedActorCount = actorCount;
            CapturedEntityCount = entityCount;
            return NextClassification;
        }

        public BananaResult Calculate(int purchases, int multiplier) => throw new NotImplementedException();

        public BananaBatchRecord CreateBatch(string batchId, string originFarm, double storageTempC, double ethyleneExposure, int estimatedShelfLifeDays) => throw new NotImplementedException();

        public BananaBatchRecord GetBatchStatus(string batchId) => throw new NotImplementedException();

        public BananaHarvestBatchRecord CreateHarvestBatch(string harvestBatchId, string fieldId, int harvestDayOrdinal) => throw new NotImplementedException();

        public BananaHarvestBatchRecord AddBunchToHarvestBatch(string harvestBatchId, string bunchId, int harvestDayOrdinal, double bunchWeightKg) => throw new NotImplementedException();

        public BananaHarvestBatchRecord GetHarvestBatchStatus(string harvestBatchId) => throw new NotImplementedException();

        public BananaTruckRecord RegisterTruck(string truckId, string nodeId, BananaDistributionNodeType nodeType, double latitude, double longitude, double capacityKg) => throw new NotImplementedException();

        public BananaTruckRecord LoadTruckContainer(string truckId, string containerId, double containerWeightKg) => throw new NotImplementedException();

        public BananaTruckRecord UnloadTruckContainer(string truckId, string containerId, double containerWeightKg) => throw new NotImplementedException();

        public BananaTruckRecord RelocateTruck(string truckId, string nodeId, BananaDistributionNodeType nodeType, double latitude, double longitude) => throw new NotImplementedException();

        public BananaTruckRecord GetTruckStatus(string truckId) => throw new NotImplementedException();

        public BananaRipenessPrediction PredictRipeness(IReadOnlyList<double> temperatureHistoryC, int daysSinceHarvest, double ethyleneExposure, double mechanicalDamage, double storageTempC) => throw new NotImplementedException();

        public BananaRipenessPrediction PredictBatchRipeness(string batchId, IReadOnlyList<double> temperatureHistoryC, int daysSinceHarvest, double mechanicalDamage) => throw new NotImplementedException();

        public double PredictBananaRegressionScore(IReadOnlyList<double> features) => throw new NotImplementedException();

        public BananaMlBinaryClassification PredictBananaBinaryClassification(IReadOnlyList<double> features) => throw new NotImplementedException();

        public BananaMlTransformerClassification PredictBananaTransformerClassification(IReadOnlyList<double> tokenFeatures) => throw new NotImplementedException();
    }
}