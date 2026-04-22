using System.Text.Json;

using Banana.Api.Models;
using Banana.Api.NativeInterop;

using Xunit;

namespace Banana.UnitTests;

public sealed class ModelAndRecordCoverageTests
{
    [Fact]
    public void NativeRecords_SupportValueSemantics_AndWithExpressions()
    {
        var truck = new BananaTruckRecord("truck-1", "node-1", "WAREHOUSE", 1.2, 2.3, 100.0, 12.5, 1);
        var truckCopy = truck with { ContainerCount = 2 };

        Assert.NotEqual(truck, truckCopy);
        Assert.Contains("truck-1", truck.ToString(), StringComparison.Ordinal);

        var harvest = new BananaHarvestBatchRecord("harvest-1", "field-1", 7, 2, 18.6);
        var harvestCopy = harvest with { BunchCount = 3 };

        Assert.NotEqual(harvest, harvestCopy);
        Assert.Contains("harvest-1", harvest.ToString(), StringComparison.Ordinal);

        var classification = new BananaNotBananaClassification(
            "NOT_BANANA",
            2,
            3,
            11,
            20,
            0.2,
            0.8,
            0.95);
        var classificationCopy = classification with { PredictedLabel = "BANANA" };

        Assert.NotEqual(classification, classificationCopy);
        Assert.Contains("NOT_BANANA", classification.ToString(), StringComparison.Ordinal);
    }

    [Fact]
    public void RipenessResponse_RecordSupportsValueSemantics()
    {
        var response = new BananaRipenessResponse("batch-1", "YELLOW", 48, 190.0, 0.22, 0.17);
        var copy = response with { PredictedStage = "SPOTTED" };

        Assert.NotEqual(response, copy);
        Assert.Contains("batch-1", response.ToString(), StringComparison.Ordinal);
    }

    [Fact]
    public void NotBananaContracts_CanBeInstantiatedAndRead()
    {
        var actorElement = Element("{\"type\":\"picker\",\"id\":\"P-1\"}");
        var entityElement = Element("{\"kind\":\"crate\",\"entityKey\":\"C-1\"}");
        var junkElement = Element("{\"freeText\":\"hello\"}");
        var metadataElement = Element("{\"source\":\"api\"}");

        var request = new NotBananaJunkRequest
        {
            Actors = [actorElement],
            Entities = [entityElement],
            Actor = actorElement,
            Entity = entityElement,
            Junk = junkElement,
            Metadata = metadataElement
        };

        Assert.NotNull(request.Actors);
        Assert.NotNull(request.Entities);
        Assert.Equal(JsonValueKind.Object, request.Junk!.Value.ValueKind);

        var response = new NotBananaJunkResponse(
            "NOT_BANANA",
            0.1,
            0.9,
            0.95,
            1,
            1,
            [new NotBananaPolymorphicActor("worker", "w-1")],
            [new NotBananaPolymorphicEntity("crate", "c-1")],
            "ok");

        Assert.Equal("NOT_BANANA", response.Classification);
        Assert.Single(response.NormalizedActors);
        Assert.Single(response.NormalizedEntities);
    }

    private static JsonElement Element(string json)
    {
        using var doc = JsonDocument.Parse(json);
        return doc.RootElement.Clone();
    }
}